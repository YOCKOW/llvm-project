// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --replace-value-regex "!annotation ![0-9]+" "!tbaa ![0-9]+" "!tbaa\.struct ![0-9]+" "!nosanitize ![0-9]+" "!srcloc ![0-9]+"

// RUN: %clang_cc1 -triple x86_64 -O2 -fbounds-safety -emit-llvm %s -o - | FileCheck --check-prefixes O2 %s
// RUN: %clang_cc1 -triple x86_64 -O0 -fbounds-safety -emit-llvm %s -o - | FileCheck --check-prefixes O0 %s
// RUN: %clang_cc1 -triple x86_64 -O2 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck --check-prefixes O2 %s
// RUN: %clang_cc1 -triple x86_64 -O0 -fbounds-safety -x objective-c -fbounds-attributes-objc-experimental -emit-llvm %s -o - | FileCheck --check-prefixes O0 %s

// O2-LABEL: @foo(
// O2-NEXT:  entry:
// O2-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i8, ptr [[ARR:%.*]], i64 4
// O2-NEXT:    [[TMP0:%.*]] = getelementptr i8, ptr [[ARR]], i64 40
// O2-NEXT:    [[TMP1:%.*]] = icmp ult ptr [[TMP0]], [[UPPER]], {{!annotation ![0-9]+}}
// O2-NEXT:    [[TMP2:%.*]] = icmp uge ptr [[TMP0]], [[ARR]], {{!annotation ![0-9]+}}
// O2-NEXT:    [[OR_COND:%.*]] = and i1 [[TMP1]], [[TMP2]], {{!annotation ![0-9]+}}
// O2-NEXT:    br i1 [[OR_COND]], label [[CONT1:%.*]], label [[TRAP:%.*]], {{!annotation ![0-9]+}}
// O2:       trap:
// O2-NEXT:    tail call void @llvm.ubsantrap(i8 25) #[[ATTR2:[0-9]+]], {{!annotation ![0-9]+}}
// O2-NEXT:    unreachable, {{!annotation ![0-9]+}}
// O2:       cont1:
// O2-NEXT:    [[TMP3:%.*]] = load i32, ptr [[TMP0]], align 4, {{!tbaa ![0-9]+}}
// O2-NEXT:    ret i32 [[TMP3]]
//
// O0-LABEL: @foo(
// O0-NEXT:  entry:
// O0-NEXT:    [[ARR_ADDR:%.*]] = alloca ptr, align 8
// O0-NEXT:    [[BIDI_ARR:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// O0-NEXT:    [[I:%.*]] = alloca i32, align 4
// O0-NEXT:    [[AGG_TEMP:%.*]] = alloca %"__bounds_safety::wide_ptr.bidi_indexable", align 8
// O0-NEXT:    store ptr [[ARR:%.*]], ptr [[ARR_ADDR]], align 8
// O0-NEXT:    [[TMP0:%.*]] = load ptr, ptr [[ARR_ADDR]], align 8
// O0-NEXT:    [[UPPER:%.*]] = getelementptr inbounds i32, ptr [[TMP0]], i64 1
// O0-NEXT:    [[TMP1:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[BIDI_ARR]], i32 0, i32 0
// O0-NEXT:    store ptr [[TMP0]], ptr [[TMP1]], align 8
// O0-NEXT:    [[TMP2:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[BIDI_ARR]], i32 0, i32 1
// O0-NEXT:    store ptr [[UPPER]], ptr [[TMP2]], align 8
// O0-NEXT:    [[TMP3:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[BIDI_ARR]], i32 0, i32 2
// O0-NEXT:    store ptr [[TMP0]], ptr [[TMP3]], align 8
// O0-NEXT:    br label [[BINGO:%.*]]
// O0:       bingo:
// O0-NEXT:    call void @llvm.memcpy.p0.p0.i64(ptr align 8 [[AGG_TEMP]], ptr align 8 [[BIDI_ARR]], i64 24, i1 false)
// O0-NEXT:    [[WIDE_PTR_PTR_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 0
// O0-NEXT:    [[WIDE_PTR_PTR:%.*]] = load ptr, ptr [[WIDE_PTR_PTR_ADDR]], align 8
// O0-NEXT:    [[TMP4:%.*]] = getelementptr i32, ptr [[WIDE_PTR_PTR]], i64 10
// O0-NEXT:    [[WIDE_PTR_UB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 1
// O0-NEXT:    [[WIDE_PTR_UB:%.*]] = load ptr, ptr [[WIDE_PTR_UB_ADDR]], align 8
// O0-NEXT:    [[WIDE_PTR_LB_ADDR:%.*]] = getelementptr inbounds %"__bounds_safety::wide_ptr.bidi_indexable", ptr [[AGG_TEMP]], i32 0, i32 2
// O0-NEXT:    [[WIDE_PTR_LB:%.*]] = load ptr, ptr [[WIDE_PTR_LB_ADDR]], align 8
// O0-NEXT:    [[TMP5:%.*]] = icmp ult ptr [[TMP4]], [[WIDE_PTR_UB]], {{!annotation ![0-9]+}}
// O0-NEXT:    br i1 [[TMP5]], label [[CONT:%.*]], label [[TRAP:%.*]], {{!annotation ![0-9]+}}
// O0:       trap:
// O0-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR3:[0-9]+]], {{!annotation ![0-9]+}}
// O0-NEXT:    unreachable, {{!annotation ![0-9]+}}
// O0:       cont:
// O0-NEXT:    [[TMP6:%.*]] = icmp uge ptr [[TMP4]], [[WIDE_PTR_LB]], {{!annotation ![0-9]+}}
// O0-NEXT:    br i1 [[TMP6]], label [[CONT2:%.*]], label [[TRAP1:%.*]], {{!annotation ![0-9]+}}
// O0:       trap1:
// O0-NEXT:    call void @llvm.ubsantrap(i8 25) #[[ATTR3]], {{!annotation ![0-9]+}}
// O0-NEXT:    unreachable, {{!annotation ![0-9]+}}
// O0:       cont2:
// O0-NEXT:    [[TMP7:%.*]] = load i32, ptr [[TMP4]], align 4
// O0-NEXT:    store i32 [[TMP7]], ptr [[I]], align 4
// O0-NEXT:    [[TMP8:%.*]] = load i32, ptr [[I]], align 4
// O0-NEXT:    ret i32 [[TMP8]]
//
int foo(int * arr) {
    int * bidi_arr = arr;
bingo:;
    int i = bidi_arr[10];

    return i;
}


