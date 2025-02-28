// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"

// RUN: %clang_cc1 -O2 -fbounds-safety -triple x86_64 -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -triple x86_64 -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

struct Packet {
    int *__counted_by(len) buf;
    int len;
};

struct PacketUnsigned {
  int *__counted_by(len) buf;
  unsigned long long len;
};

int *__counted_by(cnt) my_alloc_int(int cnt);
void *__sized_by(siz) my_alloc(int siz);
int *__sized_by(siz) my_alloc_int_siz(int siz);


// CHECK-LABEL: @TestCountIntFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int(i32 noundef 9) #[[ATTR4:[0-9]+]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
void TestCountIntFail(void) {
    struct Packet p;
    int siz = 9;
    p.buf = my_alloc_int(siz);
    p.len = 10;
}

// CHECK-LABEL: @TestCountNegFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int(i32 noundef -1) #[[ATTR4]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
void TestCountNegFail(void) {
  struct Packet p;
  int siz = -1;
  p.buf = my_alloc_int(siz);
  p.len = 10;
}

// CHECK-LABEL: @TestUCountNegFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int(i32 noundef -1) #[[ATTR4]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
void TestUCountNegFail(void) {
  struct PacketUnsigned p;
  int siz = -1;
  p.buf = my_alloc_int(siz);
  p.len = 10;
}

// CHECK-LABEL: @TestCountNegRetFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int(i32 noundef -1) #[[ATTR4]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
void TestCountNegRetFail(void) {
  int *local_p = my_alloc_int(-1); //rdar://80808704
  (void)*local_p;
}

// CHECK-LABEL: @TestCountIntOK(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int(i32 noundef 10) #[[ATTR4]]
// CHECK-NEXT:    ret void
//
void TestCountIntOK(void) {
  struct Packet p;
  int siz = 10;
  p.buf = my_alloc_int(siz);
  p.len = 10;
}

// CHECK-LABEL: @TestSizeFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc(i32 noundef 39) #[[ATTR4]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
void TestSizeFail(void) {
  struct Packet p;
  int siz = 10 * sizeof(int) - 1;
  p.buf = my_alloc(siz);
  p.len = 10;
}

// CHECK-LABEL: @TestSizeOK(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc(i32 noundef 40) #[[ATTR4]]
// CHECK-NEXT:    ret void
//
void TestSizeOK(void) {
  struct Packet p;
  int siz = 10 * sizeof(int);
  p.buf = my_alloc(siz);
  p.len = 10;
}

// CHECK-LABEL: @TestIntSizeFail(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int_siz(i32 noundef 39) #[[ATTR4]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
void TestIntSizeFail(void) {
  struct Packet p;
  int siz = 10 * sizeof(int) - 1;
  p.buf = my_alloc_int_siz(siz);
  p.len = 10;
}

// CHECK-LABEL: @TestIntSizeOK(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CALL:%.*]] = tail call ptr @my_alloc_int_siz(i32 noundef 40) #[[ATTR4]]
// CHECK-NEXT:    ret void
//
void TestIntSizeOK(void) {
  struct Packet p;
  int siz = 10 * sizeof(int);
  p.buf = my_alloc_int_siz(siz);
  p.len = 10;
}
