// REQUIRES: system-darwin
// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+" --prefix-filecheck-ir-name TMP_
// RUN: %clang_cc1 -O0 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O0 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

// CHECK-LABEL: @foo(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[BUF_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[SIZE_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[AGG_TEMP:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP1:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP2:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP5:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP8:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP16:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP19:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP20:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    store ptr [[BUF:%.*]], ptr [[BUF_ADDR]], align 8
// CHECK-NEXT:    store ptr [[SIZE:%.*]], ptr [[SIZE_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[BUF_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = load ptr, ptr [[SIZE_ADDR]], align 8
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, ptr [[TMP1]], align 4
// CHECK-NEXT:    [[IDX_EXT:%.*]] = sext i32 [[TMP2]] to i64
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[TMP0]], i64 [[IDX_EXT]]
// CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[TMP0]], ptr [[TMP3]], align 8
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[ADD_PTR]], ptr [[TMP4]], align 8
// CHECK-NEXT:    [[TMP5:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[TMP0]], ptr [[TMP5]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP1]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 0, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP2]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP2]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB4:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR3]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP:%.*]] = icmp ule ptr [[WIDE_PTR_PTR]], [[WIDE_PTR_UB4]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[CMP]], label [[LAND_LHS_TRUE:%.*]], label [[LAND_END35:%.*]], {{!annotation ![0-9]+}}
// CHECK:       land.lhs.true:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP5]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR6:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP5]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB7:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR6]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP8]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR9:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP8]], i32 0, i32 0, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR10:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR9]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR11:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP8]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB12:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR11]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR13:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP8]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB14:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR13]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP15:%.*]] = icmp ule ptr [[WIDE_PTR_LB7]], [[WIDE_PTR_PTR10]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[CMP15]], label [[LAND_RHS:%.*]], label [[LAND_END35]], {{!annotation ![0-9]+}}
// CHECK:       land.rhs:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP16]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR17:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP16]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB18:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR17]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP20]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR21:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP20]], i32 0, i32 0, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR22:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR21]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR23:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP20]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB24:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR23]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR25:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP20]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB26:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR25]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP6:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP19]], i32 0, i32 0, {{!annotation ![0-9]+}}
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR22]], ptr [[TMP6]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP7:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP19]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB24]], ptr [[TMP7]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP8:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP19]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB26]], ptr [[TMP8]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR27:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP19]], i32 0, i32 0, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_PTR28:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR27]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR29:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP19]], i32 0, i32 1, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_UB30:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR29]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR31:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP19]], i32 0, i32 2, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[WIDE_PTR_LB32:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR31]], align 8, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_UB18]] to i64, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR28]] to i64, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[CMP33:%.*]] = icmp sle i64 40, [[SUB_PTR_SUB]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[CMP33]], label [[LAND_RHS34:%.*]], label [[LAND_END:%.*]], {{!annotation ![0-9]+}}
// CHECK:       land.rhs34:
// CHECK-NEXT:    br label [[LAND_END]], {{!annotation ![0-9]+}}
// CHECK:       land.end:
// CHECK-NEXT:    [[TMP9:%.*]] = phi i1 [ false, [[LAND_RHS]] ], [ true, [[LAND_RHS34]] ]
// CHECK-NEXT:    br label [[LAND_END35]], {{!annotation ![0-9]+}}
// CHECK:       land.end35:
// CHECK-NEXT:    [[TMP10:%.*]] = phi i1 [ false, [[LAND_LHS_TRUE]] ], [ false, [[ENTRY:%.*]] ], [ [[TMP9]], [[LAND_END]] ], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[TMP10]], label [[CONT:%.*]], label [[TRAP:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR3:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable, {{!annotation ![0-9]+}}
// CHECK:       cont:
// CHECK-NEXT:    [[TMP11:%.*]] = load ptr, ptr [[SIZE_ADDR]], align 8
// CHECK-NEXT:    store i32 40, ptr [[TMP11]], align 4
// CHECK-NEXT:    ret void
//
void foo(int *__sized_by(*size) buf, int *size) {
  *size = 40;
}
