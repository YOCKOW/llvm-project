//===-- SwiftOptional.cpp ---------------------------------------*- C++ -*-===//
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

#include "SwiftOptional.h"
#include "Plugins/LanguageRuntime/Swift/SwiftLanguageRuntime.h"
#include "lldb/DataFormatters/DataVisualization.h"
#include "lldb/DataFormatters/TypeSummary.h"
#include "lldb/DataFormatters/ValueObjectPrinter.h"
#include "lldb/Target/Process.h"
#include "lldb/Utility/DataBufferHeap.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/lldb-enumerations.h"

using namespace lldb;
using namespace lldb_private;
using namespace lldb_private::formatters;
using namespace lldb_private::formatters::swift;

std::string lldb_private::formatters::swift::SwiftOptionalSummaryProvider::
    GetDescription() {
  StreamString sstr;
  sstr.Printf("`%s `%s%s%s%s%s%s%s", "Swift.Optional summary provider",
              Cascades() ? "" : " (not cascading)", " (may show children)",
              !DoesPrintValue(nullptr) ? " (hide value)" : "",
              IsOneLiner() ? " (one-line printout)" : "",
              SkipsPointers() ? " (skip pointers)" : "",
              SkipsReferences() ? " (skip references)" : "",
              HideNames(nullptr) ? " (hide member names)" : "");
  return sstr.GetString().str();
}

/// If this ValueObject is an Optional<T> with the Some(T) case selected,
/// retrieve the value of the Some case.
///
/// Returns {} on error, nullptr on .none, and a ValueObject on .some.
/// None of the callees can pass on errors messages, so this function
/// doesn't return them either.
static std::optional<ValueObjectSP>
ExtractSomeIfAny(ValueObject *optional,
                 bool synthetic_value = false) {
  if (!optional)
    return {};

  static ConstString g_Some("some");
  static ConstString g_None("none");

  ValueObjectSP non_synth_valobj = optional->GetNonSyntheticValue();
  if (!non_synth_valobj)
    return {};

  ConstString value(non_synth_valobj->GetValueAsCString());

  if (!value)
    return {};

  if (value == g_None)
    return nullptr;

  ValueObjectSP value_sp(
      non_synth_valobj->GetChildMemberWithName(g_Some, true));
  if (!value_sp)
    return {};

  auto process_sp = optional->GetProcessSP();
  auto *swift_runtime = SwiftLanguageRuntime::Get(process_sp);

  CompilerType type = non_synth_valobj->GetCompilerType();
  auto type_system = type.GetTypeSystem().dyn_cast_or_null<TypeSystemSwift>();
  if (!type_system)
    return {};
  if (auto kind = type_system->GetNonTriviallyManagedReferenceKind(
          type.GetOpaqueQualType())) {
    if (*kind == TypeSystemSwift::NonTriviallyManagedReferenceKind::eWeak) {
      if (swift_runtime) {
        lldb::addr_t original_ptr =
            value_sp->GetValueAsUnsigned(LLDB_INVALID_ADDRESS);
        lldb::addr_t tweaked_ptr =
            swift_runtime->MaybeMaskNonTrivialReferencePointer(original_ptr,
                                                               *kind);
        if (original_ptr != tweaked_ptr) {
          CompilerType value_type(value_sp->GetCompilerType());
          DataBufferSP buffer_sp(
              new DataBufferHeap(&tweaked_ptr, sizeof(tweaked_ptr)));
          DataExtractor extractor(buffer_sp, process_sp->GetByteOrder(),
                                  process_sp->GetAddressByteSize());
          ExecutionContext exe_ctx(process_sp);
          value_sp = ValueObject::CreateValueObjectFromData(
              value_sp->GetName().AsCString(), extractor, exe_ctx, value_type);
          if (!value_sp)
            return {};
          else
            value_sp->SetSyntheticChildrenGenerated(true);
        }
      }
    }
  }
  lldb::DynamicValueType use_dynamic;

  // FIXME: We usually want to display the dynamic value of an optional's
  // payload, but we don't have a simple way to determine whether the dynamic
  // value was actually requested. Consult the target setting as a workaround.
  if (swift_runtime->CouldHaveDynamicValue(*value_sp))
    // FIXME (cont): Here, we'd like to use some new API to determine whether
    // a dynamic value was actually requested.
    use_dynamic = eDynamicDontRunTarget;
  else
    use_dynamic = value_sp->GetTargetSP()->GetPreferDynamicValue();

  ValueObjectSP dyn_value_sp = value_sp->GetDynamicValue(use_dynamic);
  if (dyn_value_sp)
    value_sp = dyn_value_sp;

  if (synthetic_value && value_sp->HasSyntheticValue())
    value_sp = value_sp->GetSyntheticValue();

  return value_sp;
}

static bool
SwiftOptional_SummaryProvider_Impl(ValueObject &valobj, Stream &stream,
                                   const TypeSummaryOptions &options) {
  std::optional<ValueObjectSP> maybe_some = ExtractSomeIfAny(&valobj, true);
  if (!maybe_some)
    return false;

  ValueObjectSP some = *maybe_some;
  if (!some) {
    stream.Printf("nil");
    return true;
  }

  const char *summary = some->GetSummaryAsCString();
  const char *value = some->GetValueAsCString();

  if (summary)
    stream.Printf("%s", summary);
  else if (value)
    stream.Printf("%s", value);
  else if (lldb_private::DataVisualization::ShouldPrintAsOneLiner(*some)) {
    TypeSummaryImpl::Flags oneliner_flags;
    oneliner_flags.SetHideItemNames(false)
        .SetCascades(true)
        .SetDontShowChildren(false)
        .SetDontShowValue(false)
        .SetShowMembersOneLiner(true)
        .SetSkipPointers(false)
        .SetSkipReferences(false);
    StringSummaryFormat oneliner(oneliner_flags, "");
    std::string buffer;
    oneliner.FormatObject(some.get(), buffer, options);
    stream.Printf("%s", buffer.c_str());
  }

  return true;
}

