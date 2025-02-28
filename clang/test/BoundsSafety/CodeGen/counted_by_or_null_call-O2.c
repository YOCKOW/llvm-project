// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 4
// RUN: %clang_cc1 -O2 -triple arm64-apple-iphoneos -fbounds-safety -Wno-bounds-safety-init-list -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -triple arm64-apple-iphoneos -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -Wno-bounds-safety-init-list -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

void foo(int *__counted_by_or_null(len) p, int len);

// CHECK-LABEL: define dso_local void @caller_1(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @foo(ptr noundef null, i32 noundef 2) #[[ATTR5:[0-9]+]]
// CHECK-NEXT:    ret void
//
void caller_1() {
  foo(0, 2);
}

// CHECK-LABEL: define dso_local void @caller_2(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @foo(ptr noundef null, i32 noundef 0) #[[ATTR5]]
// CHECK-NEXT:    ret void
//
void caller_2() {
  foo(0, 0);
}

// rdar://118117905
// CHECK-LABEL: define dso_local void @caller_3(
// CHECK-SAME: ptr noundef [[P:%.*]], i32 noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[DOTNOT:%.*]] = icmp ne ptr [[P]], null, !annotation [[META2:![0-9]+]]
// CHECK-NEXT:    [[CMP_NOT65:%.*]] = icmp slt i32 [[LEN]], 0
// CHECK-NEXT:    [[CMP_NOT:%.*]] = and i1 [[DOTNOT]], [[CMP_NOT65]], !annotation [[META3:![0-9]+]]
// CHECK-NEXT:    br i1 [[CMP_NOT]], label [[TRAP:%.*]], label [[CONT:%.*]], !annotation [[META3]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6:[0-9]+]], !annotation [[META3]]
// CHECK-NEXT:    unreachable, !annotation [[META3]]
// CHECK:       cont:
// CHECK-NEXT:    tail call void @foo(ptr noundef [[P]], i32 noundef [[LEN]]) #[[ATTR5]]
// CHECK-NEXT:    ret void
//
void caller_3(int *__counted_by_or_null(len) p, int len) {
  foo(p, len);
}

// CHECK-LABEL: define dso_local void @caller_4(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR3:[0-9]+]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6]], !annotation [[META3]]
// CHECK-NEXT:    unreachable, !annotation [[META3]]
//
void caller_4() {
    int i = 0;
    foo(&i, -1);
}

// CHECK-LABEL: define dso_local void @caller_5(
// CHECK-SAME: ) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6]], !annotation [[META3]]
// CHECK-NEXT:    unreachable, !annotation [[META3]]
//
void caller_5() {
    int i = 0;
    foo(&i, 2);
}

// CHECK-LABEL: define dso_local void @caller_6(
// CHECK-SAME: ptr noundef [[P:%.*]], i32 noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CMP_NOT:%.*]] = icmp slt i32 [[LEN]], 0, !annotation [[META3]]
// CHECK-NEXT:    br i1 [[CMP_NOT]], label [[TRAP:%.*]], label [[LAND_RHS:%.*]], !annotation [[META3]]
// CHECK:       land.rhs:
// CHECK-NEXT:    tail call void @foo(ptr noundef [[P]], i32 noundef [[LEN]]) #[[ATTR5]]
// CHECK-NEXT:    ret void
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6]], !annotation [[META3]]
// CHECK-NEXT:    unreachable, !annotation [[META3]]
//
void caller_6(int *__counted_by(len) p, int len) {
  foo(p, len);
}

