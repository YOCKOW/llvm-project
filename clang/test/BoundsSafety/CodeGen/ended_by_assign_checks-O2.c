// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+" --prefix-filecheck-ir-name TMP_
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>
struct S {
    int *__ended_by(end) start;
    int *end;
};

// CHECK-LABEL: @TestOK(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret i32 0
//
int TestOK() {
  int arr[10];
  struct S s;
  s.start = arr;
  s.end = arr + 10;
  return 0;
}

// CHECK-LABEL: @TestStartFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARR:%.*]] = alloca [10 x i32], align 16
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR5:[0-9]+]]
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[ARR]], i64 -4
// CHECK-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 40
// CHECK-NEXT:    [[CMP24_NOT:%.*]] = icmp ugt ptr [[BOUND_PTR_ARITH]], [[UPPER]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP35_NOT:%.*]] = icmp ugt ptr [[ARR]], [[BOUND_PTR_ARITH]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND:%.*]] = or i1 [[CMP24_NOT]], [[CMP35_NOT]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND]], label [[TRAP:%.*]], label [[CONT:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR6:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
// CHECK:       cont:
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR5]]
// CHECK-NEXT:    ret i32 0
//
int TestStartFail() {
    int arr[10];
    struct S s;
    s.start = arr - 1;
    s.end = arr + 10;
    return 0;
}

// CHECK-LABEL: @TestEndFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARR:%.*]] = alloca [10 x i32], align 16
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR5]]
// CHECK-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 40
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[ARR]], i64 44
// CHECK-NEXT:    [[CMP_NOT:%.*]] = icmp ugt ptr [[BOUND_PTR_ARITH]], [[UPPER]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP22_NOT:%.*]] = icmp ugt ptr [[ARR]], [[BOUND_PTR_ARITH]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND:%.*]] = or i1 [[CMP_NOT]], [[CMP22_NOT]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND]], label [[TRAP:%.*]], label [[CONT:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR6]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
// CHECK:       cont:
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR5]]
// CHECK-NEXT:    ret i32 0
//
int TestEndFail() {
  int arr[10];
  struct S s;
  s.start = arr;
  s.end = arr + 11;
  return 0;
}

// CHECK-LABEL: @TestRangeFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
//
int TestRangeFail() {
  int arr[10];
  struct S s;
  s.start = arr + 1;
  s.end = arr;
  return 0;
}
