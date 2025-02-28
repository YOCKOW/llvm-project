//===-- SwiftFormatters.cpp -------------------------------------*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "SwiftFormatters.h"
#include "Plugins/Language/Swift/SwiftStringIndex.h"
#include "Plugins/LanguageRuntime/Swift/ReflectionContextInterface.h"
#include "Plugins/LanguageRuntime/Swift/SwiftLanguageRuntime.h"
#include "Plugins/TypeSystem/Clang/TypeSystemClang.h"
#include "lldb/DataFormatters/FormattersHelpers.h"
#include "lldb/DataFormatters/StringPrinter.h"
#include "lldb/Symbol/CompilerType.h"
#include "lldb/Target/Process.h"
#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/DataBufferHeap.h"
#include "lldb/Utility/LLDBLog.h"
#include "lldb/Utility/Log.h"
#include "lldb/Utility/Status.h"
#include "lldb/Utility/Timer.h"
#include "lldb/ValueObject/ValueObject.h"
#include "lldb/lldb-enumerations.h"
#include "swift/AST/Types.h"
#include "swift/Demangling/ManglingMacros.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/FormatAdapters.h"
#include "llvm/Support/raw_ostream.h"
#include <optional>

// FIXME: we should not need this
#include "Plugins/Language/CPlusPlus/CxxStringTypes.h"
#include "Plugins/Language/ObjC/Cocoa.h"
#include "Plugins/Language/ObjC/NSString.h"

using namespace lldb;
using namespace lldb_private;
using namespace lldb_private::formatters;
using namespace lldb_private::formatters::swift;
using namespace llvm;

bool lldb_private::formatters::swift::Character_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  static ConstString g__str("_str");

  ValueObjectSP str_sp = valobj.GetChildMemberWithName(g__str, true);
  if (!str_sp)
    return false;

  return String_SummaryProvider(*str_sp, stream, options);
}

bool lldb_private::formatters::swift::UnicodeScalar_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  static ConstString g_value("_value");
  ValueObjectSP value_sp(valobj.GetChildMemberWithName(g_value, true));
  if (!value_sp)
    return false;
  return Char32SummaryProvider(*value_sp.get(), stream, options);
}

bool lldb_private::formatters::swift::StringGuts_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return StringGuts_SummaryProvider(
      valobj, stream, options,
      StringPrinter::ReadStringAndDumpToStreamOptions());
}

bool lldb_private::formatters::swift::SwiftSharedString_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return SwiftSharedString_SummaryProvider_2(
      valobj, stream, options,
      StringPrinter::ReadStringAndDumpToStreamOptions());
}

struct StringSlice {
  uint64_t start, end;
};

template <typename AddrT>
static void applySlice(AddrT &address, uint64_t &length,
                       std::optional<StringSlice> slice) {
  if (!slice)
    return;

  // No slicing is performed when the slice starts beyond the string's bounds.
  if (slice->start > length)
    return;

  // The slicing logic does handle the corner case where slice->start == length.

  auto offset = slice->start;
  auto slice_length = slice->end - slice->start;

  // Adjust from the start.
  address += offset;
  length -= offset;

  // Reduce to the slice length, unless it's larger than the remaining length.
  length = std::min(slice_length, length);
}

static bool readStringFromAddress(
    uint64_t startAddress, uint64_t length, ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options,
    StringPrinter::ReadStringAndDumpToStreamOptions read_options) {
  if (length == 0) {
    stream.Printf("\"\"");
    return true;
  }

  read_options.SetLocation(startAddress);
  read_options.SetTargetSP(valobj.GetTargetSP());
  read_options.SetStream(&stream);
  read_options.SetSourceSize(length);
  read_options.SetHasSourceSize(true);
  read_options.SetNeedsZeroTermination(false);
  read_options.SetIgnoreMaxLength(summary_options.GetCapping() ==
                                  lldb::eTypeSummaryUncapped);
  read_options.SetBinaryZeroIsTerminator(false);
  read_options.SetEscapeStyle(StringPrinter::EscapeStyle::Swift);

  return StringPrinter::ReadStringAndDumpToStream<
      StringPrinter::StringElementType::UTF8>(read_options);
};

