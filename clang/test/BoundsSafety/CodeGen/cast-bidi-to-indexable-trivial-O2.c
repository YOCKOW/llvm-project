// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

static struct {
  int _a[42];
  int array[3];
  int _b[42];
} foo;

// CHECK-LABEL: @good(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret { ptr, ptr } { ptr getelementptr inbounds ([[STRUCT_ANON:%.*]], ptr @foo, i32 0, i32 1), ptr getelementptr inbounds (i32, ptr getelementptr inbounds ([[STRUCT_ANON]], ptr @foo, i32 0, i32 1), i64 3) }
//
int *__indexable good(void) {
  int *__bidi_indexable p = foo.array;
  return p;
}

// CHECK-LABEL: @good_upper(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret { ptr, ptr } { ptr getelementptr inbounds (i8, ptr @foo, i64 176), ptr getelementptr inbounds (i32, ptr getelementptr inbounds ([[STRUCT_ANON:%.*]], ptr @foo, i32 0, i32 1), i64 3) }
//
int *__indexable good_upper(void) {
  int *__bidi_indexable p = foo.array;
  p += 2;
  return p;
}

// CHECK-LABEL: @good_upper2(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret { ptr, ptr } { ptr getelementptr inbounds (i8, ptr @foo, i64 208), ptr getelementptr inbounds (i32, ptr getelementptr inbounds ([[STRUCT_ANON:%.*]], ptr @foo, i32 0, i32 1), i64 3) }
//
int *__indexable good_upper2(void) {
  int *__bidi_indexable p = foo.array;
  p += 10;
  return p;
}

// CHECK-LABEL: @bad_lower(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR3:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
//
int *__indexable bad_lower(void) {
  int *__bidi_indexable p = foo.array;
  p -= 1;
  return p;
}

// CHECK-LABEL: @good_null(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret { ptr, ptr } zeroinitializer
//
int *__indexable good_null(void) {
  int *__bidi_indexable p = 0;
  return p;
}

// CHECK-LABEL: @bad_null(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR3]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
//
int *__indexable bad_null(void) {
  // Try to cast { null, non-null, non-null } and make sure that we don't
  // return { null, non-null }.
  int *__bidi_indexable p = foo.array;
  while (--p)
    ;
  return p;
}
