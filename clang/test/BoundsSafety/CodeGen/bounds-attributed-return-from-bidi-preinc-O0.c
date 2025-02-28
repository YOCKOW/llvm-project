// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 5




// RUN: %clang_cc1 -O0 -triple arm64-apple-iphoneos -fbounds-safety -fbounds-safety-bringup-missing-checks=return_size -emit-llvm %s -o - | FileCheck %s

// XFAIL: !rdar137311963

#include <ptrcheck.h>

// CHECK-LABEL: define dso_local ptr @foo(
// CHECK-SAME: i32 noundef [[COUNT:%.*]], ptr noundef [[BUF:%.*]]) #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*]]:
// CHECK-NEXT:    [[COUNT_ADDR:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[BUF_ADDR:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    [[LOCAL:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[TMP:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP1:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP2:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP5:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP8:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP16:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP19:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP32:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    store i32 [[COUNT]], ptr [[COUNT_ADDR]], align 4
// CHECK-NEXT:    store ptr [[BUF]], ptr [[BUF_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[BUF_ADDR]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[COUNT_ADDR]], align 4
// CHECK-NEXT:    [[IDX_EXT:%.*]] = sext i32 [[TMP1]] to i64
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i32, ptr [[TMP0]], i64 [[IDX_EXT]]
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[LOCAL]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[TMP0]], ptr [[TMP2]], align 8
// CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[LOCAL]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[ADD_PTR]], ptr [[TMP3]], align 8
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[LOCAL]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[TMP0]], ptr [[TMP4]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[TMP]], ptr align 8 [[LOCAL]], i64 24, i1 false)
// CHECK-NEXT:    [[TMP5:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[TMP]], i32 0, i32 0
// CHECK-NEXT:    [[TMP6:%.*]] = load ptr, ptr [[TMP5]], align 8
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i32, ptr [[TMP6]], i64 1
// CHECK-NEXT:    [[TMP7:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[LOCAL]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[BOUND_PTR_ARITH]], ptr [[TMP7]], align 8
// CHECK-NEXT:    [[TMP8:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[TMP]], i32 0, i32 1
// CHECK-NEXT:    [[TMP9:%.*]] = load ptr, ptr [[TMP8]], align 8
// CHECK-NEXT:    [[TMP10:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[LOCAL]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[TMP9]], ptr [[TMP10]], align 8
// CHECK-NEXT:    [[TMP11:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[TMP]], i32 0, i32 2
// CHECK-NEXT:    [[TMP12:%.*]] = load ptr, ptr [[TMP11]], align 8
// CHECK-NEXT:    [[TMP13:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[LOCAL]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[TMP12]], ptr [[TMP13]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP]], ptr align 8 [[LOCAL]], i64 24, i1 false)
// CHECK-NEXT:    [[TMP14:%.*]] = load i32, ptr [[COUNT_ADDR]], align 4
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP1]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), !annotation [[META2:![0-9]+]]
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 0, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_PTR:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 1, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 2, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR]], align 8, !annotation [[META2]]
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP2]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR3:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP2]], i32 0, i32 1, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB4:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR3]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[CMP:%.*]] = icmp ule ptr [[WIDE_PTR_PTR]], [[WIDE_PTR_UB4]], !annotation [[META2]]
// CHECK-NEXT:    br i1 [[CMP]], label %[[LAND_LHS_TRUE:.*]], label %[[LAND_END31:.*]], !annotation [[META2]]
// CHECK:       [[LAND_LHS_TRUE]]:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP5]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR6:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP5]], i32 0, i32 2, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB7:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR6]], align 8, !annotation [[META2]]
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP8]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR9:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP8]], i32 0, i32 0, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_PTR10:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR9]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR11:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP8]], i32 0, i32 1, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB12:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR11]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR13:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP8]], i32 0, i32 2, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB14:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR13]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[CMP15:%.*]] = icmp ule ptr [[WIDE_PTR_LB7]], [[WIDE_PTR_PTR10]], !annotation [[META2]]
// CHECK-NEXT:    br i1 [[CMP15]], label %[[LAND_RHS:.*]], label %[[LAND_END31]], !annotation [[META2]]
// CHECK:       [[LAND_RHS]]:
// CHECK-NEXT:    [[CONV:%.*]] = sext i32 [[TMP14]] to i64, !annotation [[META2]]
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP16]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR17:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP16]], i32 0, i32 1, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB18:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR17]], align 8, !annotation [[META2]]
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP19]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false), !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR20:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP19]], i32 0, i32 0, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_PTR21:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR20]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR22:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP19]], i32 0, i32 1, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_UB23:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR22]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR24:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP19]], i32 0, i32 2, !annotation [[META2]]
// CHECK-NEXT:    [[WIDE_PTR_LB25:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR24]], align 8, !annotation [[META2]]
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_UB18]] to i64, !annotation [[META2]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR21]] to i64, !annotation [[META2]]
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]], !annotation [[META2]]
// CHECK-NEXT:    [[SUB_PTR_DIV:%.*]] = sdiv exact i64 [[SUB_PTR_SUB]], 4, !annotation [[META2]]
// CHECK-NEXT:    [[CMP26:%.*]] = icmp sle i64 [[CONV]], [[SUB_PTR_DIV]], !annotation [[META2]]
// CHECK-NEXT:    br i1 [[CMP26]], label %[[LAND_RHS28:.*]], label %[[LAND_END:.*]], !annotation [[META2]]
// CHECK:       [[LAND_RHS28]]:
// CHECK-NEXT:    [[CMP29:%.*]] = icmp sle i32 0, [[TMP14]], !annotation [[META2]]
// CHECK-NEXT:    br label %[[LAND_END]], !annotation [[META2]]
// CHECK:       [[LAND_END]]:
// CHECK-NEXT:    [[TMP15:%.*]] = phi i1 [ false, %[[LAND_RHS]] ], [ [[CMP29]], %[[LAND_RHS28]] ]
// CHECK-NEXT:    br label %[[LAND_END31]], !annotation [[META2]]
// CHECK:       [[LAND_END31]]:
// CHECK-NEXT:    [[TMP16:%.*]] = phi i1 [ false, %[[LAND_LHS_TRUE]] ], [ false, %[[ENTRY]] ], [ [[TMP15]], %[[LAND_END]] ], !annotation [[META2]]
// CHECK-NEXT:    br i1 [[TMP16]], label %[[CONT:.*]], label %[[TRAP:.*]], !annotation [[META2]]
// CHECK:       [[TRAP]]:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR3:[0-9]+]], !annotation [[META2]]
// CHECK-NEXT:    unreachable, !annotation [[META2]]
// CHECK:       [[CONT]]:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP32]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR33:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP32]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR34:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR33]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR35:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP32]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB36:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR35]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR37:%.*]] = getelementptr inbounds nuw %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP32]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB38:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR37]], align 8
// CHECK-NEXT:    ret ptr [[WIDE_PTR_PTR34]]
//
int *__counted_by(count) foo(int count, int *__counted_by(count) buf) {
  int *local = buf;
  return ++local;
}
//.
// CHECK: [[META2]] = !{!"bounds-safety-generic"}
//.