static bool makeStringGutsSummary(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options,
    StringPrinter::ReadStringAndDumpToStreamOptions read_options,
    std::optional<StringSlice> slice = std::nullopt) {
  LLDB_SCOPED_TIMER();

  static ConstString g__object("_object");
  static ConstString g__storage("_storage");
  static ConstString g__value("_value");

  auto error = [&](std::string message) {
    stream << "<cannot decode string: " << message << ">";
    return true;
  };

  ProcessSP process(valobj.GetProcessSP());
  if (!process)
    return error("no live process");

  auto ptrSize = process->GetAddressByteSize();

  auto object_sp = valobj.GetChildMemberWithName(g__object, true);
  if (!object_sp)
    return error("unexpected layout");

  // We retrieve String contents by first extracting the
  // platform-independent 128-bit raw value representation from
  // _StringObject, then interpreting that.
  Status status;
  uint64_t raw0;
  uint64_t raw1;

  if (ptrSize == 8) {
    // On 64-bit platforms, we simply need to get the raw integer
    // values of the two stored properties.
    static ConstString g__countAndFlagsBits("_countAndFlagsBits");

    auto countAndFlagsBits = object_sp->GetChildAtNamePath(
      {g__countAndFlagsBits, g__value});
    if (!countAndFlagsBits)
      return error("unexpected layout");
    raw0 = countAndFlagsBits->GetValueAsUnsigned(0);

    auto object = object_sp->GetChildMemberWithName(g__object, true);
    if (!object)
      return error("unexpected layout (object)");
    raw1 = object->GetValueAsUnsigned(0);
  } else if (ptrSize == 4) {
    // On 32-bit platforms, we emulate what `_StringObject.rawBits`
    // does. It involves inspecting the variant and rearranging bits
    // to match the 64-bit representation.
    static ConstString g__count("_count");
    static ConstString g__variant("_variant");
    static ConstString g__discriminator("_discriminator");
    static ConstString g__flags("_flags");
    static ConstString g_immortal("immortal");

    auto count_sp = object_sp->GetChildAtNamePath({g__count, g__value});
    if (!count_sp)
      return error("unexpected layout (count)");
    uint64_t count = count_sp->GetValueAsUnsigned(0);

    auto discriminator_sp =
        object_sp->GetChildAtNamePath({g__discriminator, g__value});
    if (!discriminator_sp)
      return error("unexpected layout (discriminator)");
    uint64_t discriminator = discriminator_sp->GetValueAsUnsigned(0) & 0xff;

    auto flags_sp = object_sp->GetChildAtNamePath({g__flags, g__value});
    if (!flags_sp)
      return error("unexpected layout (flags)");
    uint64_t flags = flags_sp->GetValueAsUnsigned(0) & 0xffff;

    auto variant_sp = object_sp->GetChildMemberWithName(g__variant, true);
    if (!variant_sp)
      return error("unexpected layout (variant)");

    llvm::StringRef variantCase = variant_sp->GetValueAsCString();

    ValueObjectSP payload_sp;
    if (variantCase.starts_with("immortal")) {
      payload_sp = variant_sp->GetChildAtNamePath({g_immortal, g__value});
    } else if (variantCase.starts_with("native")) {
      payload_sp = variant_sp->GetChildAtNamePath({g_immortal, g__value});
    } else if (variantCase.starts_with("bridged")) {
      static ConstString g_bridged("bridged");
      auto anyobject_sp = variant_sp->GetChildMemberWithName(g_bridged, true);
      if (!anyobject_sp)
        return error("unexpected layout (bridged)");
      payload_sp = anyobject_sp->GetChildAtIndex(0, true); // "instance"
    } else {
      return error("unknown variant");
    }
    if (!payload_sp)
      return error("no payload");

    uint64_t pointerBits = payload_sp->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);

    if (pointerBits == LLDB_INVALID_ADDRESS)
      return error("invalid payload");

    if ((discriminator & 0xB0) == 0xA0) {
      raw0 = count | (pointerBits << 32);
      raw1 = flags | (discriminator << 56);
    } else {
      raw0 = count | (flags << 48);
      raw1 = pointerBits | (discriminator << 56);
    }
  } else {
    return error("unsupported pointer size");
  }

  // Copied from StringObject.swift
  //
  // TODO: Hyperlink to final set of documentation diagrams instead
  //
  /*
  On 64-bit platforms, the discriminator is the most significant 4 bits of the
  bridge object.

  ┌─────────────────────╥─────┬─────┬─────┬─────┐
  │ Form                ║ b63 │ b62 │ b61 │ b60 │
  ╞═════════════════════╬═════╪═════╪═════╪═════╡
  │ Immortal, Small     ║  1  │ASCII│  1  │  0  │
  ├─────────────────────╫─────┼─────┼─────┼─────┤
  │ Immortal, Large     ║  1  │  0  │  0  │  0  │
  ├─────────────────────╫─────┼─────┼─────┼─────┤
  │ Immortal, Bridged   ║  1  │  1  │  0  │  0  │
  ╞═════════════════════╬═════╪═════╪═════╪═════╡
  │ Native              ║  0  │  0  │  0  │  0  │
  ├─────────────────────╫─────┼─────┼─────┼─────┤
  │ Shared              ║  x  │  0  │  0  │  0  │
  ├─────────────────────╫─────┼─────┼─────┼─────┤
  │ Shared, Bridged     ║  0  │  1  │  0  │  0  │
  ╞═════════════════════╬═════╪═════╪═════╪═════╡
  │ Foreign             ║  x  │  0  │  0  │  1  │
  ├─────────────────────╫─────┼─────┼─────┼─────┤
  │ Foreign, Bridged    ║  0  │  1  │  0  │  1  │
  └─────────────────────╨─────┴─────┴─────┴─────┘

  b63: isImmortal: Should the Swift runtime skip ARC
    - Small strings are just values, always immortal
    - Large strings can sometimes be immortal, e.g. literals
  b62: (large) isBridged / (small) isASCII
    - For large strings, this means lazily-bridged NSString: perform ObjC ARC
    - Small strings repurpose this as a dedicated bit to remember ASCII-ness
  b61: isSmall: Dedicated bit to denote small strings
  b60: isForeign: aka isSlow, cannot provide access to contiguous UTF-8

 All non-small forms share the same structure for the other half of the bits
 (i.e. non-object bits) as a word containing code unit count and various
 performance flags. The top 16 bits are for performance flags, which are not
 semantically relevant but communicate that some operations can be done more
 efficiently on this particular string, and the lower 48 are the code unit
 count (aka endIndex).

┌─────────┬───────┬──────────────────┬─────────────────┬────────┬───────┐
│   b63   │  b62  │       b61        │       b60       │ b59:48 │ b47:0 │
├─────────┼───────┼──────────────────┼─────────────────┼────────┼───────┤
│ isASCII │ isNFC │ isNativelyStored │ isTailAllocated │  TBD   │ count │
└─────────┴───────┴──────────────────┴─────────────────┴────────┴───────┘

 isASCII: set when all code units are known to be ASCII, enabling:
   - Trivial Unicode scalars, they're just the code units
   - Trivial UTF-16 transcoding (just bit-extend)
   - Also, isASCII always implies isNFC
 isNFC: set when the contents are in normal form C
   - Enables trivial lexicographical comparisons: just memcmp
   - `isASCII` always implies `isNFC`, but not vice versa
 isNativelyStored: set for native stored strings
   - `largeAddressBits` holds an instance of `_StringStorage`.
   - I.e. the start of the code units is at the stored address + `nativeBias`
 isTailAllocated: start of the code units is at the stored address + `nativeBias`
   - `isNativelyStored` always implies `isTailAllocated`, but not vice versa
      (e.g. literals)
 TBD: Reserved for future usage
   - Setting a TBD bit to 1 must be semantically equivalent to 0
   - I.e. it can only be used to "cache" fast-path information in the future
 count: stores the number of code units, corresponds to `endIndex`.
  */

  uint8_t discriminator = raw1 >> 56;

  if ((discriminator & 0b1011'0000) == 0b1010'0000) { // 1x10xxxx: Small string
    uint64_t count = (raw1 >> 56) & 0b1111;
    uint64_t maxCount = (ptrSize == 8 ? 15 : 10);
    if (count > maxCount)
      return error("count > maxCount");

    uint64_t rawBuffer[2] = {raw0, raw1};
    auto *buffer = (uint8_t *)&rawBuffer;
    applySlice(buffer, count, slice);

    StringPrinter::ReadBufferAndDumpToStreamOptions options(read_options);
    options.SetData(lldb_private::DataExtractor(
        buffer, count, process->GetByteOrder(), ptrSize));
    options.SetStream(&stream);
    options.SetSourceSize(count);
    options.SetBinaryZeroIsTerminator(false);
    options.SetEscapeStyle(StringPrinter::EscapeStyle::Swift);
    return StringPrinter::ReadBufferAndDumpToStream<
        StringPrinter::StringElementType::UTF8>(options);
  }

  uint64_t count = raw0 & 0x0000FFFFFFFFFFFF;
  uint16_t flags = raw0 >> 48;
  lldb::addr_t objectAddress = (raw1 & 0x0FFFFFFFFFFFFFFF);
  // Catch a zero-initialized string.
  if (!objectAddress) {
    stream << "<uninitialized>";
    return true;
  }

  if ((flags & 0x1000) != 0) { // Tail-allocated / biased address
    // Tail-allocation is only for natively stored or literals.
    if ((discriminator & 0b0111'0000) != 0)
      return error("unexpected discriminator");
    uint64_t bias = (ptrSize == 8 ? 32 : 20);
    auto address = objectAddress + bias;
    applySlice(address, count, slice);
    return readStringFromAddress(
      address, count, valobj, stream, summary_options, read_options);
  }

  if ((discriminator & 0b1111'0000) == 0) { // Shared string
    // FIXME: Verify that there is a __SharedStringStorage instance at `address`.
    // Shared strings must not be tail-allocated or natively stored.
    if ((flags & 0x3000) != 0)
      return false;
    uint64_t startOffset = (ptrSize == 8 ? 24 : 12);
    auto address = objectAddress + startOffset;
    lldb::addr_t start = process->ReadPointerFromMemory(address, status);
    if (status.Fail())
      return error(status.AsCString());

    applySlice(address, count, slice);
    return readStringFromAddress(
      start, count, valobj, stream, summary_options, read_options);
  }

  // Native/shared strings should already have been handled.
  if ((discriminator & 0b0111'0000) == 0)
    return error("unexpected discriminator");

  if ((discriminator & 0b0110'0000) == 0b0100'0000) { // x10xxxxx: Bridged
    TypeSystemClangSP clang_ts_sp =
        ScratchTypeSystemClang::GetForTarget(process->GetTarget());
    if (!clang_ts_sp)
      return error("no Clang type system");

    CompilerType id_type = clang_ts_sp->GetBasicType(lldb::eBasicTypeObjCID);

    // We may have an NSString pointer inline, so try formatting it directly.
    lldb_private::DataExtractor DE(&objectAddress, ptrSize,
                                   process->GetByteOrder(), ptrSize);
    auto nsstring = ValueObject::CreateValueObjectFromData(
        "nsstring", DE, valobj.GetExecutionContextRef(), id_type);
    if (!nsstring || nsstring->GetError().Fail())
      return error("could not create NSString value object");

    return NSStringSummaryProvider(*nsstring.get(), stream, summary_options);
  }

  if ((discriminator & 0b1111'1000) == 0b0001'1000) { // 0001xxxx: Foreign
    // Not currently generated: Foreign non-bridged strings are not currently
    // used in Swift.
    return error("unexpected discriminator");
  }

  // Invalid discriminator.
  return error("invalid discriminator");
}

bool lldb_private::formatters::swift::StringGuts_SummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options,
    StringPrinter::ReadStringAndDumpToStreamOptions read_options) {
  return makeStringGutsSummary(valobj, stream, summary_options, read_options);
}

bool lldb_private::formatters::swift::String_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return String_SummaryProvider(
      valobj, stream, options,
      StringPrinter::ReadStringAndDumpToStreamOptions());
}

bool lldb_private::formatters::swift::String_SummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options,
    StringPrinter::ReadStringAndDumpToStreamOptions read_options) {
  static ConstString g_guts("_guts");
  ValueObjectSP guts_sp = valobj.GetChildMemberWithName(g_guts, true);
  if (guts_sp)
    return StringGuts_SummaryProvider(*guts_sp, stream, summary_options,
                                      read_options);
  return false;
}

bool lldb_private::formatters::swift::Substring_SummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options) {
  static ConstString g__slice("_slice");
  static ConstString g__base("_base");
  static ConstString g__startIndex("_startIndex");
  static ConstString g__endIndex("_endIndex");
  static ConstString g__rawBits("_rawBits");
  auto slice_sp = valobj.GetChildMemberWithName(g__slice, true);
  if (!slice_sp)
    return false;
  auto base_sp = slice_sp->GetChildMemberWithName(g__base, true);
  if (!base_sp)
    return false;

  auto get_index =
      [&slice_sp](ConstString index_name) -> std::optional<StringIndex> {
    auto raw_bits_sp = slice_sp->GetChildAtNamePath({index_name, g__rawBits});
    if (!raw_bits_sp)
      return std::nullopt;
    bool success = false;
    StringIndex index =
        raw_bits_sp->GetSyntheticValue()->GetValueAsUnsigned(0, &success);
    if (!success)
      return std::nullopt;
    return index;
  };

  std::optional<StringIndex> start_index = get_index(g__startIndex);
  std::optional<StringIndex> end_index = get_index(g__endIndex);
  if (!start_index || !end_index)
    return false;

  if (!start_index->matchesEncoding(*end_index))
    return false;

  static ConstString g_guts("_guts");
  auto guts_sp = base_sp->GetChildMemberWithName(g_guts, true);
  if (!guts_sp)
    return false;

  StringPrinter::ReadStringAndDumpToStreamOptions read_options;
  StringSlice slice{start_index->encodedOffset(), end_index->encodedOffset()};
  return makeStringGutsSummary(*guts_sp, stream, summary_options, read_options,
                               slice);
}

bool lldb_private::formatters::swift::StringIndex_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  static ConstString g__rawBits("_rawBits");
  auto raw_bits_sp = valobj.GetChildMemberWithName(g__rawBits, true);
  if (!raw_bits_sp)
    return false;

  bool success = false;
  StringIndex index =
      raw_bits_sp->GetSyntheticValue()->GetValueAsUnsigned(0, &success);
  if (!success)
    return false;

  stream.Printf("%llu[%s]", index.encodedOffset(), index.encodingName());
  if (index.transcodedOffset() != 0)
    stream.Printf("+%u", index.transcodedOffset());

  return true;
}

bool lldb_private::formatters::swift::StaticString_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return StaticString_SummaryProvider(
      valobj, stream, options,
      StringPrinter::ReadStringAndDumpToStreamOptions());
}

bool lldb_private::formatters::swift::StaticString_SummaryProvider(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options,
    StringPrinter::ReadStringAndDumpToStreamOptions read_options) {
  LLDB_SCOPED_TIMER();

  static ConstString g__startPtrOrData("_startPtrOrData");
  static ConstString g__byteSize("_utf8CodeUnitCount");
  static ConstString g__flags("_flags");

  ValueObjectSP flags_sp(valobj.GetChildMemberWithName(g__flags, true));
  if (!flags_sp)
    return false;

  ProcessSP process_sp(valobj.GetProcessSP());
  if (!process_sp)
    return false;

  // 0 == pointer representation
  InferiorSizedWord flags(flags_sp->GetValueAsUnsigned(0), *process_sp);
  if (0 != (flags & 0x1).GetValue())
    return false;

  ValueObjectSP startptr_sp(
      valobj.GetChildMemberWithName(g__startPtrOrData, true));
  ValueObjectSP bytesize_sp(valobj.GetChildMemberWithName(g__byteSize, true));
  if (!startptr_sp || !bytesize_sp)
    return false;

  lldb::addr_t start_ptr =
      startptr_sp->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);
  uint64_t size = bytesize_sp->GetValueAsUnsigned(0);

  if (start_ptr == LLDB_INVALID_ADDRESS || start_ptr == 0)
    return false;

  if (size == 0) {
    stream.Printf("\"\"");
    return true;
  }

  read_options.SetTargetSP(valobj.GetTargetSP());
  read_options.SetLocation(start_ptr);
  read_options.SetSourceSize(size);
  read_options.SetHasSourceSize(true);
  read_options.SetBinaryZeroIsTerminator(false);
  read_options.SetNeedsZeroTermination(false);
  read_options.SetStream(&stream);
  read_options.SetIgnoreMaxLength(summary_options.GetCapping() ==
                                  lldb::eTypeSummaryUncapped);
  read_options.SetEscapeStyle(StringPrinter::EscapeStyle::Swift);

  return StringPrinter::ReadStringAndDumpToStream<
      StringPrinter::StringElementType::UTF8>(read_options);
}

