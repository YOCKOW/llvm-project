// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 4
// RUN: %clang_cc1 -O2 -triple arm64-apple-iphoneos -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O2 -triple arm64-apple-iphoneos -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>

struct Inner {
    int dummy;
    int len;
};

struct Outer {
    struct Inner hdr;
    char fam[__counted_by(hdr.len)];
};

// CHECK-LABEL: define dso_local i8 @access(
// CHECK-SAME: ptr noundef readonly [[BAR:%.*]], i32 noundef [[INDEX:%.*]]) local_unnamed_addr #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[FAM:%.*]] = getelementptr inbounds i8, ptr [[BAR]], i64 8
// CHECK-NEXT:    [[LEN:%.*]] = getelementptr inbounds i8, ptr [[BAR]], i64 4
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr [[LEN]], align 4, !tbaa [[TBAA2:![0-9]+]]
// CHECK-NEXT:    [[IDX_EXT:%.*]] = sext i32 [[TMP0]] to i64
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[FAM]], i64 [[IDX_EXT]]
// CHECK-NEXT:    [[IDXPROM:%.*]] = sext i32 [[INDEX]] to i64
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr i8, ptr [[FAM]], i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP1:%.*]] = icmp ult ptr [[ARRAYIDX]], [[ADD_PTR]], !annotation [[META7:![0-9]+]]
// CHECK-NEXT:    [[TMP2:%.*]] = icmp uge ptr [[ARRAYIDX]], [[FAM]], !annotation [[META8:![0-9]+]]
// CHECK-NEXT:    [[OR_COND:%.*]] = and i1 [[TMP2]], [[TMP1]], !annotation [[META8]]
// CHECK-NEXT:    br i1 [[OR_COND]], label [[CONT8:%.*]], label [[TRAP:%.*]], !annotation [[META7]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR2:[0-9]+]], !annotation [[META9:![0-9]+]]
// CHECK-NEXT:    unreachable, !annotation [[META9]]
// CHECK:       cont8:
// CHECK-NEXT:    [[TMP3:%.*]] = load i8, ptr [[ARRAYIDX]], align 1, !tbaa [[TBAA10:![0-9]+]]
// CHECK-NEXT:    ret i8 [[TMP3]]
//
char access(struct Outer *bar, int index) {
    return bar->fam[index];
}



