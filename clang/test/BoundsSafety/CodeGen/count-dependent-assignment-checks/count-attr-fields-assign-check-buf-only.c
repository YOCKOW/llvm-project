
// RUN: %clang_cc1 -O0  -fbounds-safety -emit-llvm -triple arm64-apple-darwin %s -o /dev/null
// RUN: %clang_cc1 -O2  -fbounds-safety -emit-llvm -triple arm64-apple-darwin %s -o - | FileCheck %s

#include <ptrcheck.h>

struct S {
    int *__counted_by(l) bp;
    int *bp2 __counted_by(l+1);
    int l;
};

int foo () {
    int arr[10];
    struct S s = {arr, arr, 0};
    s.bp = &arr[1];
    s.bp2 = arr;
    s.l = 9;
    // run-time check here

    for (int i = 0; i < s.l; ++i)
        s.bp[i] = i;

    s.bp = &arr[9]; // trap: can't make the bounds smaller than s.l
    s.l = s.l;
    s.bp2 = s.bp2;

    return 0;
}

int bar () {
    int arr[10];
    struct S s = {arr, arr, 0};
    s.bp = &arr[1];
    s.bp2 = arr;
    s.l = 9;
    // run-time check here

    for (int i = 0; i < s.l; ++i)
        s.bp[i] = i;

    s.bp = &arr[0]; // no trap
    s.l = s.l;
    s.bp2 = s.bp2;

    return 0;
}

// CHECK: define noundef i32 @foo()
// CHECK: {{.*}}:
// CHECK:   call void @llvm.ubsantrap(i8 25)
// CHECK:   unreachable
// CHECK: }

// CHECK: define noundef i32 @bar()
// CHECK: {{.*}}:
// CHECK:   ret i32 0
// CHECK: }