bool lldb_private::formatters::swift::SwiftSharedString_SummaryProvider_2(
    ValueObject &valobj, Stream &stream,
    const TypeSummaryOptions &summary_options,
    StringPrinter::ReadStringAndDumpToStreamOptions read_options) {
  LLDB_SCOPED_TIMER();
  ProcessSP process(valobj.GetProcessSP());
  if (!process)
    return false;

  Status error;
  auto ptr_size = process->GetAddressByteSize();

  lldb::addr_t raw1 = valobj.GetPointerValue();
  lldb::addr_t address = (raw1 & 0x00FFFFFFFFFFFFFF);
  uint64_t startOffset = (ptr_size == 8 ? 24 : 12);

  lldb::addr_t start =
      process->ReadPointerFromMemory(address + startOffset, error);
  if (error.Fail())
    return false;
  lldb::addr_t raw0 =
      process->ReadPointerFromMemory(address + startOffset + ptr_size, error);
  if (error.Fail())
    return false;

  uint64_t count = raw0 & 0x0000FFFFFFFFFFFF;

  return readStringFromAddress(start, count, valobj, stream, summary_options,
                               read_options);
}

bool lldb_private::formatters::swift::SwiftStringStorage_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  LLDB_SCOPED_TIMER();
  ProcessSP process(valobj.GetProcessSP());
  if (!process)
    return false;
  auto ptrSize = process->GetAddressByteSize();
  uint64_t bias = (ptrSize == 8 ? 32 : 20);
  uint64_t raw0_offset = (ptrSize == 8 ? 24 : 12);
  lldb::addr_t raw1 = valobj.GetPointerValue();
  lldb::addr_t address = (raw1 & 0x00FFFFFFFFFFFFFF) + bias;

  Status error;
  lldb::addr_t raw0 = process->ReadPointerFromMemory(raw1 + raw0_offset, error);
  if (error.Fail())
    return false;
  uint64_t count = raw0 & 0x0000FFFFFFFFFFFF;
  return readStringFromAddress(
      address, count, valobj, stream, options,
      StringPrinter::ReadStringAndDumpToStreamOptions());
}

