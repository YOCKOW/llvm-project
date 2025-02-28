// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!llvm.loop ![0-9]+" "#[0-9]+"
//
// RUN: %clang_cc1 -O2 -fbounds-safety -emit-llvm -triple x86_64 %s -o - | FileCheck %s

// XFAIL: !rdar109424213

#include <ptrcheck.h>
#include <stdint.h>

int a[4];
int b[4];
int c[4];
int d[4];

// CHECK-LABEL: @read_from_global_array_can_remove_checks(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[TMP0:%.*]] = load i32, ptr @a, align 16, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP1:%.*]] = load i32, ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 1), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[ADD_1:%.*]] = add nsw i32 [[TMP1]], [[TMP0]]
// CHECK-NEXT:    [[TMP2:%.*]] = load i32, ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 2), align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[ADD_2:%.*]] = add nsw i32 [[TMP2]], [[ADD_1]]
// CHECK-NEXT:    [[TMP3:%.*]] = load i32, ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 3), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[ADD_3:%.*]] = add nsw i32 [[TMP3]], [[ADD_2]]
// CHECK-NEXT:    ret i32 [[ADD_3]]
//
int read_from_global_array_can_remove_checks() {
    int res = 0;
  for (unsigned char i = 0; i < 4; i++) {
    res += a[i];
  }
  return res;
}

// CHECK-LABEL: @read_from_global_array_trap_last_iter(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) {{#[0-9]+}}, {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
//
int read_from_global_array_trap_last_iter() {
    int res = 0;
  for (unsigned char i = 0; i < 5; i++) {
    res += a[i];
  }
  return res;
}

// CHECK-LABEL: @read_from_global_array_cannot_rename(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[CMP7_NOT:%.*]] = icmp eq i32 [[N:%.*]], 0
// CHECK-NEXT:    br i1 [[CMP7_NOT]], label [[FOR_COND_CLEANUP:%.*]], label [[FOR_BODY:%.*]]
// CHECK:       for.cond.cleanup:
// CHECK-NEXT:    [[RES_0_LCSSA:%.*]] = phi i32 [ 0, [[ENTRY:%.*]] ], [ [[ADD:%.*]], [[CONT2:%.*]] ]
// CHECK-NEXT:    ret i32 [[RES_0_LCSSA]]
// CHECK:       for.body:
// CHECK-NEXT:    [[RES_09:%.*]] = phi i32 [ [[ADD]], [[CONT2]] ], [ 0, [[ENTRY]] ]
// CHECK-NEXT:    [[I_08:%.*]] = phi i8 [ [[INC:%.*]], [[CONT2]] ], [ 0, [[ENTRY]] ]
// CHECK-NEXT:    [[IDXPROM:%.*]] = zext i8 [[I_08]] to i64
// CHECK-NEXT:    [[TMP0:%.*]] = getelementptr i32, ptr @a, i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP1:%.*]] = icmp ult ptr [[TMP0]], getelementptr inbounds ([4 x i32], ptr @a, i64 1, i64 0), {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP2:%.*]] = icmp uge ptr [[TMP0]], @a, {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND:%.*]] = and i1 [[TMP1]], [[TMP2]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND]], label [[CONT2]], label [[TRAP:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) {{#[0-9]+}}, {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont2:
// CHECK-NEXT:    [[TMP3:%.*]] = load i32, ptr [[TMP0]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[ADD]] = add nsw i32 [[TMP3]], [[RES_09]]
// CHECK-NEXT:    [[INC]] = add nuw nsw i8 [[I_08]], 1
// CHECK-NEXT:    [[CONV:%.*]] = zext i8 [[INC]] to i32
// CHECK-NEXT:    [[CMP:%.*]] = icmp ult i32 [[CONV]], [[N]]
// CHECK-NEXT:    br i1 [[CMP]], label [[FOR_BODY]], label [[FOR_COND_CLEANUP]], {{!llvm.loop ![0-9]+}}
//
int read_from_global_array_cannot_rename(unsigned n) {
    int res = 0;
  for (unsigned char i = 0; i < n; i++) {
    res += a[i];
  }
  return res;
}

typedef struct {
  uint16_t len;
  uint16_t offset;
  uint8_t payload[__counted_by(offset + len)];
} hdr_t;