// CHECK-LABEL: define dso_local void @caller_7(
// CHECK-SAME: ptr nocapture noundef readonly [[P:%.*]], i32 noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[P]], align 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_9_0_P_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[P]], i64 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_9_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP_SROA_9_0_P_SROA_IDX]], align 8
// CHECK-NEXT:    [[CMP_NOT:%.*]] = icmp ugt ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], [[AGG_TEMP_SROA_9_0_COPYLOAD]], !annotation [[META3]]
// CHECK-NEXT:    br i1 [[CMP_NOT]], label [[TRAP:%.*]], label [[LAND_LHS_TRUE:%.*]], !annotation [[META3]]
// CHECK:       land.lhs.true:
// CHECK-NEXT:    [[AGG_TEMP_SROA_17_0_P_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[P]], i64 16
// CHECK-NEXT:    [[AGG_TEMP_SROA_17_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP_SROA_17_0_P_SROA_IDX]], align 8, !tbaa [[TBAA5:!.+]]
// CHECK-NEXT:    [[CMP27_NOT:%.*]] = icmp ugt ptr [[AGG_TEMP_SROA_17_0_COPYLOAD]], [[AGG_TEMP_SROA_0_0_COPYLOAD]], !annotation [[META3]]
// CHECK-NEXT:    br i1 [[CMP27_NOT]], label [[TRAP]], label [[LAND_RHS:%.*]], !annotation [[META3]]
// CHECK:       land.rhs:
// CHECK-NEXT:    [[TOBOOL_NOT:%.*]] = icmp eq ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], null, !annotation [[META3]]
// CHECK-NEXT:    br i1 [[TOBOOL_NOT]], label [[CONT:%.*]], label [[LOR_RHS:%.*]], !annotation [[META3]]
// CHECK:       lor.rhs:
// CHECK-NEXT:    [[CONV:%.*]] = sext i32 [[LEN]] to i64, !annotation [[META3]]
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[AGG_TEMP_SROA_9_0_COPYLOAD]] to i64, !annotation [[META3]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]] to i64, !annotation [[META3]]
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]], !annotation [[META11:![0-9]+]]
// CHECK-NEXT:    [[SUB_PTR_DIV:%.*]] = ashr exact i64 [[SUB_PTR_SUB]], 2, !annotation [[META3]]
// CHECK-NEXT:    [[CMP51:%.*]] = icmp sge i64 [[SUB_PTR_DIV]], [[CONV]], !annotation [[META3]]
// CHECK-NEXT:    [[CMP54:%.*]] = icmp sgt i32 [[LEN]], -1, !annotation [[META3]]
// CHECK-NEXT:    [[SPEC_SELECT:%.*]] = and i1 [[CMP54]], [[CMP51]]
// CHECK-NEXT:    br i1 [[SPEC_SELECT]], label [[CONT]], label [[TRAP]], !annotation [[META3]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6]], !annotation [[META3]]
// CHECK-NEXT:    unreachable, !annotation [[META3]]
// CHECK:       cont:
// CHECK-NEXT:    tail call void @foo(ptr noundef [[AGG_TEMP_SROA_0_0_COPYLOAD]], i32 noundef [[LEN]]) #[[ATTR5]]
// CHECK-NEXT:    ret void
//
void caller_7(int *__bidi_indexable p, int len) {
  foo(p, len);
}

// CHECK-LABEL: define dso_local void @caller_8(
// CHECK-SAME: ptr noundef [[P:%.*]], i32 noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TOBOOL_NOT:%.*]] = icmp eq ptr [[P]], null, !annotation [[META3]]
// CHECK-NEXT:    [[SPEC_SELECT:%.*]] = icmp ult i32 [[LEN]], 2
// CHECK-NEXT:    [[OR_COND:%.*]] = or i1 [[TOBOOL_NOT]], [[SPEC_SELECT]], !annotation [[META3]]
// CHECK-NEXT:    br i1 [[OR_COND]], label [[CONT:%.*]], label [[TRAP:%.*]], !annotation [[META3]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR6]], !annotation [[META3]]
// CHECK-NEXT:    unreachable, !annotation [[META3]]
// CHECK:       cont:
// CHECK-NEXT:    tail call void @foo(ptr noundef [[P]], i32 noundef [[LEN]]) #[[ATTR5]]
// CHECK-NEXT:    ret void
//
void caller_8(int *__single p, int len) {
  foo(p, len);
}

void bar(int *__counted_by(*len) *out, int *len);

// CHECK-LABEL: define dso_local void @caller_9(
// CHECK-SAME: ptr noundef [[OUT:%.*]], ptr noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @bar(ptr noundef [[OUT]], ptr noundef [[LEN]]) #[[ATTR5]]
// CHECK-NEXT:    ret void
//
void caller_9(int *__counted_by(*len) *out, int *len){
    bar(out, len);
}