bool lldb_private::formatters::swift::Bool_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  static ConstString g_value("_value");
  ValueObjectSP value_child(valobj.GetChildMemberWithName(g_value, true));
  if (!value_child)
    return false;

  // Swift Bools are stored in a byte, but only the LSB of the byte is
  // significant.  The swift::irgen::FixedTypeInfo structure represents
  // this information by providing a mask of the "extra bits" for the type.
  // But at present CompilerType has no way to represent that information.
  // So for now we hard code it.
  uint64_t value = value_child->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);
  const uint64_t mask = 1 << 0;
  value &= mask;

  switch (value) {
  case 0:
    stream.Printf("false");
    return true;
  case 1:
    stream.Printf("true");
    return true;
  case LLDB_INVALID_ADDRESS:
    return false;
  default:
    stream.Printf("<invalid> (0x%" PRIx8 ")", (uint8_t)value);
    return true;
  }
}

bool lldb_private::formatters::swift::DarwinBoolean_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  static ConstString g__value("_value");
  ValueObjectSP value_child(valobj.GetChildMemberWithName(g__value, true));
  if (!value_child)
    return false;
  auto value = value_child->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);
  switch (value) {
  case 0:
    stream.Printf("false");
    return true;
  default:
    stream.Printf("true");
    return true;
  }
}

static bool RangeFamily_SummaryProvider(ValueObject &valobj, Stream &stream,
                                        const TypeSummaryOptions &options,
                                        bool isHalfOpen) {
  LLDB_SCOPED_TIMER();
  static ConstString g_lowerBound("lowerBound");
  static ConstString g_upperBound("upperBound");

  ValueObjectSP lowerBound_sp(
      valobj.GetChildMemberWithName(g_lowerBound, true));
  ValueObjectSP upperBound_sp(
      valobj.GetChildMemberWithName(g_upperBound, true));

  if (!lowerBound_sp || !upperBound_sp)
    return false;

  lowerBound_sp = lowerBound_sp->GetQualifiedRepresentationIfAvailable(
      lldb::eDynamicDontRunTarget, true);
  upperBound_sp = upperBound_sp->GetQualifiedRepresentationIfAvailable(
      lldb::eDynamicDontRunTarget, true);

  auto start_summary = lowerBound_sp->GetValueAsCString();
  auto end_summary = upperBound_sp->GetValueAsCString();

  // the Range should not have a summary unless both start and end indices have
  // one - or it will look awkward
  if (!start_summary || !start_summary[0] || !end_summary || !end_summary[0])
    return false;

  stream.Printf("%s%s%s", start_summary, isHalfOpen ? "..<" : "...",
                end_summary);

  return true;
}

bool lldb_private::formatters::swift::Range_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return RangeFamily_SummaryProvider(valobj, stream, options, true);
}

bool lldb_private::formatters::swift::CountableRange_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return RangeFamily_SummaryProvider(valobj, stream, options, true);
}

bool lldb_private::formatters::swift::ClosedRange_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return RangeFamily_SummaryProvider(valobj, stream, options, false);
}

bool lldb_private::formatters::swift::CountableClosedRange_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  return RangeFamily_SummaryProvider(valobj, stream, options, false);
}

bool lldb_private::formatters::swift::BuiltinObjC_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  stream.Printf("0x%" PRIx64 " ", valobj.GetValueAsUnsigned(0));
  llvm::Expected<std::string> desc = valobj.GetObjectDescription();
  if (desc)
    stream << toString(desc.takeError());
  else
    stream << *desc;
  return true;
}

namespace lldb_private {
namespace formatters {
namespace swift {

class EnumSyntheticFrontEnd : public SyntheticChildrenFrontEnd {
public:
  EnumSyntheticFrontEnd(lldb::ValueObjectSP valobj_sp);

  llvm::Expected<uint32_t> CalculateNumChildren() override;
  lldb::ValueObjectSP GetChildAtIndex(uint32_t idx) override;
  lldb::ChildCacheState Update() override;
  bool MightHaveChildren() override;
  size_t GetIndexOfChildWithName(ConstString name) override;

private:
  ExecutionContextRef m_exe_ctx_ref;
  ConstString m_element_name;
  size_t m_child_index;
};

/// Synthetic provider for `Swift.Task`.
///
/// As seen by lldb, a `Task` instance is an opaque pointer, with neither type
/// metadata nor an AST to describe it. To implement this synthetic provider, a
/// `Task`'s state is retrieved from a `ReflectionContext`, and that data is
/// used to manually construct `ValueObject` children.
class TaskSyntheticFrontEnd : public SyntheticChildrenFrontEnd {
public:
  TaskSyntheticFrontEnd(lldb::ValueObjectSP valobj_sp)
      : SyntheticChildrenFrontEnd(*valobj_sp.get()) {
    auto target_sp = m_backend.GetTargetSP();
    auto ts_or_err =
        target_sp->GetScratchTypeSystemForLanguage(eLanguageTypeSwift);
    if (auto err = ts_or_err.takeError()) {
      LLDB_LOG(
          GetLog(LLDBLog::DataFormatters | LLDBLog::Types),
          "could not get Swift type system for Task synthetic provider: {0}",
          fmt_consume(std::move(err)));
      return;
    }
    m_ts = llvm::dyn_cast_or_null<TypeSystemSwiftTypeRef>(ts_or_err->get());
  }