// CHECK-LABEL: @concat_to_separate_clobals(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[PAYLOAD:%.*]] = getelementptr inbounds [[STRUCT_HDR_T:%.*]], ptr [[P_BUF:%.*]], i64 0, i32 2
// CHECK-NEXT:    [[OFFSET:%.*]] = getelementptr inbounds [[STRUCT_HDR_T]], ptr [[P_BUF]], i64 0, i32 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i16, ptr [[OFFSET]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV:%.*]] = zext i16 [[TMP0]] to i64
// CHECK-NEXT:    [[TMP1:%.*]] = load i16, ptr [[P_BUF]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV3:%.*]] = zext i16 [[TMP1]] to i64
// CHECK-NEXT:    [[ADD:%.*]] = add nuw nsw i64 [[CONV3]], [[CONV]]
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[PAYLOAD]], i64 [[ADD]]
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 7
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[ADD_PTR]] to i64
// CHECK-NEXT:    [[CMP14_NOT:%.*]] = icmp ule ptr [[BOUND_PTR_ARITH]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[BOUND_PTR_ARITH]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]]
// CHECK-NEXT:    [[CMP38:%.*]] = icmp sgt i64 [[SUB_PTR_SUB]], 3
// CHECK-NEXT:    [[OR_COND156:%.*]] = select i1 [[CMP14_NOT]], i1 [[CMP38]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND156]], label [[CONT:%.*]], label [[TRAP:%.*]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) {{#[0-9]+}}, {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont:
// CHECK-NEXT:    [[ADD_PTR50:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 11
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 10
// CHECK-NEXT:    [[TMP3:%.*]] = load i8, ptr [[TMP2]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59:%.*]] = zext i8 [[TMP3]] to i32
// CHECK-NEXT:    store i32 [[CONV59]], ptr @a, align 16, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 8
// CHECK-NEXT:    [[TMP5:%.*]] = load i8, ptr [[TMP4]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV79:%.*]] = zext i8 [[TMP5]] to i32
// CHECK-NEXT:    store i32 [[CONV79]], ptr @b, align 16, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP6:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 9
// CHECK-NEXT:    [[TMP7:%.*]] = load i8, ptr [[TMP6]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV100:%.*]] = zext i8 [[TMP7]] to i32
// CHECK-NEXT:    store i32 [[CONV100]], ptr @c, align 16, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP8:%.*]] = load i8, ptr [[BOUND_PTR_ARITH]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV121:%.*]] = zext i8 [[TMP8]] to i32
// CHECK-NEXT:    store i32 [[CONV121]], ptr @d, align 16, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_1:%.*]] = icmp ule ptr [[ADD_PTR50]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_1:%.*]] = ptrtoint ptr [[ADD_PTR50]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_1:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_1]]
// CHECK-NEXT:    [[CMP38_1:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_1]], 3
// CHECK-NEXT:    [[OR_COND156_1:%.*]] = select i1 [[CMP14_NOT_1]], i1 [[CMP38_1]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND156_1]], label [[CONT_1:%.*]], label [[TRAP]]
// CHECK:       cont.1:
// CHECK-NEXT:    [[ADD_PTR50_1:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 15
// CHECK-NEXT:    [[TMP9:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 14
// CHECK-NEXT:    [[TMP10:%.*]] = load i8, ptr [[TMP9]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_1:%.*]] = zext i8 [[TMP10]] to i32
// CHECK-NEXT:    store i32 [[CONV59_1]], ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 1), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP11:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 12
// CHECK-NEXT:    [[TMP12:%.*]] = load i8, ptr [[TMP11]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV79_1:%.*]] = zext i8 [[TMP12]] to i32
// CHECK-NEXT:    store i32 [[CONV79_1]], ptr getelementptr inbounds ([4 x i32], ptr @b, i64 0, i64 1), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP13:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 13
// CHECK-NEXT:    [[TMP14:%.*]] = load i8, ptr [[TMP13]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV100_1:%.*]] = zext i8 [[TMP14]] to i32
// CHECK-NEXT:    store i32 [[CONV100_1]], ptr getelementptr inbounds ([4 x i32], ptr @c, i64 0, i64 1), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP15:%.*]] = load i8, ptr [[ADD_PTR50]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV121_1:%.*]] = zext i8 [[TMP15]] to i32
// CHECK-NEXT:    store i32 [[CONV121_1]], ptr getelementptr inbounds ([4 x i32], ptr @d, i64 0, i64 1), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_2:%.*]] = icmp ule ptr [[ADD_PTR50_1]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_2:%.*]] = ptrtoint ptr [[ADD_PTR50_1]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_2:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_2]]
// CHECK-NEXT:    [[CMP38_2:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_2]], 3
// CHECK-NEXT:    [[OR_COND156_2:%.*]] = select i1 [[CMP14_NOT_2]], i1 [[CMP38_2]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND156_2]], label [[CONT_2:%.*]], label [[TRAP]]
// CHECK:       cont.2:
// CHECK-NEXT:    [[ADD_PTR50_2:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 19
// CHECK-NEXT:    [[TMP16:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 18
// CHECK-NEXT:    [[TMP17:%.*]] = load i8, ptr [[TMP16]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_2:%.*]] = zext i8 [[TMP17]] to i32
// CHECK-NEXT:    store i32 [[CONV59_2]], ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 2), align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP18:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 16
// CHECK-NEXT:    [[TMP19:%.*]] = load i8, ptr [[TMP18]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV79_2:%.*]] = zext i8 [[TMP19]] to i32
// CHECK-NEXT:    store i32 [[CONV79_2]], ptr getelementptr inbounds ([4 x i32], ptr @b, i64 0, i64 2), align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP20:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 17
// CHECK-NEXT:    [[TMP21:%.*]] = load i8, ptr [[TMP20]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV100_2:%.*]] = zext i8 [[TMP21]] to i32
// CHECK-NEXT:    store i32 [[CONV100_2]], ptr getelementptr inbounds ([4 x i32], ptr @c, i64 0, i64 2), align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP22:%.*]] = load i8, ptr [[ADD_PTR50_1]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV121_2:%.*]] = zext i8 [[TMP22]] to i32
// CHECK-NEXT:    store i32 [[CONV121_2]], ptr getelementptr inbounds ([4 x i32], ptr @d, i64 0, i64 2), align 8, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_3:%.*]] = icmp ule ptr [[ADD_PTR50_2]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_3:%.*]] = ptrtoint ptr [[ADD_PTR50_2]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_3:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_3]]
// CHECK-NEXT:    [[CMP38_3:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_3]], 3
// CHECK-NEXT:    [[OR_COND156_3:%.*]] = select i1 [[CMP14_NOT_3]], i1 [[CMP38_3]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND156_3]], label [[CONT_3:%.*]], label [[TRAP]]
// CHECK:       cont.3:
// CHECK-NEXT:    [[TMP23:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 22
// CHECK-NEXT:    [[TMP24:%.*]] = load i8, ptr [[TMP23]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_3:%.*]] = zext i8 [[TMP24]] to i32
// CHECK-NEXT:    store i32 [[CONV59_3]], ptr getelementptr inbounds ([4 x i32], ptr @a, i64 0, i64 3), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP25:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 20
// CHECK-NEXT:    [[TMP26:%.*]] = load i8, ptr [[TMP25]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV79_3:%.*]] = zext i8 [[TMP26]] to i32
// CHECK-NEXT:    store i32 [[CONV79_3]], ptr getelementptr inbounds ([4 x i32], ptr @b, i64 0, i64 3), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP27:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 21
// CHECK-NEXT:    [[TMP28:%.*]] = load i8, ptr [[TMP27]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV100_3:%.*]] = zext i8 [[TMP28]] to i32
// CHECK-NEXT:    store i32 [[CONV100_3]], ptr getelementptr inbounds ([4 x i32], ptr @c, i64 0, i64 3), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP29:%.*]] = load i8, ptr [[ADD_PTR50_2]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV121_3:%.*]] = zext i8 [[TMP29]] to i32
// CHECK-NEXT:    store i32 [[CONV121_3]], ptr getelementptr inbounds ([4 x i32], ptr @d, i64 0, i64 3), align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    ret void
//
void concat_to_separate_clobals(hdr_t *p_buf) {
  uint8_t *params = p_buf->payload + 3;
  for (unsigned char i = 0; i < 4; i++) {
    uint8_t *__counted_by(4) p = params;
    a[i] = p[3]; // checks not removed for p[]
    b[i] = p[1]; // checks not removed for b[] and p[]
    c[i] = p[2]; // checks not removed for c[] and p[]
    d[i] = p[0]; // checks not removed for d[]
    params += 4;
  }
}

