// REQUIRES: system-darwin
// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+" --prefix-filecheck-ir-name TMP_
// RUN: %clang_cc1 -O0 -triple x86_64 -fbounds-safety -emit-llvm %s -o - | FileCheck %s
// RUN: %clang_cc1 -O0 -triple x86_64 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck %s

#include <ptrcheck.h>


//

// CHECK-LABEL: @foo(
// CHECK-NEXT:  entry:
// CHECK-NEXT:    [[DST:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[SRC:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP1:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP2:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP3:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP10:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP17:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP26:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP35:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP43:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.2", align 8
// CHECK-NEXT:    [[AGG_TEMP44:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP59:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.2", align 8
// CHECK-NEXT:    [[AGG_TEMP60:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    [[AGG_TEMP74:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP81:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP92:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP101:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP110:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP111:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP126:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// CHECK-NEXT:    [[AGG_TEMP127:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP147:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.0", align 8
// CHECK-NEXT:    [[AGG_TEMP154:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable.1", align 8
// CHECK-NEXT:    call void @llvm.memset.p0.i64(ptr align 8 [[DST]], i8 0, i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memset.p0.i64(ptr align 8 [[SRC]], i8 0, i64 24, i1 false), {{!annotation ![0-9]+}}
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP1]], ptr align 8 [[DST]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP1]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR]], align 8
// CHECK-NEXT:    [[TMP0:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR]], ptr [[TMP0]], align 8
// CHECK-NEXT:    [[TMP1:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB]], ptr [[TMP1]], align 8
// CHECK-NEXT:    [[TMP2:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB]], ptr [[TMP2]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP3]], ptr align 8 [[SRC]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR4:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP3]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR5:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR4]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR6:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP3]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB7:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR6]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR8:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP3]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB9:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR8]], align 8
// CHECK-NEXT:    [[TMP3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP2]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR5]], ptr [[TMP3]], align 8
// CHECK-NEXT:    [[TMP4:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP2]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB7]], ptr [[TMP4]], align 8
// CHECK-NEXT:    [[TMP5:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP2]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB9]], ptr [[TMP5]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP10]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR11:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP10]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR12:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR11]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR13:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP10]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB14:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR13]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR15:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP10]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB16:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR15]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP17]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR18:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP17]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB19:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR18]], align 8
// CHECK-NEXT:    [[TMP6:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP17]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB19]], ptr [[TMP6]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR20:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP17]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR21:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR20]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR22:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP17]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB23:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR22]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR24:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP17]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB25:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR24]], align 8
// CHECK-NEXT:    [[CMP:%.*]] = icmp ule ptr [[WIDE_PTR_PTR12]], [[WIDE_PTR_PTR21]]
// CHECK-NEXT:    br i1 [[CMP]], label [[LAND_LHS_TRUE:%.*]], label [[LAND_END:%.*]]
// CHECK:       land.lhs.true:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP26]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR27:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP26]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB28:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR27]], align 8
// CHECK-NEXT:    [[TMP7:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP26]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB28]], ptr [[TMP7]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR29:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP26]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR30:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR29]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR31:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP26]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB32:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR31]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR33:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP26]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB34:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR33]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP35]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR36:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP35]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR37:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR36]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR38:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP35]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB39:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR38]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR40:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP35]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB41:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR40]], align 8
// CHECK-NEXT:    [[CMP42:%.*]] = icmp ule ptr [[WIDE_PTR_PTR30]], [[WIDE_PTR_PTR37]]
// CHECK-NEXT:    br i1 [[CMP42]], label [[LAND_RHS:%.*]], label [[LAND_END]]
// CHECK:       land.rhs:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP44]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR45:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP44]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB46:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR45]], align 8
// CHECK-NEXT:    [[TMP8:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP44]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB46]], ptr [[TMP8]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR47:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP44]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR48:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR47]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR49:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP44]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB50:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR49]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR51:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP44]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB52:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR51]], align 8
// CHECK-NEXT:    [[TMP9:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP43]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR48]], ptr [[TMP9]], align 8
// CHECK-NEXT:    [[TMP10:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP43]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB50]], ptr [[TMP10]], align 8
// CHECK-NEXT:    [[TMP11:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP43]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB52]], ptr [[TMP11]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR53:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP43]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR54:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR53]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR55:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP43]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB56:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR55]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR57:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP43]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB58:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR57]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP60]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR61:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP60]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR62:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR61]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR63:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP60]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB64:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR63]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR65:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP60]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB66:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR65]], align 8
// CHECK-NEXT:    [[TMP12:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP59]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR62]], ptr [[TMP12]], align 8
// CHECK-NEXT:    [[TMP13:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP59]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB64]], ptr [[TMP13]], align 8
// CHECK-NEXT:    [[TMP14:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP59]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB66]], ptr [[TMP14]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR67:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP59]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR68:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR67]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR69:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP59]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB70:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR69]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR71:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.2", ptr [[AGG_TEMP59]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB72:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR71]], align 8
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR54]] to i64
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR68]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB:%.*]] = sub i64 [[SUB_PTR_LHS_CAST]], [[SUB_PTR_RHS_CAST]]
// CHECK-NEXT:    [[CMP73:%.*]] = icmp ule i64 10, [[SUB_PTR_SUB]]
// CHECK-NEXT:    br label [[LAND_END]]
// CHECK:       land.end:
// CHECK-NEXT:    [[TMP15:%.*]] = phi i1 [ false, [[LAND_LHS_TRUE]] ], [ false, [[ENTRY:%.*]] ], [ [[CMP73]], [[LAND_RHS]] ], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[TMP15]], label [[CONT:%.*]], label [[TRAP:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR5:[0-9]+]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP74]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR75:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP74]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR76:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR75]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR77:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP74]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB78:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR77]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR79:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP74]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB80:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR79]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP81]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR82:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP81]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB83:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR82]], align 8
// CHECK-NEXT:    [[TMP16:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP81]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB83]], ptr [[TMP16]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR84:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP81]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR85:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR84]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR86:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP81]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB87:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR86]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR88:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP81]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB89:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR88]], align 8
// CHECK-NEXT:    [[CMP90:%.*]] = icmp ule ptr [[WIDE_PTR_PTR76]], [[WIDE_PTR_PTR85]]
// CHECK-NEXT:    br i1 [[CMP90]], label [[LAND_LHS_TRUE91:%.*]], label [[LAND_END144:%.*]]
// CHECK:       land.lhs.true91:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP92]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR93:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP92]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB94:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR93]], align 8
// CHECK-NEXT:    [[TMP17:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP92]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB94]], ptr [[TMP17]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR95:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP92]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR96:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR95]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR97:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP92]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB98:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR97]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR99:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP92]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB100:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR99]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP101]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR102:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP101]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR103:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR102]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR104:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP101]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB105:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR104]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR106:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP101]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB107:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR106]], align 8
// CHECK-NEXT:    [[CMP108:%.*]] = icmp ule ptr [[WIDE_PTR_PTR96]], [[WIDE_PTR_PTR103]]
// CHECK-NEXT:    br i1 [[CMP108]], label [[LAND_RHS109:%.*]], label [[LAND_END144]]
// CHECK:       land.rhs109:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP111]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR112:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP111]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB113:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR112]], align 8
// CHECK-NEXT:    [[TMP18:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP111]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB113]], ptr [[TMP18]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR114:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP111]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR115:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR114]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR116:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP111]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB117:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR116]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR118:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP111]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB119:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR118]], align 8
// CHECK-NEXT:    [[TMP19:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP110]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR115]], ptr [[TMP19]], align 8
// CHECK-NEXT:    [[TMP20:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP110]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB117]], ptr [[TMP20]], align 8
// CHECK-NEXT:    [[TMP21:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP110]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB119]], ptr [[TMP21]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR120:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP110]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR121:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR120]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR122:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP110]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB123:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR122]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR124:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP110]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB125:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR124]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP127]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR128:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP127]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR129:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR128]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR130:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP127]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB131:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR130]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR132:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP127]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB133:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR132]], align 8
// CHECK-NEXT:    [[TMP22:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP126]], i32 0, i32 0
// CHECK-NEXT:    store ptr [[WIDE_PTR_PTR129]], ptr [[TMP22]], align 8
// CHECK-NEXT:    [[TMP23:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP126]], i32 0, i32 1
// CHECK-NEXT:    store ptr [[WIDE_PTR_UB131]], ptr [[TMP23]], align 8
// CHECK-NEXT:    [[TMP24:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP126]], i32 0, i32 2
// CHECK-NEXT:    store ptr [[WIDE_PTR_LB133]], ptr [[TMP24]], align 8
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR134:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP126]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR135:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR134]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR136:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP126]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB137:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR136]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR138:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP126]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB139:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR138]], align 8
// CHECK-NEXT:    [[SUB_PTR_LHS_CAST140:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR121]] to i64
// CHECK-NEXT:    [[SUB_PTR_RHS_CAST141:%.*]] = ptrtoint ptr [[WIDE_PTR_PTR135]] to i64
// CHECK-NEXT:    [[SUB_PTR_SUB142:%.*]] = sub i64 [[SUB_PTR_LHS_CAST140]], [[SUB_PTR_RHS_CAST141]]
// CHECK-NEXT:    [[CMP143:%.*]] = icmp ule i64 10, [[SUB_PTR_SUB142]]
// CHECK-NEXT:    br label [[LAND_END144]]
// CHECK:       land.end144:
// CHECK-NEXT:    [[TMP25:%.*]] = phi i1 [ false, [[LAND_LHS_TRUE91]] ], [ false, [[CONT]] ], [ [[CMP143]], [[LAND_RHS109]] ], {{!annotation ![0-9]+}}
// CHECK-NEXT:    br i1 [[TMP25]], label [[CONT146:%.*]], label [[TRAP145:%.*]], {{!annotation ![0-9]+}}
// CHECK:       trap145:
// CHECK-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR5]], {{!annotation ![0-9]+}}
// CHECK-NEXT:    unreachable
// CHECK:       cont146:
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP147]], ptr align 8 [[AGG_TEMP]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR148:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP147]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR149:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR148]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR150:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP147]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB151:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR150]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR152:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.0", ptr [[AGG_TEMP147]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB153:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR152]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP154]], ptr align 8 [[AGG_TEMP2]], i64 24, i1 false)
// CHECK-NEXT:    [[WIDE_PTR_PTR_ADDR155:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP154]], i32 0, i32 0
// CHECK-NEXT:    [[WIDE_PTR_PTR156:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR155]], align 8
// CHECK-NEXT:    [[WIDE_PTR_UB_ADDR157:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP154]], i32 0, i32 1
// CHECK-NEXT:    [[WIDE_PTR_UB158:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR157]], align 8
// CHECK-NEXT:    [[WIDE_PTR_LB_ADDR159:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable.1", ptr [[AGG_TEMP154]], i32 0, i32 2
// CHECK-NEXT:    [[WIDE_PTR_LB160:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR159]], align 8
// CHECK-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 1 [[WIDE_PTR_PTR149]], ptr align 1 [[WIDE_PTR_PTR156]], i64 10, i1 false)
// CHECK-NEXT:    call void @llvm.assume(i1 true)
// CHECK-NEXT:    [[ADD_PTR:%.*]] = getelementptr inbounds i8, ptr [[WIDE_PTR_PTR149]], i64 10
// CHECK-NEXT:    ret void
//
void foo(void) {
    char *dst, *src;
    __builtin_memcpy(dst, src, 10);
}