  constexpr static StringLiteral TaskChildren[] = {
      "id",
      "kind",
      "enqueuPriority",
      "isChildTask",
      "isFuture",
      "isGroupChildTask",
      "isAsyncLetTask",
      "isCancelled",
      "isStatusRecordLocked",
      "isEscalated",
      "isEnqueued",
      "isRunning",
  };

  llvm::Expected<uint32_t> CalculateNumChildren() override {
    auto count = ArrayRef(TaskChildren).size();
    return m_task_info.hasIsRunning ? count : count - 1;
  }

  lldb::ValueObjectSP GetChildAtIndex(uint32_t idx) override {
    auto target_sp = m_backend.GetTargetSP();

    // TypeMangling for "Swift.Bool"
    CompilerType bool_type =
        m_ts->GetTypeFromMangledTypename(ConstString("$sSbD"));
    // TypeMangling for "Swift.UInt32"
    CompilerType uint32_type =
        m_ts->GetTypeFromMangledTypename(ConstString("$ss6UInt32VD"));
    // TypeMangling for "Swift.UInt64"
    CompilerType uint64_type =
        m_ts->GetTypeFromMangledTypename(ConstString("$ss6UInt64VD"));

#define RETURN_CHILD(FIELD, NAME, TYPE)                                        \
  if (!FIELD) {                                                                \
    auto value = m_task_info.NAME;                                             \
    DataExtractor data{reinterpret_cast<const void *>(&value), sizeof(value),  \
                       endian::InlHostByteOrder(), sizeof(void *)};            \
    FIELD = ValueObject::CreateValueObjectFromData(                            \
        #NAME, data, m_backend.GetExecutionContextRef(), TYPE);                \
  }                                                                            \
  return FIELD;

    switch (idx) {
    case 0:
      RETURN_CHILD(m_id_sp, id, uint64_type);
    case 1:
      RETURN_CHILD(m_kind_sp, kind, uint32_type);
    case 2:
      RETURN_CHILD(m_enqueue_priority_sp, enqueuePriority, uint32_type);
    case 3:
      RETURN_CHILD(m_is_child_task_sp, isChildTask, bool_type);
    case 4:
      RETURN_CHILD(m_is_future_sp, isFuture, bool_type);
    case 5:
      RETURN_CHILD(m_is_group_child_task_sp, isGroupChildTask, bool_type);
    case 6:
      RETURN_CHILD(m_is_async_let_task_sp, isAsyncLetTask, bool_type);
    case 7:
      RETURN_CHILD(m_is_cancelled_sp, isCancelled, bool_type);
    case 8:
      RETURN_CHILD(m_is_status_record_locked_sp, isStatusRecordLocked,
                   bool_type);
    case 9:
      RETURN_CHILD(m_is_escalated_sp, isEscalated, bool_type);
    case 10:
      RETURN_CHILD(m_is_enqueued_sp, isEnqueued, bool_type);
    case 11:
      RETURN_CHILD(m_is_running_sp, isRunning, bool_type);
    default:
      return {};
    }

#undef RETURN_CHILD
  }

  lldb::ChildCacheState Update() override {
    if (auto *runtime = SwiftLanguageRuntime::Get(m_backend.GetProcessSP())) {
      ThreadSafeReflectionContext reflection_ctx =
          runtime->GetReflectionContext();
      ValueObjectSP task_obj_sp = m_backend.GetChildMemberWithName("_task");
      if (!task_obj_sp)
        return ChildCacheState::eRefetch;
      uint64_t task_ptr = task_obj_sp->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);
      if (task_ptr != LLDB_INVALID_ADDRESS) {
        llvm::Expected<ReflectionContextInterface::AsyncTaskInfo> task_info =
            reflection_ctx->asyncTaskInfo(task_ptr);
        if (auto err = task_info.takeError()) {
          LLDB_LOG(GetLog(LLDBLog::DataFormatters | LLDBLog::Types),
                   "could not get info for async task {0:x}: {1}", task_ptr,
                   fmt_consume(std::move(err)));
        } else {
          m_task_info = *task_info;
          for (auto child :
               {m_id_sp, m_kind_sp, m_enqueue_priority_sp, m_is_child_task_sp,
                m_is_future_sp, m_is_group_child_task_sp,
                m_is_async_let_task_sp, m_is_cancelled_sp,
                m_is_status_record_locked_sp, m_is_escalated_sp,
                m_is_enqueued_sp, m_is_running_sp})
            child.reset();
        }
      }
    }
    return ChildCacheState::eRefetch;
  }

  bool MightHaveChildren() override { return true; }

  size_t GetIndexOfChildWithName(ConstString name) override {
    ArrayRef children = TaskChildren;
    const auto *it = llvm::find(children, name);
    if (it == children.end())
      return UINT32_MAX;
    return std::distance(children.begin(), it);
  }

private:
  TypeSystemSwiftTypeRef *m_ts = nullptr;
  ReflectionContextInterface::AsyncTaskInfo m_task_info;
  ValueObjectSP m_id_sp;
  ValueObjectSP m_kind_sp;
  ValueObjectSP m_enqueue_priority_sp;
  ValueObjectSP m_is_child_task_sp;
  ValueObjectSP m_is_future_sp;
  ValueObjectSP m_is_group_child_task_sp;
  ValueObjectSP m_is_async_let_task_sp;
  ValueObjectSP m_is_cancelled_sp;
  ValueObjectSP m_is_status_record_locked_sp;
  ValueObjectSP m_is_escalated_sp;
  ValueObjectSP m_is_enqueued_sp;
  ValueObjectSP m_is_running_sp;
};
}
}
}

lldb_private::formatters::swift::EnumSyntheticFrontEnd::EnumSyntheticFrontEnd(
    lldb::ValueObjectSP valobj_sp)
    : SyntheticChildrenFrontEnd(*valobj_sp.get()), m_exe_ctx_ref(),
      m_element_name(nullptr), m_child_index(UINT32_MAX) {
  if (valobj_sp)
    Update();
}

llvm::Expected<uint32_t>
lldb_private::formatters::swift::EnumSyntheticFrontEnd::CalculateNumChildren() {
  return m_child_index != UINT32_MAX ? 1 : 0;
}

lldb::ValueObjectSP
lldb_private::formatters::swift::EnumSyntheticFrontEnd::GetChildAtIndex(
    uint32_t idx) {
  if (idx)
    return ValueObjectSP();
  if (m_child_index == UINT32_MAX)
    return ValueObjectSP();
  return m_backend.GetChildAtIndex(m_child_index, true);
}

