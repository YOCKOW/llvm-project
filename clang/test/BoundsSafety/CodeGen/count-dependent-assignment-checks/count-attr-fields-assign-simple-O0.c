// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+" --prefix-filecheck-ir-name TMP_
// RUN: %clang_cc1 -O0 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

struct Foo {
    int len;
    int *__counted_by(len) ptr;
};

// CHECK-LABEL: @Test(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[ARR:%.*]] = alloca [10 x i32], align 16
// CHECK-NEXT:    [[F:%.*]] = alloca [[STRUCT_FOO:%.*]], align 8
// CHECK-NEXT:    [[N:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[AGG_TEMP:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP1:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP2:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP9:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[LEN:%.*]] = getelementptr inbounds [[STRUCT_FOO]], ptr [[F]], i32 0, i32 0
// CHECK-NEXT:    store i32 0, ptr [[LEN]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[PTR:%.*]] = getelementptr inbounds [[STRUCT_FOO]], ptr [[F]], i32 0, i32 1
// CHECK-NEXT:    store ptr null, ptr [[PTR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    store i32 12, ptr [[N]], align 4
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr [[N]], align 4
// CHECK-NEXT:    [[ARRAYDECAY:%.*]] = getelementptr inbounds [10 x i32], ptr [[ARR]], i64 0, i64 0
// CHECK-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i32, ptr [[ARRAYDECAY]], i64 10
// CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[ARRAYDECAY]], ptr [[TMP1]], align 8
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[UPPER]], ptr [[TMP2]], align 8
// CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[ARRAYDECAY]], ptr [[TMP3]], align 8
// CHECK-NEXT:    [[CONV:%.*]] = sext i32 [[TMP0]] to i64, {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP1]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP2]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP2]], i32 0, i32 0, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP2]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB4:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR3]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP2]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_UB]] to i64, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR]] to i64, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_DIV:%.*]] = sdiv exact i64 [[SUB_PTR_SUB]], 4, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP:%.*]] = icmp sle i64 [[CONV]], [[SUB_PTR_DIV]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[CMP]], label [[LAND_RHS:%.*]], label [[LAND_END:%.*]], {{!annotation ![0-9]+}}
// CHECK:       land.rhs:
// CHECK-NEXT:    [[CMP6:%.*]] = icmp sle i32 0, [[TMP0]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br label [[LAND_END]], {{!annotation ![0-9]+}}
// CHECK:       land.end:
// CHECK-NEXT:    [[TMP4:%.*]] = phi i1 [ false, [[ENTRY:%.*]] ], [ [[CMP6]], [[LAND_RHS]] ], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[TMP4]], label [[CONT:%.*]], label [[TRAP:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR3:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
// CHECK:       cont:
// CHECK-NEXT:    [[LEN8:%.*]] = getelementptr inbounds [[STRUCT_FOO]], ptr [[F]], i32 0, i32 0
// CHECK-NEXT:    store i32 [[TMP0]], ptr [[LEN8]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP9]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR10:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP9]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR11:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR10]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR12:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP9]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB13:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR12]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR14:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP9]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB15:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR14]], align 8
// CHECK-NEXT:    [[PTR16:%.*]] = getelementptr inbounds [[STRUCT_FOO]], ptr [[F]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR11]], ptr [[PTR16]], align 8
// CHECK-NEXT:    ret void
//
void Test() {
    int arr[10];
    struct Foo f;
    int n = 12;
    f.len = n;
    f.ptr = arr;
}
