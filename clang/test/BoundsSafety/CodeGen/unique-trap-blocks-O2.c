// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --include-generated-funcs --version 5

// Make sure that with or without `-fsplit-cold-code` that the trap blocks don't
// get outlined into their own function. Outlining trap blocks is unhelpful
// because it makes debugging harder which is the opposite of what
// `-funique-trap` is tries to do.

// RUN: %clang_cc1 -O2 -triple arm64e-apple-ios -fbounds-safety \
// RUN:   -funique-traps -emit-llvm %s -o - -fno-split-cold-code \
// RUN: | FileCheck %s
// RUN: %clang_cc1 -O2 -triple arm64e-apple-ios -fbounds-safety \
// RUN:   -funique-traps -emit-llvm %s -o - -fsplit-cold-code \
// RUN: | FileCheck %s
#include <ptrcheck.h>

__attribute__((always_inline)) int i_want_to_be_inlined(
  int* __bidi_indexable ptr, int idx) {
    return ptr[idx];
}

// Note: The trap counter is global to the module so even in the presence of
// inlining the counter will be unique.
int consume(int* __bidi_indexable ptr, int* __bidi_indexable ptr2, int idx) {
  int other = i_want_to_be_inlined(ptr2, idx);
  return ptr[idx];
}
// CHECK-LABEL: define i32 @i_want_to_be_inlined(
// CHECK-SAME: ptr nocapture noundef readonly [[PTR:%.*]], i32 noundef [[IDX:%.*]]) local_unnamed_addr #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[PTR]], align 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_2_0_PTR_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_2_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP_SROA_2_0_PTR_SROA_IDX]], align 8
// CHECK-NEXT:    [[IDXPROM:%.*]] = sext i32 [[IDX]] to i64
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr i32, ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP0:%.*]] = icmp ult ptr [[ARRAYIDX]], [[AGG_TEMP_SROA_2_0_COPYLOAD]], !annotation [[META2:![0-9]+]]
// CHECK-NEXT:    br i1 [[TMP0]], label %[[CONT:.*]], label %[[TRAP:.*]], !annotation [[META2]]
// CHECK:       [[TRAP]]:
// CHECK-NEXT:    tail call void asm sideeffect "", "n"(i64 0) #[[ATTR3:[0-9]+]], !annotation [[META2]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR4:[0-9]+]], !annotation [[META2]]
// CHECK-NEXT:    unreachable, !annotation [[META2]]
// CHECK:       [[CONT]]:
// CHECK-NEXT:    [[AGG_TEMP_SROA_3_0_PTR_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    [[AGG_TEMP_SROA_3_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP_SROA_3_0_PTR_SROA_IDX]], align 8, !tbaa [[TBAA3:![0-9]+]]
// CHECK-NEXT:    [[DOTNOT:%.*]] = icmp ult ptr [[ARRAYIDX]], [[AGG_TEMP_SROA_3_0_COPYLOAD]], !annotation [[META7:![0-9]+]]
// CHECK-NEXT:    br i1 [[DOTNOT]], label %[[TRAP1:.*]], label %[[CONT2:.*]], !annotation [[META7]]
// CHECK:       [[TRAP1]]:
// CHECK-NEXT:    tail call void asm sideeffect "", "n"(i64 1) #[[ATTR3]], !annotation [[META7]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR4]], !annotation [[META7]]
// CHECK-NEXT:    unreachable, !annotation [[META7]]
// CHECK:       [[CONT2]]:
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr [[ARRAYIDX]], align 4, !tbaa [[TBAA8:![0-9]+]]
// CHECK-NEXT:    ret i32 [[TMP1]]
//
//
// CHECK-LABEL: define i32 @consume(
// CHECK-SAME: ptr nocapture noundef readonly [[PTR:%.*]], ptr nocapture noundef readonly [[PTR2:%.*]], i32 noundef [[IDX:%.*]]) local_unnamed_addr #[[ATTR2:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[BYVAL_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[PTR2]], align 8
// CHECK-NEXT:    [[BYVAL_TEMP_SROA_4_0_PTR2_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR2]], i64 8
// CHECK-NEXT:    [[BYVAL_TEMP_SROA_4_0_COPYLOAD:%.*]] = load ptr, ptr [[BYVAL_TEMP_SROA_4_0_PTR2_SROA_IDX]], align 8
// CHECK-NEXT:    [[IDXPROM_I:%.*]] = sext i32 [[IDX]] to i64
// CHECK-NEXT:    [[ARRAYIDX_I:%.*]] = getelementptr i32, ptr [[BYVAL_TEMP_SROA_0_0_COPYLOAD]], i64 [[IDXPROM_I]]
// CHECK-NEXT:    [[TMP0:%.*]] = icmp ult ptr [[ARRAYIDX_I]], [[BYVAL_TEMP_SROA_4_0_COPYLOAD]], !annotation [[META2]]
// CHECK-NEXT:    br i1 [[TMP0]], label %[[CONT_I:.*]], label %[[TRAP_I:.*]], !annotation [[META2]]
// CHECK:       [[TRAP_I]]:
// CHECK-NEXT:    tail call void asm sideeffect "", "n"(i64 0) #[[ATTR3]], !annotation [[META2]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR4]], !annotation [[META2]]
// CHECK-NEXT:    unreachable, !annotation [[META2]]
// CHECK:       [[CONT_I]]:
// CHECK-NEXT:    [[BYVAL_TEMP_SROA_5_0_PTR2_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR2]], i64 16
// CHECK-NEXT:    [[BYVAL_TEMP_SROA_5_0_COPYLOAD:%.*]] = load ptr, ptr [[BYVAL_TEMP_SROA_5_0_PTR2_SROA_IDX]], align 8, !tbaa [[TBAA3]]
// CHECK-NEXT:    [[DOTNOT_I:%.*]] = icmp ult ptr [[ARRAYIDX_I]], [[BYVAL_TEMP_SROA_5_0_COPYLOAD]], !annotation [[META7]]
// CHECK-NEXT:    br i1 [[DOTNOT_I]], label %[[TRAP1_I:.*]], label %[[I_WANT_TO_BE_INLINED_EXIT:.*]], !annotation [[META7]]
// CHECK:       [[TRAP1_I]]:
// CHECK-NEXT:    tail call void asm sideeffect "", "n"(i64 1) #[[ATTR3]], !annotation [[META7]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR4]], !annotation [[META7]]
// CHECK-NEXT:    unreachable, !annotation [[META7]]
// CHECK:       [[I_WANT_TO_BE_INLINED_EXIT]]:
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[PTR]], align 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_2_0_PTR_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_2_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP_SROA_2_0_PTR_SROA_IDX]], align 8
// CHECK-NEXT:    [[ARRAYIDX:%.*]] = getelementptr i32, ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], i64 [[IDXPROM_I]]
// CHECK-NEXT:    [[TMP1:%.*]] = icmp ult ptr [[ARRAYIDX]], [[AGG_TEMP_SROA_2_0_COPYLOAD]], !annotation [[META2]]
// CHECK-NEXT:    br i1 [[TMP1]], label %[[CONT:.*]], label %[[TRAP:.*]], !annotation [[META2]]
// CHECK:       [[TRAP]]:
// CHECK-NEXT:    tail call void asm sideeffect "", "n"(i64 2) #[[ATTR3]], !annotation [[META2]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR4]], !annotation [[META2]]
// CHECK-NEXT:    unreachable, !annotation [[META2]]
// CHECK:       [[CONT]]:
// CHECK-NEXT:    [[AGG_TEMP_SROA_3_0_PTR_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    [[AGG_TEMP_SROA_3_0_COPYLOAD:%.*]] = load ptr, ptr [[AGG_TEMP_SROA_3_0_PTR_SROA_IDX]], align 8, !tbaa [[TBAA3]]
// CHECK-NEXT:    [[DOTNOT:%.*]] = icmp ult ptr [[ARRAYIDX]], [[AGG_TEMP_SROA_3_0_COPYLOAD]], !annotation [[META7]]
// CHECK-NEXT:    br i1 [[DOTNOT]], label %[[TRAP1:.*]], label %[[CONT2:.*]], !annotation [[META7]]
// CHECK:       [[TRAP1]]:
// CHECK-NEXT:    tail call void asm sideeffect "", "n"(i64 3) #[[ATTR3]], !annotation [[META7]]
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR4]], !annotation [[META7]]
// CHECK-NEXT:    unreachable, !annotation [[META7]]
// CHECK:       [[CONT2]]:
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, ptr [[ARRAYIDX]], align 4, !tbaa [[TBAA8]]
// CHECK-NEXT:    ret i32 [[TMP2]]
//
//.
// CHECK: [[META2]] = !{!"bounds-safety-check-ptr-lt-upper-bound"}
// CHECK: [[TBAA3]] = !{[[META4:![0-9]+]], [[META4]], i64 0}
// CHECK: [[META4]] = !{!"any pointer", [[META5:![0-9]+]], i64 0}
// CHECK: [[META5]] = !{!"omnipotent char", [[META6:![0-9]+]], i64 0}
// CHECK: [[META6]] = !{!"Simple C/C++ TBAA"}
// CHECK: [[META7]] = !{!"bounds-safety-check-ptr-ge-lower-bound"}
// CHECK: [[TBAA8]] = !{[[META9:![0-9]+]], [[META9]], i64 0}
// CHECK: [[META9]] = !{!"int", [[META5]], i64 0}
//.