lldb::ChildCacheState
lldb_private::formatters::swift::EnumSyntheticFrontEnd::Update() {
  m_element_name.Clear();
  m_child_index = UINT32_MAX;
  m_exe_ctx_ref = m_backend.GetExecutionContextRef();
  m_element_name.SetCString(m_backend.GetValueAsCString());
  m_child_index = m_backend.GetIndexOfChildWithName(m_element_name);
  return ChildCacheState::eRefetch;
}

bool lldb_private::formatters::swift::EnumSyntheticFrontEnd::
    MightHaveChildren() {
  return m_child_index != UINT32_MAX;
}

size_t
lldb_private::formatters::swift::EnumSyntheticFrontEnd::GetIndexOfChildWithName(
    ConstString name) {
  if (name == m_element_name)
    return 0;
  return UINT32_MAX;
}

SyntheticChildrenFrontEnd *
lldb_private::formatters::swift::EnumSyntheticFrontEndCreator(
    CXXSyntheticChildren *, lldb::ValueObjectSP valobj_sp) {
  if (!valobj_sp)
    return NULL;
  return (new EnumSyntheticFrontEnd(valobj_sp));
}

SyntheticChildrenFrontEnd *
lldb_private::formatters::swift::TaskSyntheticFrontEndCreator(
    CXXSyntheticChildren *, lldb::ValueObjectSP valobj_sp) {
  if (!valobj_sp)
    return NULL;
  return new TaskSyntheticFrontEnd(valobj_sp);
}

bool lldb_private::formatters::swift::ObjC_Selector_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  LLDB_SCOPED_TIMER();
  static ConstString g_ptr("ptr");
  static ConstString g__rawValue("_rawValue");

  ValueObjectSP ptr_sp(valobj.GetChildAtNamePath({g_ptr, g__rawValue}));
  if (!ptr_sp)
    return false;

  auto ptr_value = ptr_sp->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);

  if (0 == ptr_value || LLDB_INVALID_ADDRESS == ptr_value)
    return false;

  StringPrinter::ReadStringAndDumpToStreamOptions read_options;
  read_options.SetLocation(ptr_value);
  read_options.SetTargetSP(valobj.GetTargetSP());
  read_options.SetStream(&stream);
  read_options.SetQuote('"');
  read_options.SetNeedsZeroTermination(true);
  read_options.SetEscapeStyle(StringPrinter::EscapeStyle::Swift);

  return StringPrinter::ReadStringAndDumpToStream<
      StringPrinter::StringElementType::ASCII>(read_options);
}

template <int Key> struct TypePreservingNSNumber;

template <> struct TypePreservingNSNumber<0> {
  typedef int64_t SixtyFourValueType;
  typedef int32_t ThirtyTwoValueType;

  static constexpr const char *FormatString = "Int(%" PRId64 ")";
};

template <> struct TypePreservingNSNumber<1> {
  typedef int64_t ValueType;
  static constexpr const char *FormatString = "Int64(%" PRId64 ")";
};

template <> struct TypePreservingNSNumber<2> {
  typedef int32_t ValueType;
  static constexpr const char *FormatString = "Int32(%" PRId32 ")";
};

template <> struct TypePreservingNSNumber<3> {
  typedef int16_t ValueType;
  static constexpr const char *FormatString = "Int16(%" PRId16 ")";
};

template <> struct TypePreservingNSNumber<4> {
  typedef int8_t ValueType;
  static constexpr const char *FormatString = "Int8(%" PRId8 ")";
};

template <> struct TypePreservingNSNumber<5> {
  typedef uint64_t SixtyFourValueType;
  typedef uint32_t ThirtyTwoValueType;

  static constexpr const char *FormatString = "UInt(%" PRIu64 ")";
};

template <> struct TypePreservingNSNumber<6> {
  typedef uint64_t ValueType;
  static constexpr const char *FormatString = "UInt64(%" PRIu64 ")";
};

template <> struct TypePreservingNSNumber<7> {
  typedef uint32_t ValueType;
  static constexpr const char *FormatString = "UInt32(%" PRIu32 ")";
};

template <> struct TypePreservingNSNumber<8> {
  typedef uint16_t ValueType;
  static constexpr const char *FormatString = "UInt16(%" PRIu16 ")";
};

template <> struct TypePreservingNSNumber<9> {
  typedef uint8_t ValueType;
  static constexpr const char *FormatString = "UInt8(%" PRIu8 ")";
};

template <> struct TypePreservingNSNumber<10> {
  typedef float ValueType;
  static constexpr const char *FormatString = "Float(%f)";
};

template <> struct TypePreservingNSNumber<11> {
  typedef double ValueType;
  static constexpr const char *FormatString = "Double(%f)";
};

template <> struct TypePreservingNSNumber<12> {
  typedef double SixtyFourValueType;
  typedef float ThirtyTwoValueType;

  static constexpr const char *FormatString = "CGFloat(%f)";
};

template <> struct TypePreservingNSNumber<13> {
  typedef bool ValueType;
  static constexpr const char *FormatString = "Bool(%d)";
};

template <int Key,
          typename Value = typename TypePreservingNSNumber<Key>::ValueType>
bool PrintTypePreservingNSNumber(DataBufferSP buffer_sp, Stream &stream) {
  Value value;
  memcpy(&value, buffer_sp->GetBytes(), sizeof(value));
  stream.Printf(TypePreservingNSNumber<Key>::FormatString, value);
  return true;
}

template <>
bool PrintTypePreservingNSNumber<13, void>(DataBufferSP buffer_sp,
                                           Stream &stream) {
  typename TypePreservingNSNumber<13>::ValueType value;
  memcpy(&value, buffer_sp->GetBytes(), sizeof(value));
  stream.PutCString(value ? "true" : "false");
  return true;
}

template <int Key, typename SixtyFour =
                       typename TypePreservingNSNumber<Key>::SixtyFourValueType,
          typename ThirtyTwo =
              typename TypePreservingNSNumber<Key>::ThirtyTwoValueType>
bool PrintTypePreservingNSNumber(DataBufferSP buffer_sp, ProcessSP process_sp,
                                 Stream &stream) {
  switch (process_sp->GetAddressByteSize()) {
  case 4: {
    ThirtyTwo value;
    memcpy(&value, buffer_sp->GetBytes(), sizeof(value));
    stream.Printf(TypePreservingNSNumber<Key>::FormatString, (SixtyFour)value);
    return true;
  }
  case 8: {
    SixtyFour value;
    memcpy(&value, buffer_sp->GetBytes(), sizeof(value));
    stream.Printf(TypePreservingNSNumber<Key>::FormatString, value);
    return true;
  }
  }

  llvm_unreachable("unknown address byte size");
}