// CHECK-LABEL: define dso_local ptr @caller_10(
// CHECK-SAME: i32 noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[COUNT:%.*]] = alloca i32, align 4
// CHECK-NEXT:    [[P:%.*]] = alloca ptr, align 8
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 4, ptr nonnull [[COUNT]]) #[[ATTR5]]
// CHECK-NEXT:    store i32 0, ptr [[COUNT]], align 4, !annotation [[META13:![0-9]+]]
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 8, ptr nonnull [[P]]) #[[ATTR5]]
// CHECK-NEXT:    store ptr null, ptr [[P]], align 8, !annotation [[META13]]
// CHECK-NEXT:    call void @bar(ptr noundef nonnull [[P]], ptr noundef nonnull [[COUNT]]) #[[ATTR5]]
// CHECK-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[P]], align 8, !tbaa [[TBAA5]]
// CHECK-NEXT:    [[DOTNOT:%.*]] = icmp ne ptr [[TMP0]], null, !annotation [[META2]]
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[COUNT]], align 4
// CHECK-NEXT:    [[CMP_NOT83:%.*]] = icmp slt i32 [[TMP1]], 0, !annotation [[META3]]
// CHECK-NEXT:    [[CMP_NOT:%.*]] = select i1 [[DOTNOT]], i1 [[CMP_NOT83]], i1 false, !annotation [[META3]]
// CHECK-NEXT:    br i1 [[CMP_NOT]], label [[TRAP:%.*]], label [[LAND_RHS:%.*]], !annotation [[META3]]
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR6]], !annotation [[META14:![0-9]+]]
// CHECK-NEXT:    unreachable, !annotation [[META14]]
// CHECK:       land.rhs:
// CHECK-NEXT:    br i1 [[DOTNOT]], label [[LOR_RHS:%.*]], label [[CONT60:%.*]], !annotation [[META3]]
// CHECK:       lor.rhs:
// CHECK-NEXT:    [[CMP54:%.*]] = icmp sge i32 [[TMP1]], [[LEN]], !annotation [[META3]]
// CHECK-NEXT:    [[CMP57:%.*]] = icmp sgt i32 [[LEN]], -1, !annotation [[META3]]
// CHECK-NEXT:    [[SPEC_SELECT:%.*]] = and i1 [[CMP57]], [[CMP54]]
// CHECK-NEXT:    br i1 [[SPEC_SELECT]], label [[CONT60]], label [[TRAP]], !annotation [[META3]]
// CHECK:       cont60:
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 8, ptr nonnull [[P]]) #[[ATTR5]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 4, ptr nonnull [[COUNT]]) #[[ATTR5]]
// CHECK-NEXT:    ret ptr [[TMP0]]
//
int *__counted_by_or_null(len) caller_10(int len) {
    int count;
    int *__counted_by_or_null(count) p;
    bar(&p, &count);
    p = p; // workaround for missing return bounds check
    count = len;
    return p;
}

//.
// CHECK: [[META2]] = !{!"bounds-safety-check-ptr-neq-null"}
// CHECK: [[META3]] = !{!"bounds-safety-generic"}
// CHECK: [[TBAA5]] = !{[[META6:![0-9]+]], [[META6]], i64 0}
// CHECK: [[META6]] = !{!"any pointer", [[META7:![0-9]+]], i64 0}
// CHECK: [[META7]] = !{!"omnipotent char", [[META8:![0-9]+]], i64 0}
// CHECK: [[META8]] = !{!"Simple C/C++ TBAA"}
// CHECK: [[META11]] = !{!"bounds-safety-generic", [[META12:![0-9]+]]}
// CHECK: [[META12]] = !{!"bounds-safety-missed-optimization-nsw", !"Check can not be removed because the arithmetic operation might wrap in the signed sense. Optimize the check by adding conditions to check for overflow before doing the operation"}
// CHECK: [[META13]] = !{!"bounds-safety-zero-init"}
// CHECK: [[META14]] = !{!"bounds-safety-check-ptr-lt-upper-bound", !"bounds-safety-check-ptr-ge-lower-bound", !"bounds-safety-generic"}
//.