struct arrays {
  int a[4];
  int b[4];
  int c[4];
  int d[4];
};

// CHECK-LABEL: @concat_to_arrays_struct_can_remove_arrays_check(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[PAYLOAD:%.*]] = getelementptr inbounds [[STRUCT_HDR_T:%.*]], ptr [[P_BUF:%.*]], i64 0, i32 2
// CHECK-NEXT:    [[OFFSET:%.*]] = getelementptr inbounds [[STRUCT_HDR_T]], ptr [[P_BUF]], i64 0, i32 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i16, ptr [[OFFSET]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV:%.*]] = zext i16 [[TMP0]] to i64
// CHECK-NEXT:    [[TMP1:%.*]] = load i16, ptr [[P_BUF]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV3:%.*]] = zext i16 [[TMP1]] to i64
// CHECK-NEXT:    [[ADD:%.*]] = add nuw nsw i64 [[CONV3]], [[CONV]]
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[PAYLOAD]], i64 [[ADD]]
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 7
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[ADD_PTR]] to i64
// CHECK-NEXT:    [[CMP14_NOT:%.*]] = icmp ule ptr [[BOUND_PTR_ARITH]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[BOUND_PTR_ARITH]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]]
// CHECK-NEXT:    [[CMP38:%.*]] = icmp sgt i64 [[SUB_PTR_SUB]], 3
// CHECK-NEXT:    [[OR_COND166:%.*]] = select i1 [[CMP14_NOT]], i1 [[CMP38]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166]], label [[CONT138:%.*]], label [[TRAP:%.*]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) {{#[0-9]+}}, {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont138:
// CHECK-NEXT:    [[UPPER106:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS:%.*]], ptr [[ARRAYS:%.*]], i64 0, i32 3
// CHECK-NEXT:    [[UPPER83:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2
// CHECK-NEXT:    [[B:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 1
// CHECK-NEXT:    [[ADD_PTR50:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 11
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 10
// CHECK-NEXT:    [[TMP3:%.*]] = load i8, ptr [[TMP2]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59:%.*]] = zext i8 [[TMP3]] to i32
// CHECK-NEXT:    store i32 [[CONV59]], ptr [[ARRAYS]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 8
// CHECK-NEXT:    [[TMP5:%.*]] = load i8, ptr [[TMP4]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80:%.*]] = zext i8 [[TMP5]] to i32
// CHECK-NEXT:    store i32 [[CONV80]], ptr [[B]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP6:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 9
// CHECK-NEXT:    [[TMP7:%.*]] = load i8, ptr [[TMP6]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103:%.*]] = zext i8 [[TMP7]] to i32
// CHECK-NEXT:    store i32 [[CONV103]], ptr [[UPPER83]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP8:%.*]] = load i8, ptr [[BOUND_PTR_ARITH]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126:%.*]] = zext i8 [[TMP8]] to i32
// CHECK-NEXT:    store i32 [[CONV126]], ptr [[UPPER106]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_1:%.*]] = icmp ule ptr [[ADD_PTR50]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_1:%.*]] = ptrtoint ptr [[ADD_PTR50]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_1:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_1]]
// CHECK-NEXT:    [[CMP38_1:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_1]], 3
// CHECK-NEXT:    [[OR_COND166_1:%.*]] = select i1 [[CMP14_NOT_1]], i1 [[CMP38_1]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166_1]], label [[CONT138_1:%.*]], label [[TRAP]]
// CHECK:       cont138.1:
// CHECK-NEXT:    [[ADD_PTR50_1:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 15
// CHECK-NEXT:    [[TMP9:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 14
// CHECK-NEXT:    [[TMP10:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 1
// CHECK-NEXT:    [[TMP11:%.*]] = load i8, ptr [[TMP9]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_1:%.*]] = zext i8 [[TMP11]] to i32
// CHECK-NEXT:    store i32 [[CONV59_1]], ptr [[TMP10]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP12:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 12
// CHECK-NEXT:    [[TMP13:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 1, i64 1
// CHECK-NEXT:    [[TMP14:%.*]] = load i8, ptr [[TMP12]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80_1:%.*]] = zext i8 [[TMP14]] to i32
// CHECK-NEXT:    store i32 [[CONV80_1]], ptr [[TMP13]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP15:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2, i64 1
// CHECK-NEXT:    [[TMP16:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 13
// CHECK-NEXT:    [[TMP17:%.*]] = load i8, ptr [[TMP16]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103_1:%.*]] = zext i8 [[TMP17]] to i32
// CHECK-NEXT:    store i32 [[CONV103_1]], ptr [[TMP15]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP18:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3, i64 1
// CHECK-NEXT:    [[TMP19:%.*]] = load i8, ptr [[ADD_PTR50]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126_1:%.*]] = zext i8 [[TMP19]] to i32
// CHECK-NEXT:    store i32 [[CONV126_1]], ptr [[TMP18]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_2:%.*]] = icmp ule ptr [[ADD_PTR50_1]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_2:%.*]] = ptrtoint ptr [[ADD_PTR50_1]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_2:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_2]]
// CHECK-NEXT:    [[CMP38_2:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_2]], 3
// CHECK-NEXT:    [[OR_COND166_2:%.*]] = select i1 [[CMP14_NOT_2]], i1 [[CMP38_2]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166_2]], label [[CONT138_2:%.*]], label [[TRAP]]
// CHECK:       cont138.2:
// CHECK-NEXT:    [[ADD_PTR50_2:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 19
// CHECK-NEXT:    [[TMP20:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 18
// CHECK-NEXT:    [[TMP21:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 2
// CHECK-NEXT:    [[TMP22:%.*]] = load i8, ptr [[TMP20]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_2:%.*]] = zext i8 [[TMP22]] to i32
// CHECK-NEXT:    store i32 [[CONV59_2]], ptr [[TMP21]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP23:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 16
// CHECK-NEXT:    [[TMP24:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 1, i64 2
// CHECK-NEXT:    [[TMP25:%.*]] = load i8, ptr [[TMP23]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80_2:%.*]] = zext i8 [[TMP25]] to i32
// CHECK-NEXT:    store i32 [[CONV80_2]], ptr [[TMP24]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP26:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2, i64 2
// CHECK-NEXT:    [[TMP27:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 17
// CHECK-NEXT:    [[TMP28:%.*]] = load i8, ptr [[TMP27]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103_2:%.*]] = zext i8 [[TMP28]] to i32
// CHECK-NEXT:    store i32 [[CONV103_2]], ptr [[TMP26]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP29:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3, i64 2
// CHECK-NEXT:    [[TMP30:%.*]] = load i8, ptr [[ADD_PTR50_1]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126_2:%.*]] = zext i8 [[TMP30]] to i32
// CHECK-NEXT:    store i32 [[CONV126_2]], ptr [[TMP29]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_3:%.*]] = icmp ule ptr [[ADD_PTR50_2]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_3:%.*]] = ptrtoint ptr [[ADD_PTR50_2]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_3:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_3]]
// CHECK-NEXT:    [[CMP38_3:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_3]], 3
// CHECK-NEXT:    [[OR_COND166_3:%.*]] = select i1 [[CMP14_NOT_3]], i1 [[CMP38_3]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166_3]], label [[CONT138_3:%.*]], label [[TRAP]]
// CHECK:       cont138.3:
// CHECK-NEXT:    [[TMP31:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 22
// CHECK-NEXT:    [[TMP32:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 3
// CHECK-NEXT:    [[TMP33:%.*]] = load i8, ptr [[TMP31]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_3:%.*]] = zext i8 [[TMP33]] to i32
// CHECK-NEXT:    store i32 [[CONV59_3]], ptr [[TMP32]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP34:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 20
// CHECK-NEXT:    [[TMP35:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 1, i64 3
// CHECK-NEXT:    [[TMP36:%.*]] = load i8, ptr [[TMP34]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80_3:%.*]] = zext i8 [[TMP36]] to i32
// CHECK-NEXT:    store i32 [[CONV80_3]], ptr [[TMP35]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP37:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2, i64 3
// CHECK-NEXT:    [[TMP38:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 21
// CHECK-NEXT:    [[TMP39:%.*]] = load i8, ptr [[TMP38]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103_3:%.*]] = zext i8 [[TMP39]] to i32
// CHECK-NEXT:    store i32 [[CONV103_3]], ptr [[TMP37]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP40:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3, i64 3
// CHECK-NEXT:    [[TMP41:%.*]] = load i8, ptr [[ADD_PTR50_2]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126_3:%.*]] = zext i8 [[TMP41]] to i32
// CHECK-NEXT:    store i32 [[CONV126_3]], ptr [[TMP40]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    ret void
//
void concat_to_arrays_struct_can_remove_arrays_check(struct arrays *arrays, hdr_t *p_buf) {
  uint8_t *params = p_buf->payload + 3;

  for (unsigned char i = 0; i < 4; i++) {
    uint8_t *__counted_by(4) p = params;
    arrays->a[i] = p[3]; // checks not removed for p[]
    arrays->b[i] = p[1]; // checks not removed for b[] and p[]
    arrays->c[i] = p[2]; // checks not removed for c[] and p[]
    arrays->d[i] = p[0]; // checks not removed for d[]
    params += 4;
  }
}