bool lldb_private::formatters::swift::TypePreservingNSNumber_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  LLDB_SCOPED_TIMER();
  lldb::addr_t ptr_value(valobj.GetValueAsUnsigned(LLDB_INVALID_ADDRESS));
  if (ptr_value == LLDB_INVALID_ADDRESS)
    return false;

  ProcessSP process_sp(valobj.GetProcessSP());
  if (!process_sp)
    return false;

  uint32_t ptr_size = process_sp->GetAddressByteSize();
  const uint32_t size_of_tag = 1;
  const uint32_t size_of_payload = 8;

  lldb::addr_t addr_of_payload = ptr_value + ptr_size;
  lldb::addr_t addr_of_tag = addr_of_payload + size_of_payload;

  Status read_error;
  uint64_t tag = process_sp->ReadUnsignedIntegerFromMemory(
      addr_of_tag, size_of_tag, 0, read_error);
  if (read_error.Fail())
    return false;

  WritableDataBufferSP buffer_sp(new DataBufferHeap(size_of_payload, 0));
  process_sp->ReadMemoryFromInferior(addr_of_payload, buffer_sp->GetBytes(),
                                     size_of_payload, read_error);
  if (read_error.Fail())
    return false;

#define PROCESS_DEPENDENT_TAG(Key)                                             \
  case Key:                                                                    \
    return PrintTypePreservingNSNumber<Key>(buffer_sp, process_sp, stream);
#define PROCESS_INDEPENDENT_TAG(Key)                                           \
  case Key:                                                                    \
    return PrintTypePreservingNSNumber<Key>(buffer_sp, stream);

  switch (tag) {
    PROCESS_DEPENDENT_TAG(0);
    PROCESS_INDEPENDENT_TAG(1);
    PROCESS_INDEPENDENT_TAG(2);
    PROCESS_INDEPENDENT_TAG(3);
    PROCESS_INDEPENDENT_TAG(4);
    PROCESS_DEPENDENT_TAG(5);
    PROCESS_INDEPENDENT_TAG(6);
    PROCESS_INDEPENDENT_TAG(7);
    PROCESS_INDEPENDENT_TAG(8);
    PROCESS_INDEPENDENT_TAG(9);
    PROCESS_INDEPENDENT_TAG(10);
    PROCESS_INDEPENDENT_TAG(11);
    PROCESS_DEPENDENT_TAG(12);
    PROCESS_INDEPENDENT_TAG(13);
  default:
    break;
  }

#undef PROCESS_DEPENDENT_TAG
#undef PROCESS_INDEPENDENT_TAG

  return false;
}

namespace {

/// Enumerate the kinds of SIMD elements.
enum class SIMDElementKind {
  Int32,
  UInt32,
  Float32,
  Float64
};

/// A helper for formatting a kind of SIMD element.
class SIMDElementFormatter {
  SIMDElementKind m_kind;

public:
  SIMDElementFormatter(SIMDElementKind kind) : m_kind(kind) {}

  /// Create a string representation of a SIMD element given a pointer to it.
  std::string Format(const uint8_t *data) const {
    std::string S;
    llvm::raw_string_ostream OS(S);
    switch (m_kind) {
    case SIMDElementKind::Int32: {
      auto *p = reinterpret_cast<const int32_t *>(data);
      OS << *p;
      break;
    }
    case SIMDElementKind::UInt32: {
      auto *p = reinterpret_cast<const uint32_t *>(data);
      OS << *p;
      break;
    }
    case SIMDElementKind::Float32: {
      auto *p = reinterpret_cast<const float *>(data);
      OS << *p;
      break;
    }
    case SIMDElementKind::Float64: {
      auto *p = reinterpret_cast<const double *>(data);
      OS << *p;
      break;
    }
    }
    return S;
  }

  /// Get the size in bytes of this kind of SIMD element.
  unsigned getElementSize() const {
    return (m_kind == SIMDElementKind::Float64) ? 8 : 4;
  }
};

/// Read a vector from a buffer target.
std::optional<std::vector<std::string>>
ReadVector(const SIMDElementFormatter &formatter, const uint8_t *buffer,
           unsigned len, unsigned offset, unsigned num_elements) {
  unsigned elt_size = formatter.getElementSize();
  if ((offset + num_elements * elt_size) > len)
    return std::nullopt;
  std::vector<std::string> elements;
  for (unsigned I = 0; I < num_elements; ++I)
    elements.emplace_back(formatter.Format(buffer + offset + (I * elt_size)));
  return elements;
}

/// Read a SIMD vector from the target.
std::optional<std::vector<std::string>>
ReadVector(Process &process, ValueObject &valobj,
           const SIMDElementFormatter &formatter, unsigned num_elements) {
  Status error;
  static ConstString g_storage("_storage");
  static ConstString g_value("_value");
  ValueObjectSP value_sp = valobj.GetChildAtNamePath({g_storage, g_value});
  if (!value_sp)
    return std::nullopt;

  // The layout of the vector is the same as what you'd expect for a C-style
  // array. It's a contiguous bag of bytes with no padding.
  lldb_private::DataExtractor data;
  uint64_t len = value_sp->GetData(data, error);
  if (error.Fail())
    return std::nullopt;

  const uint8_t *buffer = data.GetDataStart();
  return ReadVector(formatter, buffer, len, 0, num_elements);
}

/// Print a vector of elements as a row, if possible.
bool PrintRow(Stream &stream, std::optional<std::vector<std::string>> vec) {
  if (!vec)
    return false;

  std::string joined = llvm::join(*vec, ", ");
  stream.Printf("(%s)", joined.c_str());
  return true;
}

void PrintMatrix(Stream &stream,
                 const std::vector<std::vector<std::string>> &matrix,
                 int num_columns, int num_rows) {
  // Print each row.
  stream.Printf("\n[ ");
  for (int J = 0; J < num_rows; ++J) {
    // Join the J-th row's elements with commas.
    std::vector<std::string> row;
    for (int I = 0; I < num_columns; ++I)
      row.emplace_back(std::move(matrix[I][J]));
    std::string joined = llvm::join(row, ", ");

    // Add spacing and punctuation to 1) make it possible to copy the matrix
    // into a Python repl and 2) to avoid writing '[[' in FileCheck tests.
    if (J > 0)
      stream.Printf("  ");
    stream.Printf("[%s]", joined.c_str());
    if (J != (num_rows - 1))
      stream.Printf(",\n");
    else
      stream.Printf(" ]\n");
  }
}

} // end anonymous namespace

