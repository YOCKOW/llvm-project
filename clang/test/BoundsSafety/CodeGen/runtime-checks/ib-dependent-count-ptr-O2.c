// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"

// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck --check-prefix=CHECK-O2 %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -emit-llvm -mllvm -enable-constraint-elimination %s -o - | FileCheck --check-prefix=CHECK-O2 %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck --check-prefix=CHECK-O2 %s
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm -mllvm -enable-constraint-elimination %s -o - | FileCheck --check-prefix=CHECK-O2 %s

// piggy-back test for -fcoverage-mapping
// RUN: %clang_cc1 -O2 -triple x86_64 -fbounds-safety -fprofile-instrument=clang -fcoverage-mapping -emit-llvm %s -o /dev/null

#include <ptrcheck.h>

// The range check can be removed because 'i >= 0 && i < len'.

// CHECK-O2-LABEL: @foo(
// CHECK-O2-NEXT:  entry:
// CHECK-O2-NEXT:    [[CMP6:%.*]] = icmp sgt i32 [[LEN:%.*]], 0
// CHECK-O2-NEXT:    br i1 [[CMP6]], label [[CONT1_PREHEADER:%.*]], label [[FOR_COND_CLEANUP:%.*]]
// CHECK-O2:       for.body.preheader:
// CHECK-O2-NEXT:    [[WIDE_TRIP_COUNT:%.*]] = zext nneg i32 [[LEN]] to i64
// CHECK-O2-NEXT:    br label [[CONT1:%.*]]
// CHECK-O2:       for.cond.cleanup:
// CHECK-O2-NEXT:    ret void
// CHECK-O2:       for.body:
// CHECK-O2-NEXT:    [[INDVARS_IV:%.*]] = phi i64 [ 0, [[CONT1_PREHEADER]] ], [ [[INDVARS_IV_NEXT:%.*]], [[CONT1]] ]
// CHECK-O2-NEXT:    [[TMP0:%.*]] = getelementptr i32, ptr [[BUF:%.*]], i64 [[INDVARS_IV]]
// CHECK-O2-NEXT:    [[TMP1:%.*]] = trunc nuw nsw i64 [[INDVARS_IV]] to i32
// CHECK-O2-NEXT:    store i32 [[TMP1]], ptr [[TMP0]], align 4, {{!tbaa ![0-9]+}}
// CHECK-O2-NEXT:    [[INDVARS_IV_NEXT]] = add nuw nsw i64 [[INDVARS_IV]], 1
// CHECK-O2-NEXT:    [[EXITCOND_NOT:%.*]] = icmp eq i64 [[INDVARS_IV_NEXT]], [[WIDE_TRIP_COUNT]]
// CHECK-O2-NEXT:    br i1 [[EXITCOND_NOT]], label [[FOR_COND_CLEANUP]], label [[CONT1]], !llvm.loop [[LOOP6:![0-9]+]]
//
void foo(int *__counted_by(len) buf, int len) {
    for (int i = 0; i < len; ++i)
        buf[i] = i;
}

// CHECK-O2-LABEL: @main(
// CHECK-O2-NEXT:  entry:
// CHECK-O2-NEXT:    ret i32 9
//
int main() {
    int arr[10];
    foo(arr, 10);
    return arr[9];
}
