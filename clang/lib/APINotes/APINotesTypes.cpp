//===-- APINotesTypes.cpp - API Notes Data Types ----------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/APINotes/Types.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace api_notes {
LLVM_DUMP_METHOD void CommonEntityInfo::dump(llvm::raw_ostream &OS) const {
  if (Unavailable)
    OS << "[Unavailable] (" << UnavailableMsg << ")" << ' ';
  if (UnavailableInSwift)
    OS << "[UnavailableInSwift] ";
  if (SwiftPrivateSpecified)
    OS << (SwiftPrivate ? "[SwiftPrivate] " : "");
  if (!SwiftName.empty())
    OS << "Swift Name: " << SwiftName << ' ';
  OS << '\n';
}

LLVM_DUMP_METHOD void CommonTypeInfo::dump(llvm::raw_ostream &OS) const {
  static_cast<const CommonEntityInfo &>(*this).dump(OS);
  if (SwiftBridge)
    OS << "Swift Briged Type: " << *SwiftBridge << ' ';
  if (NSErrorDomain)
    OS << "NSError Domain: " << *NSErrorDomain << ' ';
  OS << '\n';
}

LLVM_DUMP_METHOD void ContextInfo::dump(llvm::raw_ostream &OS) {
  static_cast<CommonTypeInfo &>(*this).dump(OS);
  if (HasDefaultNullability)
    OS << "DefaultNullability: " << DefaultNullability << ' ';
  if (HasDesignatedInits)
    OS << "[HasDesignatedInits] ";
  if (SwiftImportAsNonGenericSpecified)
    OS << (SwiftImportAsNonGeneric ? "[SwiftImportAsNonGeneric] " : "");
  if (SwiftObjCMembersSpecified)
    OS << (SwiftObjCMembers ? "[SwiftObjCMembers] " : "");
  OS << '\n';
}

LLVM_DUMP_METHOD void VariableInfo::dump(llvm::raw_ostream &OS) const {
  static_cast<const CommonEntityInfo &>(*this).dump(OS);
  if (NullabilityAudited)
    OS << "Audited Nullability: " << Nullable << ' ';
  if (!Type.empty())
    OS << "C Type: " << Type << ' ';
  OS << '\n';
}

LLVM_DUMP_METHOD void ObjCPropertyInfo::dump(llvm::raw_ostream &OS) const {
  static_cast<const VariableInfo &>(*this).dump(OS);
  if (SwiftImportAsAccessorsSpecified)
    OS << (SwiftImportAsAccessors ? "[SwiftImportAsAccessors] " : "");
  OS << '\n';
}

LLVM_DUMP_METHOD void BoundsSafetyInfo::dump(llvm::raw_ostream &OS) const {
  if (KindAudited) {
    assert((BoundsSafetyKind)Kind >= BoundsSafetyKind::CountedBy);
    assert((BoundsSafetyKind)Kind <= BoundsSafetyKind::EndedBy);
    switch ((BoundsSafetyKind)Kind) {
    case BoundsSafetyKind::CountedBy:
      OS << "[counted_by] ";
      break;
    case BoundsSafetyKind::CountedByOrNull:
      OS << "[counted_by_or_null] ";
      break;
    case BoundsSafetyKind::SizedBy:
      OS << "[sized_by] ";
      break;
    case BoundsSafetyKind::SizedByOrNull:
      OS << "[sized_by_or_null] ";
      break;
    case BoundsSafetyKind::EndedBy:
      OS << "[ended_by] ";
      break;
    }
  }
  if (LevelAudited)
    OS << "Level: " << Level << " ";
  OS << "ExternalBounds: "
     << (ExternalBounds.empty() ? "<missing>" : ExternalBounds) << '\n';
}

LLVM_DUMP_METHOD void ParamInfo::dump(llvm::raw_ostream &OS) const {
  static_cast<const VariableInfo &>(*this).dump(OS);
  if (NoEscapeSpecified)
    OS << (NoEscape ? "[NoEscape] " : "");
  if (LifetimeboundSpecified)
    OS << (Lifetimebound ? "[Lifetimebound] " : "");
  OS << "RawRetainCountConvention: " << RawRetainCountConvention << ' ';
  OS << '\n';
  if (BoundsSafety)
    BoundsSafety->dump(OS);
}

LLVM_DUMP_METHOD void FunctionInfo::dump(llvm::raw_ostream &OS) const {
  static_cast<const CommonEntityInfo &>(*this).dump(OS);
  OS << (NullabilityAudited ? "[NullabilityAudited] " : "")
     << "RawRetainCountConvention: " << RawRetainCountConvention << ' ';
  if (!ResultType.empty())
    OS << "Result Type: " << ResultType << ' ';
  if (!SwiftReturnOwnership.empty())
    OS << "SwiftReturnOwnership: " << SwiftReturnOwnership << ' ';
  if (!Params.empty())
    OS << '\n';
  for (auto &PI : Params)
    PI.dump(OS);
}

LLVM_DUMP_METHOD void ObjCMethodInfo::dump(llvm::raw_ostream &OS) {
  static_cast<FunctionInfo &>(*this).dump(OS);
  if (Self)
    Self->dump(OS);
  OS << (DesignatedInit ? "[DesignatedInit] " : "")
     << (RequiredInit ? "[RequiredInit] " : "") << '\n';
}

LLVM_DUMP_METHOD void CXXMethodInfo::dump(llvm::raw_ostream &OS) {
  static_cast<FunctionInfo &>(*this).dump(OS);
  if (This)
    This->dump(OS);
}

LLVM_DUMP_METHOD void TagInfo::dump(llvm::raw_ostream &OS) {
  static_cast<CommonTypeInfo &>(*this).dump(OS);
  if (HasFlagEnum)
    OS << (IsFlagEnum ? "[FlagEnum] " : "");
  if (EnumExtensibility)
    OS << "Enum Extensibility: " << static_cast<long>(*EnumExtensibility)
       << ' ';
  if (SwiftCopyableSpecified)
    OS << (SwiftCopyable ? "[SwiftCopyable] " : "[~SwiftCopyable]");
  if (SwiftEscapableSpecified)
    OS << (SwiftEscapable ? "[SwiftEscapable] " : "[~SwiftEscapable]");
  OS << '\n';
}

LLVM_DUMP_METHOD void TypedefInfo::dump(llvm::raw_ostream &OS) const {
  static_cast<const CommonTypeInfo &>(*this).dump(OS);
  if (SwiftWrapper)
    OS << "Swift Type: " << static_cast<long>(*SwiftWrapper) << ' ';
  OS << '\n';
}
} // namespace api_notes
} // namespace clang
