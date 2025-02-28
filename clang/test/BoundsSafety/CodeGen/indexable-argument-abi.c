// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"

// RUN: %clang_cc1 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s --check-prefix=X86_64
// RUN: %clang_cc1 -triple aarch64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s --check-prefix=AARCH64
// RUN: %clang_cc1 -triple i686 -fbounds-safety -emit-llvm %s -o - | FileCheck %s --check-prefix=I686
// RUN: %clang_cc1 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s --check-prefix=X86_64
// RUN: %clang_cc1 -triple aarch64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s --check-prefix=AARCH64
// RUN: %clang_cc1 -triple i686 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s --check-prefix=I686

#include <ptrcheck.h>

int*__indexable bar(int *__indexable ptrArg) {
    return ptrArg;
}

// X86_64-LABEL: @bar(
// X86_64:  entry:
// X86_64:    [[RETVAL:%.*]] = alloca %"__bounds_safety::wide_ptr.indexable", align 8
// X86_64:    [[PTRARG:%.*]] = alloca %"__bounds_safety::wide_ptr.indexable", align 8
// X86_64:    [[TMP0:%.*]] = getelementptr inbounds { ptr, ptr }, ptr [[PTRARG]], i32 0, i32 0
// X86_64:    store ptr [[PTRARG_COERCE0:%.*]], ptr [[TMP0]], align 8
// X86_64:    [[TMP1:%.*]] = getelementptr inbounds { ptr, ptr }, ptr [[PTRARG]], i32 0, i32 1
// X86_64:    store ptr [[PTRARG_COERCE1:%.*]], ptr [[TMP1]], align 8
// X86_64:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[RETVAL]], ptr align 8 [[PTRARG]], i64 16, i1 false)
// X86_64:    [[TMP2:%.*]] = load { ptr, ptr }, ptr [[RETVAL]], align 8
// X86_64:    ret { ptr, ptr } [[TMP2]]
//
// AARCH64-LABEL: @bar(
// AARCH64:  entry:
// AARCH64:    [[RETVAL:%.*]] = alloca %"__bounds_safety::wide_ptr.indexable", align 8
// AARCH64:    [[PTRARG:%.*]] = alloca %"__bounds_safety::wide_ptr.indexable", align 8
// AARCH64:    store [2 x i64] [[PTRARG_COERCE:%.*]], ptr [[PTRARG]], align 8
// AARCH64:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[RETVAL]], ptr align 8 [[PTRARG]], i64 16, i1 false)
// AARCH64:    [[TMP0:%.*]] = load [2 x i64], ptr [[RETVAL]], align 8
// AARCH64:    ret [2 x i64] [[TMP0]]
//
// I686-LABEL: @bar(
// I686:  entry:
// I686:    [[RESULT_PTR:%.*]] = alloca ptr, align 4
// I686:    store ptr [[AGG_RESULT:%.*]], ptr [[RESULT_PTR]], align 4
// I686:    call void @llvm.memcpy.p0.p0.i32(ptr align 4 [[AGG_RESULT]], ptr align 4 [[PTRARG:%.*]], i32 8, i1 false)
// I686:    ret void
//




