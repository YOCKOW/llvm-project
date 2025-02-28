// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py

// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck --check-prefix=CHECK-O2 %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck --check-prefix=CHECK-O2 %s

#include <ptrcheck.h>

// CHECK-O2-LABEL: @TestFail(
// CHECK-O2-NEXT:  entry:
// CHECK-O2-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR3:[0-9]+]], !annotation !{{.*}}
// CHECK-O2-NEXT:    unreachable
//
int TestFail() {
    int array[5] = {4, 3, 2, 1, 0};
    int len = 2;
    int *__counted_by(len) dcp = array;
    int *bp = dcp;
    return bp[2]; // oob - run-time trap
}

// CHECK-O2-LABEL: @TestOK(
// CHECK-O2-NEXT:  entry:
// CHECK-O2-NEXT:    ret i32 3
//
int TestOK() {
  int array[5] = {4, 3, 2, 1, 0};
  int len = 2;
  int *__counted_by(len) dcp = array;
  int *bp = dcp;
  return bp[1];
}