// CHECK-LABEL: @concat_to_arrays_struct_trap_on_last_iter(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[PAYLOAD:%.*]] = getelementptr inbounds [[STRUCT_HDR_T:%.*]], ptr [[P_BUF:%.*]], i64 0, i32 2
// CHECK-NEXT:    [[OFFSET:%.*]] = getelementptr inbounds [[STRUCT_HDR_T]], ptr [[P_BUF]], i64 0, i32 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i16, ptr [[OFFSET]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV:%.*]] = zext i16 [[TMP0]] to i64
// CHECK-NEXT:    [[TMP1:%.*]] = load i16, ptr [[P_BUF]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV3:%.*]] = zext i16 [[TMP1]] to i64
// CHECK-NEXT:    [[ADD:%.*]] = add nuw nsw i64 [[CONV3]], [[CONV]]
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[PAYLOAD]], i64 [[ADD]]
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 7
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[ADD_PTR]] to i64
// CHECK-NEXT:    [[CMP14_NOT:%.*]] = icmp ule ptr [[BOUND_PTR_ARITH]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[BOUND_PTR_ARITH]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]]
// CHECK-NEXT:    [[CMP38:%.*]] = icmp sgt i64 [[SUB_PTR_SUB]], 3
// CHECK-NEXT:    [[OR_COND166:%.*]] = select i1 [[CMP14_NOT]], i1 [[CMP38]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166]], label [[CONT138:%.*]], label [[TRAP:%.*]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) {{#[0-9]+}}, {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont138:
// CHECK-NEXT:    [[UPPER106:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS:%.*]], ptr [[ARRAYS:%.*]], i64 0, i32 3
// CHECK-NEXT:    [[UPPER83:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2
// CHECK-NEXT:    [[UPPER:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 4
// CHECK-NEXT:    [[ADD_PTR50:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 11
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 10
// CHECK-NEXT:    [[TMP3:%.*]] = load i8, ptr [[TMP2]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59:%.*]] = zext i8 [[TMP3]] to i32
// CHECK-NEXT:    store i32 [[CONV59]], ptr [[ARRAYS]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 8
// CHECK-NEXT:    [[TMP5:%.*]] = load i8, ptr [[TMP4]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80:%.*]] = zext i8 [[TMP5]] to i32
// CHECK-NEXT:    store i32 [[CONV80]], ptr [[UPPER]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP6:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 9
// CHECK-NEXT:    [[TMP7:%.*]] = load i8, ptr [[TMP6]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103:%.*]] = zext i8 [[TMP7]] to i32
// CHECK-NEXT:    store i32 [[CONV103]], ptr [[UPPER83]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP8:%.*]] = load i8, ptr [[BOUND_PTR_ARITH]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126:%.*]] = zext i8 [[TMP8]] to i32
// CHECK-NEXT:    store i32 [[CONV126]], ptr [[UPPER106]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_1:%.*]] = icmp ule ptr [[ADD_PTR50]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_1:%.*]] = ptrtoint ptr [[ADD_PTR50]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_1:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_1]]
// CHECK-NEXT:    [[CMP38_1:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_1]], 3
// CHECK-NEXT:    [[OR_COND166_1:%.*]] = select i1 [[CMP14_NOT_1]], i1 [[CMP38_1]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166_1]], label [[CONT138_1:%.*]], label [[TRAP]]
// CHECK:       cont138.1:
// CHECK-NEXT:    [[ADD_PTR50_1:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 15
// CHECK-NEXT:    [[TMP9:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 1
// CHECK-NEXT:    [[TMP10:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 14
// CHECK-NEXT:    [[TMP11:%.*]] = load i8, ptr [[TMP10]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_1:%.*]] = zext i8 [[TMP11]] to i32
// CHECK-NEXT:    store i32 [[CONV59_1]], ptr [[TMP9]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP12:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 12
// CHECK-NEXT:    [[TMP13:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 5
// CHECK-NEXT:    [[TMP14:%.*]] = load i8, ptr [[TMP12]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80_1:%.*]] = zext i8 [[TMP14]] to i32
// CHECK-NEXT:    store i32 [[CONV80_1]], ptr [[TMP13]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP15:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2, i64 1
// CHECK-NEXT:    [[TMP16:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 13
// CHECK-NEXT:    [[TMP17:%.*]] = load i8, ptr [[TMP16]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103_1:%.*]] = zext i8 [[TMP17]] to i32
// CHECK-NEXT:    store i32 [[CONV103_1]], ptr [[TMP15]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP18:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3, i64 1
// CHECK-NEXT:    [[TMP19:%.*]] = load i8, ptr [[ADD_PTR50]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126_1:%.*]] = zext i8 [[TMP19]] to i32
// CHECK-NEXT:    store i32 [[CONV126_1]], ptr [[TMP18]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_2:%.*]] = icmp ule ptr [[ADD_PTR50_1]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_2:%.*]] = ptrtoint ptr [[ADD_PTR50_1]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_2:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_2]]
// CHECK-NEXT:    [[CMP38_2:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_2]], 3
// CHECK-NEXT:    [[OR_COND166_2:%.*]] = select i1 [[CMP14_NOT_2]], i1 [[CMP38_2]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166_2]], label [[CONT138_2:%.*]], label [[TRAP]]
// CHECK:       cont138.2:
// CHECK-NEXT:    [[ADD_PTR50_2:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 19
// CHECK-NEXT:    [[TMP20:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 2
// CHECK-NEXT:    [[TMP21:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 18
// CHECK-NEXT:    [[TMP22:%.*]] = load i8, ptr [[TMP21]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_2:%.*]] = zext i8 [[TMP22]] to i32
// CHECK-NEXT:    store i32 [[CONV59_2]], ptr [[TMP20]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP23:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 16
// CHECK-NEXT:    [[TMP24:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 6
// CHECK-NEXT:    [[TMP25:%.*]] = load i8, ptr [[TMP23]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80_2:%.*]] = zext i8 [[TMP25]] to i32
// CHECK-NEXT:    store i32 [[CONV80_2]], ptr [[TMP24]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP26:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2, i64 2
// CHECK-NEXT:    [[TMP27:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 17
// CHECK-NEXT:    [[TMP28:%.*]] = load i8, ptr [[TMP27]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103_2:%.*]] = zext i8 [[TMP28]] to i32
// CHECK-NEXT:    store i32 [[CONV103_2]], ptr [[TMP26]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP29:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3, i64 2
// CHECK-NEXT:    [[TMP30:%.*]] = load i8, ptr [[ADD_PTR50_1]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126_2:%.*]] = zext i8 [[TMP30]] to i32
// CHECK-NEXT:    store i32 [[CONV126_2]], ptr [[TMP29]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CMP14_NOT_3:%.*]] = icmp ule ptr [[ADD_PTR50_2]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST_3:%.*]] = ptrtoint ptr [[ADD_PTR50_2]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB_3:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST_3]]
// CHECK-NEXT:    [[CMP38_3:%.*]] = icmp sgt i64 [[SUB_PTR_SUB_3]], 3
// CHECK-NEXT:    [[OR_COND166_3:%.*]] = select i1 [[CMP14_NOT_3]], i1 [[CMP38_3]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166_3]], label [[CONT138_3:%.*]], label [[TRAP]]
// CHECK:       cont138.3:
// CHECK-NEXT:    [[TMP31:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 3
// CHECK-NEXT:    [[TMP32:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 22
// CHECK-NEXT:    [[TMP33:%.*]] = load i8, ptr [[TMP32]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59_3:%.*]] = zext i8 [[TMP33]] to i32
// CHECK-NEXT:    store i32 [[CONV59_3]], ptr [[TMP31]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP34:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 20
// CHECK-NEXT:    [[TMP35:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 7
// CHECK-NEXT:    [[TMP36:%.*]] = load i8, ptr [[TMP34]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80_3:%.*]] = zext i8 [[TMP36]] to i32
// CHECK-NEXT:    store i32 [[CONV80_3]], ptr [[TMP35]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP37:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 2, i64 3
// CHECK-NEXT:    [[TMP38:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 21
// CHECK-NEXT:    [[TMP39:%.*]] = load i8, ptr [[TMP38]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103_3:%.*]] = zext i8 [[TMP39]] to i32
// CHECK-NEXT:    store i32 [[CONV103_3]], ptr [[TMP37]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP40:%.*]] = getelementptr [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3, i64 3
// CHECK-NEXT:    [[TMP41:%.*]] = load i8, ptr [[ADD_PTR50_2]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126_3:%.*]] = zext i8 [[TMP41]] to i32
// CHECK-NEXT:    store i32 [[CONV126_3]], ptr [[TMP40]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    br label [[TRAP]]
//
void concat_to_arrays_struct_trap_on_last_iter(struct arrays *arrays, hdr_t *p_buf) {
  uint8_t *params = p_buf->payload + 3;

  for (unsigned char i = 0; i < 5; i++) {
    uint8_t *__counted_by(4) p = params;
    arrays->a[i] = p[3]; // checks not removed for p[]
    arrays->b[i] = p[1]; // checks not removed for b[] and p[]
    arrays->c[i] = p[2]; // checks not removed for c[] and p[]
    arrays->d[i] = p[0]; // checks not removed for d[]
    params += 4;
  }
}

