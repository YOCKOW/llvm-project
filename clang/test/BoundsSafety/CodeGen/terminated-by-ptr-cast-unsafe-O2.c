// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

// CHECK-LABEL: @unsafe_to_null_terminated(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret ptr [[P:%.*]]
//
const char *__null_terminated unsafe_to_null_terminated(const char *__unsafe_indexable p) {
  return __unsafe_forge_terminated_by(const char *, p, 0);
}

// CHECK-LABEL: @null_terminated_to_unsafe(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret ptr [[P:%.*]]
//
const char *__unsafe_indexable null_terminated_to_unsafe(const char *__null_terminated p) {
  return p;
}