bool lldb_private::formatters::swift::SIMDVector_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  LLDB_SCOPED_TIMER();

  // SIMD vector contains an inner member `_storage` which is an opaque
  // container. Given SIMD is always in the form SIMDX<Type> where X is a
  // positive integer, we can calculate the number of elements and the
  // dynamic archetype (and hence its size). Everything follows naturally
  // as the elements are laid out in a contigous buffer without padding.
  CompilerType simd_type = valobj.GetCompilerType().GetCanonicalType();
  auto ts = simd_type.GetTypeSystem().dyn_cast_or_null<TypeSystemSwift>();
  if (!ts)
    return false;

  ExecutionContext exe_ctx = valobj.GetExecutionContextRef().Lock(true);
  std::optional<uint64_t> opt_type_size =
    simd_type.GetByteSize(exe_ctx.GetBestExecutionContextScope());
  if (!opt_type_size)
    return false;
  uint64_t type_size = *opt_type_size;

  lldbassert(simd_type.GetNumTemplateArguments() == 1 && "broken SIMD type");
  if (simd_type.GetNumTemplateArguments() != 1)
    return false;

  auto arg_type = ts->GetGenericArgumentType(simd_type.GetOpaqueQualType(), 0);
  lldbassert(arg_type && "Unexpected invalid SIMD generic argument type");
  if (!arg_type)
    return false;

  std::optional<uint64_t> opt_arg_size =
      arg_type.GetByteSize(exe_ctx.GetBestExecutionContextScope());
  if (!opt_arg_size)
    return false;
  uint64_t arg_size = *opt_arg_size;

  DataExtractor storage_buf;
  Status error;
  uint64_t len = valobj.GetData(storage_buf, error);
  lldbassert(len == type_size && "extracted less bytes than requested");
  if (len < type_size)
    return false;

  // We deduce the number of elements looking at the size of the swift
  // type and the size of the generic argument, as we know the type is
  // laid out contiguosly in memory. SIMD3, though, has an element of
  // padding. Given this is the only type in the standard library with
  // padding, we special-case it.
  ConstString full_type_name = simd_type.GetTypeName();
  llvm::StringRef type_name = full_type_name.GetStringRef();
  uint64_t num_elements = type_size / arg_size;
  auto generic_pos = type_name.find("<");
  if (generic_pos != llvm::StringRef::npos)
    type_name = type_name.slice(0, generic_pos);
  if (type_name == "Swift.SIMD3")
    num_elements = 3;

  std::vector<std::string> elem_vector;
  for (uint64_t i = 0; i < num_elements; ++i) {
    DataExtractor elem_extractor(storage_buf, i * arg_size, arg_size);
    auto simd_elem = ValueObject::CreateValueObjectFromData(
        "simd_elem", elem_extractor, valobj.GetExecutionContextRef(), arg_type);
    if (!simd_elem || simd_elem->GetError().Fail())
      return false;

    auto synthetic = simd_elem->GetSyntheticValue();
    if (!synthetic)
      return false;
    const char *value_string = synthetic->GetValueAsCString();
    elem_vector.push_back(value_string);
  }

  return PrintRow(stream, elem_vector);
}

bool lldb_private::formatters::swift::LegacySIMD_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  LLDB_SCOPED_TIMER();

  Status error;
  ProcessSP process_sp(valobj.GetProcessSP());
  if (!process_sp)
    return false;
  Process &process = *process_sp.get();

  // Get the type name without the "simd.simd_" prefix.
  ConstString full_type_name = valobj.GetTypeName();

  llvm::StringRef type_name = full_type_name.GetStringRef();
  if (type_name.starts_with("simd."))
    type_name = type_name.drop_front(5);
  if (type_name.starts_with("simd_"))
    type_name = type_name.drop_front(5);

  // Get the type of object this is.
  bool is_quaternion = type_name.starts_with("quat");
  bool is_matrix = type_name[type_name.size() - 2] == 'x';
  bool is_vector = !is_matrix && !is_quaternion;

  // Get the kind of SIMD element inside of this object.
  std::optional<SIMDElementKind> kind = std::nullopt;
  if (type_name.starts_with("int"))
    kind = SIMDElementKind::Int32;
  else if (type_name.starts_with("uint"))
    kind = SIMDElementKind::UInt32;
  else if ((is_quaternion && type_name.ends_with("f")) ||
           type_name.starts_with("float"))
    kind = SIMDElementKind::Float32;
  else if ((is_quaternion && type_name.ends_with("d")) ||
           type_name.starts_with("double"))
    kind = SIMDElementKind::Float64;
  if (!kind)
    return false;

  SIMDElementFormatter formatter(*kind);

  if (is_vector) {
    unsigned num_elements = llvm::hexDigitValue(type_name.back());
    return PrintRow(stream,
                    ReadVector(process, valobj, formatter, num_elements));
  } else if (is_quaternion) {
    static ConstString g_vector("vector");
    ValueObjectSP vec_sp = valobj.GetChildAtNamePath({g_vector});
    if (!vec_sp)
      return false;

    return PrintRow(stream, ReadVector(process, *vec_sp.get(), formatter, 4));
  } else if (is_matrix) {
    static ConstString g_columns("columns");
    ValueObjectSP columns_sp = valobj.GetChildAtNamePath({g_columns});
    if (!columns_sp)
      return false;

    unsigned num_columns = llvm::hexDigitValue(type_name[type_name.size() - 3]);
    unsigned num_rows = llvm::hexDigitValue(type_name[type_name.size() - 1]);

    // SIMD matrices are stored column-major. Collect each column vector as a
    // precursor for row-by-row pretty-printing.
    std::vector<std::vector<std::string>> columns;
    for (unsigned I = 0; I < num_columns; ++I) {
      std::string col_num_str = llvm::utostr(I);
      ConstString col_num_const_str(col_num_str.c_str());
      ValueObjectSP column_sp =
          columns_sp->GetChildAtNamePath({col_num_const_str});
      if (!column_sp)
        return false;

      auto vec = ReadVector(process, *column_sp.get(), formatter, num_rows);
      if (!vec)
        return false;

      columns.emplace_back(std::move(*vec));
    }

    PrintMatrix(stream, columns, num_columns, num_rows);
    return true;
  }

  return false;
}

bool lldb_private::formatters::swift::GLKit_SummaryProvider(
    ValueObject &valobj, Stream &stream, const TypeSummaryOptions &options) {
  LLDB_SCOPED_TIMER();

  // Get the type name without the "GLKit." prefix.
  ConstString full_type_name = valobj.GetTypeName();
  llvm::StringRef type_name = full_type_name.GetStringRef();
  if (type_name.starts_with("GLKit."))
    type_name = type_name.drop_front(6);

  // Get the type of object this is.
  bool is_quaternion = type_name == "GLKQuaternion";
  bool is_matrix = type_name.starts_with("GLKMatrix");
  bool is_vector = type_name.starts_with("GLKVector");

  if (!(is_quaternion || is_matrix || is_vector))
    return false;

  SIMDElementFormatter formatter(SIMDElementKind::Float32);

  unsigned num_elements =
      is_quaternion ? 4 : llvm::hexDigitValue(type_name.back());
  DataExtractor data;
  Status error;
  uint64_t len = valobj.GetData(data, error);
  const uint8_t *buffer = data.GetDataStart();
  if (!is_matrix) {
    return PrintRow(stream,
                    ReadVector(formatter, buffer, len, 0, num_elements));
  }

  // GLKit matrices are stored column-major. Collect each column vector as a
  // precursor for row-by-row pretty-printing.
  std::vector<std::vector<std::string>> columns;
  for (unsigned I = 0; I < num_elements; ++I) {
    auto vec =
        ReadVector(formatter, buffer, len, I * 4 * num_elements, num_elements);
    if (!vec)
      return false;

    columns.emplace_back(std::move(*vec));
  }

  PrintMatrix(stream, columns, num_elements, num_elements);
  return true;
}
