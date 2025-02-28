// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"
// RUN: %clang_cc1 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

// CHECK-LABEL: @test(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[P:%.*]] = alloca %"__bounds_safety::wide_ptr.indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP:%.*]] = alloca %"__bounds_safety::wide_ptr.indexable", align 8
// CHECK-NEXT:    [[TMP0:%.*]] = getelementptr inbounds { ptr, ptr }, ptr [[P]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[P_COERCE0:%.*]], ptr [[TMP0]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds { ptr, ptr }, ptr [[P]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[P_COERCE1:%.*]], ptr [[TMP1]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP]], ptr align 8 [[P]], i64 16, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.indexable", ptr [[AGG_TEMP]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.indexable", ptr [[AGG_TEMP]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR]], align 8
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_RESULT:%.*]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR]], ptr [[TMP2]], align 8
// CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_RESULT]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB]], ptr [[TMP3]], align 8
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_RESULT]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR]], ptr [[TMP4]], align 8
// CHECK-NEXT:    ret void
//
int *__bidi_indexable test(int *__indexable p) {
  return p;
}