// CHECK-LABEL: @concat_to_arrays_struct_cannot_remove_arrays_check(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[PAYLOAD:%.*]] = getelementptr [[STRUCT_HDR_T:%.*]], ptr [[P_BUF:%.*]], i64 0, i32 2
// CHECK-NEXT:    [[OFFSET:%.*]] = getelementptr inbounds [[STRUCT_HDR_T]], ptr [[P_BUF]], i64 0, i32 1
// CHECK-NEXT:    [[TMP0:%.*]] = load i16, ptr [[OFFSET]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV:%.*]] = zext i16 [[TMP0]] to i64
// CHECK-NEXT:    [[TMP1:%.*]] = load i16, ptr [[P_BUF]], align 2, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV3:%.*]] = zext i16 [[TMP1]] to i64
// CHECK-NEXT:    [[ADD:%.*]] = add nuw nsw i64 [[CONV3]], [[CONV]]
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[PAYLOAD]], i64 [[ADD]]
// CHECK-NEXT:    [[CMP168_NOT:%.*]] = icmp eq i32 [[N:%.*]], 0
// CHECK-NEXT:    br i1 [[CMP168_NOT]], label [[FOR_COND_CLEANUP:%.*]], label [[FOR_BODY_LR_PH:%.*]]
// CHECK:       for.body.lr.ph:
// CHECK-NEXT:    [[BOUND_PTR_ARITH:%.*]] = getelementptr i8, ptr [[P_BUF]], i64 7
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[ADD_PTR]] to i64
// CHECK-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i32, ptr [[ARRAYS:%.*]], i64 4
// CHECK-NEXT:    [[UPPER83:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS:%.*]], ptr [[ARRAYS]], i64 0, i32 2
// CHECK-NEXT:    [[UPPER106:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 0, i32 3
// CHECK-NEXT:    [[UPPER129:%.*]] = getelementptr inbounds [[STRUCT_ARRAYS]], ptr [[ARRAYS]], i64 1
// CHECK-NEXT:    br label [[FOR_BODY:%.*]]
// CHECK:       for.cond.cleanup:
// CHECK-NEXT:    ret void
// CHECK:       for.body:
// CHECK-NEXT:    [[PARAMS_SROA_0_0170:%.*]] = phi ptr [ [[BOUND_PTR_ARITH]], [[FOR_BODY_LR_PH]] ], [ [[ADD_PTR50:%.*]], [[CONT138:%.*]] ]
// CHECK-NEXT:    [[I_0169:%.*]] = phi i8 [ 0, [[FOR_BODY_LR_PH]] ], [ [[INC:%.*]], [[CONT138]] ]
// CHECK-NEXT:    [[CMP14_NOT:%.*]] = icmp ule ptr [[PARAMS_SROA_0_0170]], [[ADD_PTR]]
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[PARAMS_SROA_0_0170]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]]
// CHECK-NEXT:    [[CMP38:%.*]] = icmp sgt i64 [[SUB_PTR_SUB]], 3
// CHECK-NEXT:    [[OR_COND166:%.*]] = select i1 [[CMP14_NOT]], i1 [[CMP38]], i1 false, {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND166]], label [[CONT:%.*]], label [[TRAP:%.*]]
// CHECK:       trap:
// CHECK-NEXT:    tail call void @llvm.ubsantrap(i8 25) {{#[0-9]+}}, {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont:
// CHECK-NEXT:    [[ADD_PTR50]] = getelementptr inbounds i8, ptr [[PARAMS_SROA_0_0170]], i64 4
// CHECK-NEXT:    [[IDXPROM:%.*]] = zext i8 [[I_0169]] to i64
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr i32, ptr [[ARRAYS]], i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP3:%.*]] = icmp ult ptr [[TMP2]], [[UPPER]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP4:%.*]] = icmp uge ptr [[TMP2]], [[ARRAYS]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND141:%.*]] = and i1 [[TMP3]], [[TMP4]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND141]], label [[CONT69:%.*]], label [[TRAP]], {{!annotation ![0-9]+}}
// CHECK:       cont69:
// CHECK-NEXT:    [[TMP5:%.*]] = getelementptr i8, ptr [[PARAMS_SROA_0_0170]], i64 3
// CHECK-NEXT:    [[TMP6:%.*]] = load i8, ptr [[TMP5]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV59:%.*]] = zext i8 [[TMP6]] to i32
// CHECK-NEXT:    store i32 [[CONV59]], ptr [[TMP2]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP7:%.*]] = getelementptr i8, ptr [[PARAMS_SROA_0_0170]], i64 1
// CHECK-NEXT:    [[TMP8:%.*]] = getelementptr i32, ptr [[UPPER]], i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP9:%.*]] = load i8, ptr [[TMP7]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV80:%.*]] = zext i8 [[TMP9]] to i32
// CHECK-NEXT:    store i32 [[CONV80]], ptr [[TMP8]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP10:%.*]] = getelementptr i32, ptr [[UPPER83]], i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP11:%.*]] = icmp ult ptr [[TMP10]], [[UPPER106]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP12:%.*]] = icmp uge ptr [[TMP10]], [[UPPER83]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND145:%.*]] = and i1 [[TMP11]], [[TMP12]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND145]], label [[CONT115:%.*]], label [[TRAP]], {{!annotation ![0-9]+}}
// CHECK:       cont115:
// CHECK-NEXT:    [[TMP13:%.*]] = getelementptr i8, ptr [[PARAMS_SROA_0_0170]], i64 2
// CHECK-NEXT:    [[TMP14:%.*]] = load i8, ptr [[TMP13]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV103:%.*]] = zext i8 [[TMP14]] to i32
// CHECK-NEXT:    store i32 [[CONV103]], ptr [[TMP10]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[TMP15:%.*]] = getelementptr i32, ptr [[UPPER106]], i64 [[IDXPROM]]
// CHECK-NEXT:    [[TMP16:%.*]] = icmp ult ptr [[TMP15]], [[UPPER129]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[TMP17:%.*]] = icmp uge ptr [[TMP15]], [[UPPER106]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    [[OR_COND147:%.*]] = and i1 [[TMP16]], [[TMP17]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[OR_COND147]], label [[CONT138]], label [[TRAP]], {{!annotation ![0-9]+}}
// CHECK:       cont138:
// CHECK-NEXT:    [[TMP18:%.*]] = load i8, ptr [[PARAMS_SROA_0_0170]], align 1, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[CONV126:%.*]] = zext i8 [[TMP18]] to i32
// CHECK-NEXT:    store i32 [[CONV126]], ptr [[TMP15]], align 4, {{!tbaa ![0-9]+}}
// CHECK-NEXT:    [[INC]] = add i8 [[I_0169]], 1
// CHECK-NEXT:    [[CONV4:%.*]] = zext i8 [[INC]] to i32
// CHECK-NEXT:    [[CMP:%.*]] = icmp ult i32 [[CONV4]], [[N]]
// CHECK-NEXT:    br i1 [[CMP]], label [[FOR_BODY]], label [[FOR_COND_CLEANUP]], {{!llvm.loop ![0-9]+}}
//
void concat_to_arrays_struct_cannot_remove_arrays_check(struct arrays *arrays, hdr_t *p_buf, unsigned n) {
  uint8_t *params = p_buf->payload + 3;

  for (unsigned char i = 0; i < n; i++) {
    uint8_t *__counted_by(4) p = params;
    arrays->a[i] = p[3]; // checks not removed for p[]
    arrays->b[i] = p[1]; // checks not removed for b[] and p[]
    arrays->c[i] = p[2]; // checks not removed for c[] and p[]
    arrays->d[i] = p[0]; // checks not removed for d[]
    params += 4;
  }
}
