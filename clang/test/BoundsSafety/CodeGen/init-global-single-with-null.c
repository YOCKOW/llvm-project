// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"

// RUN: %clang_cc1 -triple x86_64 -O0 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -triple x86_64 -O0 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

int* __single ptr = 0;
int* __single ptr2 = (int *)(void *)0;

// CHECK: @ptr = {{.*}} ptr null
// CHECK: @ptr2 = {{.*}} ptr null