bool lldb_private::formatters::swift::SwiftOptionalSummaryProvider::
    FormatObject(ValueObject *target_valobj_sp, std::string &dest,
                 const TypeSummaryOptions &options) {
  if (!target_valobj_sp)
    return false;

  StreamString stream;

  bool is_ok =
      SwiftOptional_SummaryProvider_Impl(*target_valobj_sp, stream, options);
  dest.assign(stream.GetString().str());

  return is_ok;
}

bool lldb_private::formatters::swift::SwiftOptionalSummaryProvider::
    DoesPrintChildren(ValueObject *target_valobj) const {
  if (!target_valobj)
    return false;

  std::optional<ValueObjectSP> maybe_some =
      ExtractSomeIfAny(target_valobj, true);
  if (!maybe_some)
    return false;

  ValueObjectSP some = *maybe_some;
  if (!some)
    return true;

  lldb_private::Flags some_flags(some->GetCompilerType().GetTypeInfo());

  if (some_flags.AllSet(eTypeIsSwift)) {
    if (some_flags.AnySet(eTypeInstanceIsPointer | eTypeIsProtocol))
      return true;
  }

  lldb::TypeSummaryImplSP summary_sp = some->GetSummaryFormat();

  if (!summary_sp) {
    if (lldb_private::DataVisualization::ShouldPrintAsOneLiner(*some))
      return false;
    return some->HasChildren();
  }
  return some->HasChildren() && summary_sp->DoesPrintChildren(some.get());
}

bool lldb_private::formatters::swift::SwiftOptionalSummaryProvider::
    DoesPrintValue(ValueObject *valobj) const {
  return false;
}

lldb_private::formatters::swift::SwiftOptionalSyntheticFrontEnd::
    SwiftOptionalSyntheticFrontEnd(lldb::ValueObjectSP valobj_sp)
    : SyntheticChildrenFrontEnd(*valobj_sp.get()), m_is_none(false),
      m_children(false), m_some(nullptr) {}

bool lldb_private::formatters::swift::SwiftOptionalSyntheticFrontEnd::IsEmpty()
    const {
  return (m_is_none == true || m_children == false || m_some == nullptr);
}

llvm::Expected<uint32_t> lldb_private::formatters::swift::
    SwiftOptionalSyntheticFrontEnd::CalculateNumChildren() {
  if (IsEmpty())
    return 0;
  return m_some->GetNumChildren();
}

lldb::ValueObjectSP lldb_private::formatters::swift::
    SwiftOptionalSyntheticFrontEnd::GetChildAtIndex(uint32_t idx) {
  if (IsEmpty())
    return nullptr;
  auto child = m_some->GetChildAtIndex(idx, true);
  if (child && m_some->IsSyntheticChildrenGenerated())
    child->SetSyntheticChildrenGenerated(true);
  return child;
}

lldb::ChildCacheState lldb_private::formatters::swift::SwiftOptionalSyntheticFrontEnd::Update() {
  m_some = nullptr;
  m_is_none = true;
  m_children = false;

  std::optional<ValueObjectSP> maybe_some =
      ExtractSomeIfAny(&m_backend, true);
  if (!maybe_some)
    return ChildCacheState::eRefetch;

  m_some = *maybe_some;

  if (!m_some) {
    m_is_none = true;
    m_children = false;
    return ChildCacheState::eRefetch;
  }

  m_is_none = false;

  m_children = m_some->HasChildren();

  return ChildCacheState::eRefetch;
}

bool lldb_private::formatters::swift::SwiftOptionalSyntheticFrontEnd::
    MightHaveChildren() {
  return IsEmpty() ? false : true;
}

size_t lldb_private::formatters::swift::SwiftOptionalSyntheticFrontEnd::
    GetIndexOfChildWithName(ConstString name) {
  static ConstString g_Some("some");

  if (IsEmpty())
    return UINT32_MAX;

  return m_some->GetIndexOfChildWithName(name);
}

lldb::ValueObjectSP lldb_private::formatters::swift::
    SwiftOptionalSyntheticFrontEnd::GetSyntheticValue() {
  if (m_some && m_some->CanProvideValue())
    return m_some->GetSP();
  return nullptr;
}

SyntheticChildrenFrontEnd *
lldb_private::formatters::swift::SwiftOptionalSyntheticFrontEndCreator(
    CXXSyntheticChildren *, lldb::ValueObjectSP valobj_sp) {
  if (!valobj_sp)
    return nullptr;
  return (new SwiftOptionalSyntheticFrontEnd(valobj_sp));
}

SyntheticChildrenFrontEnd *
lldb_private::formatters::swift::SwiftUncheckedOptionalSyntheticFrontEndCreator(
    CXXSyntheticChildren *cxx_synth, lldb::ValueObjectSP valobj_sp) {
  if (!valobj_sp)
    return nullptr;
  return SwiftOptionalSyntheticFrontEndCreator(cxx_synth, valobj_sp);
}
