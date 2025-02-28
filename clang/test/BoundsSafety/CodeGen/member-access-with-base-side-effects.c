// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+" --prefix-filecheck-ir-name TMP_

// RUN: %clang_cc1 -triple x86_64 -O2 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64 -O2 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

struct foo {
    int *__counted_by(count) p;
    int count;
};

struct foo *bar(void);

// CHECK-LABEL: @baz(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @bar() #[[ATTR2:[0-9]+]]
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[CALL]], align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[COUNT:%.*]] = getelementptr inbounds i8, ptr [[CALL]], i64 8
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[COUNT]], align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[IDX_EXT:%.*]] = sext i32 [[TMP1]] to i64
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i32, ptr [[TMP0]], i64 [[IDX_EXT]]
// CHECK-NEXT:    [[DOTFCA_0_INSERT:%.*]] = insertvalue { ptr, ptr } poison, ptr [[TMP0]], 0
// CHECK-NEXT:    [[DOTFCA_1_INSERT:%.*]] = insertvalue { ptr, ptr } [[DOTFCA_0_INSERT]], ptr [[ADD_PTR]], 1
// CHECK-NEXT:    ret { ptr, ptr } [[DOTFCA_1_INSERT]]
//
int *__indexable baz(void) {
    return bar()->p;
}

