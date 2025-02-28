// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+" --prefix-filecheck-ir-name TMP_

// RUN: %clang_cc1 -O2 -fbounds-safety -emit-llvm -triple x86_64 %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm -triple x86_64 %s -o - | FileCheck %s

#include <ptrcheck.h>

struct RangePtrs {
  int *__ended_by(iter) start;
  int *__ended_by(end) iter;
  void *end;
};

// CHECK-LABEL: @TestOK(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    ret void
//
void TestOK(void) {
  int arr[10];
  struct RangePtrs rptrs = { arr, arr, arr + 10 };
}

// CHECK-LABEL: @TestRangeFail1(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
//
void TestRangeFail1(void) {
  int arr[10];
  struct RangePtrs rptrs = { arr + 2, arr + 1, arr + 10 };
}

// CHECK-LABEL: @TestRangeFail2(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
//
void TestRangeFail2(void) {
  int arr[10];
  struct RangePtrs rptrs = { arr, arr + 2, arr + 1 };
}

// CHECK-LABEL: @TestStartFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
//
void TestStartFail(void) {
  int arr[10];
  struct RangePtrs rptrs = { arr - 1, arr, arr + 10 };
}

// CHECK-LABEL: @TestIterFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARR:%.*]] = alloca [10 x i32], align 16
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR6:[0-9]+]]
// CHECK-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 40
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[ARR]], i64 44
// CHECK-NEXT:    [[CMP_NOT:%.*]] = icmp ugt ptr [[BOUND_PTR_ARITH]], [[UPPER]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP34_NOT:%.*]] = icmp ugt ptr [[ARR]], [[BOUND_PTR_ARITH]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND:%.*]] = or i1 [[CMP_NOT]], [[CMP34_NOT]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND]], label [[TRAP:%.*]], label [[CONT86:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
// CHECK:       cont86:
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR6]]
// CHECK-NEXT:    ret void
//
void TestIterFail(void) {
  int arr[10];
  struct RangePtrs rptrs = { arr, arr + 11, arr + 11 };
}

// CHECK-LABEL: @TestEndFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARR:%.*]] = alloca [10 x i32], align 16
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR6]]
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[ARR]], i64 44
// CHECK-NEXT:    [[UPPER3:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 40
// CHECK-NEXT:    [[CMP28_NOT:%.*]] = icmp ugt ptr [[BOUND_PTR_ARITH]], [[UPPER3]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP43_NOT:%.*]] = icmp ugt ptr [[ARR]], [[BOUND_PTR_ARITH]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND:%.*]] = or i1 [[CMP28_NOT]], [[CMP43_NOT]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND]], label [[TRAP:%.*]], label [[CONT55:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
// CHECK:       cont55:
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 40, ptr nonnull [[ARR]]) #[[ATTR6]]
// CHECK-NEXT:    ret void
//
void TestEndFail(void) {
  int arr[10];
  struct RangePtrs rptrs = { arr, arr, arr + 11 };
}