// CHECK-LABEL: define dso_local ptr @assign(
// CHECK-SAME: ptr nocapture noundef readonly [[BAR:%.*]], i32 noundef [[LEN:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[AGG_TEMP1_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[BAR]], align 8
// CHECK-NEXT:    [[AGG_TEMP1_SROA_2_0_BAR_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[BAR]], i64 8
// CHECK-NEXT:    [[AGG_TEMP1_SROA_2_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP1_SROA_2_0_BAR_SROA_IDX]], align 8
// CHECK-NEXT:    [[AGG_TEMP1_SROA_3_0_BAR_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[BAR]], i64 16
// CHECK-NEXT:    [[AGG_TEMP1_SROA_3_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP1_SROA_3_0_BAR_SROA_IDX]], align 8, !tbaa [[TBAA11:![0-9]+]]
// CHECK-NEXT:    [[FLEX_BASE_NULL_CHECK_NOT:%.*]] = icmp eq ptr [[AGG_TEMP1_SROA_0_0_COPYLOAD]], null, !annotation [[META13:![0-9]+]]
// CHECK-NEXT:    br i1 [[FLEX_BASE_NULL_CHECK_NOT]], label [[CONT46:%.*]], label [[FLEX_BASE_NONNULL:%.*]], !annotation [[META13]]
// CHECK:       flex.base.nonnull:
// CHECK-NEXT:    [[TMP0:%.*]] = getelementptr i8, ptr [[AGG_TEMP1_SROA_0_0_COPYLOAD]], i64 8
// CHECK-NEXT:    [[DOTNOT:%.*]] = icmp ugt ptr [[AGG_TEMP1_SROA_0_0_COPYLOAD]], [[TMP0]], !annotation [[META14:![0-9]+]]
// CHECK-NEXT:    br i1 [[DOTNOT]], label [[TRAP:%.*]], label [[CONT:%.*]], !annotation [[META14]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR2]], !annotation [[META15:![0-9]+]]
// CHECK-NEXT:    unreachable, !annotation [[META15]]
// CHECK:       cont:
// CHECK-NEXT:    [[TMP1:%.*]] = icmp ule ptr [[TMP0]], [[AGG_TEMP1_SROA_2_0_COPYLOAD]], !annotation [[META7]]
// CHECK-NEXT:    [[TMP2:%.*]] = icmp ule ptr [[AGG_TEMP1_SROA_3_0_COPYLOAD]], [[AGG_TEMP1_SROA_0_0_COPYLOAD]], !annotation [[META8]]
// CHECK-NEXT:    [[OR_COND:%.*]] = select i1 [[TMP1]], i1 [[TMP2]], i1 false, !annotation [[META8]]
// CHECK-NEXT:    br i1 [[OR_COND]], label [[CONT18:%.*]], label [[TRAP]], !annotation [[META7]]
// CHECK:       cont18:
// CHECK-NEXT:    [[FLEX_COUNT_MINUS:%.*]] = icmp sgt i32 [[LEN]], -1, !annotation [[META16:![0-9]+]]
// CHECK-NEXT:    [[UPPER_INTPTR:%.*]] = ptrtoint ptr [[AGG_TEMP1_SROA_2_0_COPYLOAD]] to i64, !annotation [[META17:![0-9]+]]
// CHECK-NEXT:    [[FAM_INTPTR:%.*]] = ptrtoint ptr [[TMP0]] to i64, !annotation [[META17]]
// CHECK-NEXT:    [[FLEX_AVAIL_COUNT:%.*]] = sub nuw i64 [[UPPER_INTPTR]], [[FAM_INTPTR]], !annotation [[META17]]
// CHECK-NEXT:    [[FLEX_COUNT_INTPTR:%.*]] = zext nneg i32 [[LEN]] to i64, !annotation [[META17]]
// CHECK-NEXT:    [[FLEX_COUNT_CHECK:%.*]] = icmp uge i64 [[FLEX_AVAIL_COUNT]], [[FLEX_COUNT_INTPTR]], !annotation [[META17]]
// CHECK-NEXT:    [[OR_COND49:%.*]] = select i1 [[FLEX_COUNT_MINUS]], i1 [[FLEX_COUNT_CHECK]], i1 false, !annotation [[META17]]
// CHECK-NEXT:    [[TMP3:%.*]] = icmp ult ptr [[AGG_TEMP1_SROA_0_0_COPYLOAD]], [[AGG_TEMP1_SROA_2_0_COPYLOAD]], !annotation [[META7]]
// CHECK-NEXT:    [[OR_COND60:%.*]] = select i1 [[OR_COND49]], i1 [[TMP3]], i1 false, !annotation [[META7]]
// CHECK-NEXT:    br i1 [[OR_COND60]], label [[BOUNDSCHECK_CONT:%.*]], label [[TRAP]], !annotation [[META16]]
// CHECK:       boundscheck.cont:
// CHECK-NEXT:    [[LEN31:%.*]] = getelementptr inbounds i8, ptr [[AGG_TEMP1_SROA_0_0_COPYLOAD]], i64 4
// CHECK-NEXT:    br label [[CONT46]], !annotation [[META13]]
// CHECK:       cont46:
// CHECK-NEXT:    [[LEN31_SINK:%.*]] = phi ptr [ [[LEN31]], [[BOUNDSCHECK_CONT]] ], [ inttoptr (i64 4 to ptr), [[ENTRY:%.*]] ]
// CHECK-NEXT:    store i32 [[LEN]], ptr [[LEN31_SINK]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret ptr [[AGG_TEMP1_SROA_0_0_COPYLOAD]]
//
struct Outer * assign(void * __bidi_indexable bar, int len) {
    struct Outer * __single s = (struct Outer *) bar;
    s->hdr.len = len;
    return s;
}
//.
// CHECK: [[TBAA2]] = !{[[META3:![0-9]+]], [[META4:![0-9]+]], i64 4}
// CHECK: [[META3]] = !{!"Inner", [[META4]], i64 0, [[META4]], i64 4}
// CHECK: [[META4]] = !{!"int", [[META5:![0-9]+]], i64 0}
// CHECK: [[META5]] = !{!"omnipotent char", [[META6:![0-9]+]], i64 0}
// CHECK: [[META6]] = !{!"Simple C/C++ TBAA"}
// CHECK: [[META7]] = !{!"bounds-safety-check-ptr-lt-upper-bound"}
// CHECK: [[META8]] = !{!"bounds-safety-check-ptr-ge-lower-bound"}
// CHECK: [[META9]] = !{!"bounds-safety-check-ptr-lt-upper-bound", !"bounds-safety-check-ptr-ge-lower-bound"}
// CHECK: [[TBAA10]] = !{[[META5]], [[META5]], i64 0}
// CHECK: [[TBAA11]] = !{[[META12:![0-9]+]], [[META12]], i64 0}
// CHECK: [[META12]] = !{!"any pointer", [[META5]], i64 0}
// CHECK: [[META13]] = !{!"bounds-safety-check-ptr-neq-null"}
// CHECK: [[META14]] = !{!"bounds-safety-check-one-past-end-overflow"}
// CHECK: [[META15]] = !{!"bounds-safety-check-one-past-end-overflow", !"bounds-safety-check-ptr-lt-upper-bound", !"bounds-safety-check-ptr-ge-lower-bound", !"bounds-safety-check-count-negative", !"bounds-safety-check-ptr-le-upper-bound", !"bounds-safety-check-flexible-count-gt-bounds"}
// CHECK: [[META16]] = !{!"bounds-safety-check-count-negative"}
// CHECK: [[META17]] = !{!"bounds-safety-check-flexible-count-gt-bounds"}
//.
