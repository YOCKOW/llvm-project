// NOTE: Assertions have been autogenerated by utils/update_cc_test_checks.py UTC_ARGS: --version 5
// Note: Specifying the triple seems to be necessary for `update_cc_test_checks.py` to work
// RUN: %clang_cc1 -O2 -triple arm64-apple-iphoneos -emit-llvm -fbounds-safety -fno-bounds-safety-bringup-missing-checks=compound_literal_init -Wno-bounds-attributes-init-list-side-effect -o - %s | FileCheck %s

#include <ptrcheck.h>

struct cb {
  int count;
  char* __counted_by(count) buf;
};
void consume_cb(struct cb);
void consume_cb_arr(struct cb (*arr)[]);

struct nested_cb {
  struct cb nested;
  int other;
};

struct nested_and_outer_cb {
  struct cb nested;
  int other;
  int count;
  char* __counted_by(count) buf;
};


int get_int(void);

struct cb_with_other_data {
  int count;
  char* __counted_by(count) buf;
  int other;
};
struct no_attr_with_other_data {
  int count;
  char* buf;
  int other;
};
_Static_assert(sizeof(struct cb_with_other_data) ==
               sizeof(struct no_attr_with_other_data), "size mismatch");
void consume_cb_with_other_data_arr(struct cb_with_other_data (*arr)[]);

union TransparentUnion {
  struct cb_with_other_data cb;
  struct no_attr_with_other_data no_cb;
} __attribute__((__transparent_union__));

void receive_transparent_union(union TransparentUnion);

// NOTE: We currently don't test globals because those don't generate bounds
// checks. We currently rely on Sema checks to prevent all invalid externally
// counted pointers. This only works because global initializers must be
// constant evaluable.

// =============================================================================
// Tests with __bidi_indexable source ptr
//
// =============================================================================

// CHECK-LABEL: define dso_local void @assign_via_ptr(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR0:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2:![0-9]+]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6:![0-9]+]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr(struct cb* ptr, int new_count,
                    char* __bidi_indexable new_ptr) {
  *ptr = (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @assign_operator(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void assign_operator(int new_count, char* __bidi_indexable new_ptr) {
  struct cb new;
  new = (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}


// CHECK-LABEL: define dso_local void @local_var_init(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void local_var_init(int new_count, char* __bidi_indexable new_ptr) {
  struct cb new = (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @call_arg(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_0_0_INSERT_EXT:%.*]] = zext i32 [[NEW_COUNT]] to i64
// CHECK-NEXT:    [[DOTFCA_0_INSERT:%.*]] = insertvalue [2 x i64] poison, i64 [[DOTCOMPOUNDLITERAL_SROA_0_0_INSERT_EXT]], 0
// CHECK-NEXT:    [[TMP0:%.*]] = ptrtoint ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]] to i64
// CHECK-NEXT:    [[DOTFCA_1_INSERT:%.*]] = insertvalue [2 x i64] [[DOTFCA_0_INSERT]], i64 [[TMP0]], 1
// CHECK-NEXT:    tail call void @consume_cb([2 x i64] [[DOTFCA_1_INSERT]]) #[[ATTR7:[0-9]+]]
// CHECK-NEXT:    ret void
//
void call_arg(int new_count, char* __bidi_indexable new_ptr) {
  consume_cb((struct cb) {
    .count = new_count,
    .buf = new_ptr
  });
}

// CHECK-LABEL: define dso_local [2 x i64] @return_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR5:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    [[RETVAL_SROA_0_0_INSERT_EXT:%.*]] = zext i32 [[NEW_COUNT]] to i64
// CHECK-NEXT:    [[DOTFCA_0_INSERT:%.*]] = insertvalue [2 x i64] poison, i64 [[RETVAL_SROA_0_0_INSERT_EXT]], 0
// CHECK-NEXT:    [[TMP0:%.*]] = ptrtoint ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]] to i64
// CHECK-NEXT:    [[DOTFCA_1_INSERT:%.*]] = insertvalue [2 x i64] [[DOTFCA_0_INSERT]], i64 [[TMP0]], 1
// CHECK-NEXT:    ret [2 x i64] [[DOTFCA_1_INSERT]]
//
struct cb return_cb(int new_count, char* __bidi_indexable new_ptr) {
  return (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @construct_not_used(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void construct_not_used(int new_count, char* __bidi_indexable new_ptr) {
  (void)(struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_nullptr(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR6:[0-9]+]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr null, ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_nullptr(struct cb* ptr, int new_count) {
  *ptr = (struct cb) {
    .count = new_count,
    .buf = 0x0
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_nested(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_nested(struct nested_cb* ptr,
                           char* __bidi_indexable new_ptr,
                           int new_count) {
  *ptr = (struct nested_cb) {
    .nested = {.buf = new_ptr, .count = new_count },
    .other = 0x0
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_nested_v2(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_nested_v2(struct nested_cb* ptr,
                              char* __bidi_indexable new_ptr,
                              int new_count) {
  *ptr = (struct nested_cb) {
    .nested = (struct cb){.buf = new_ptr, .count = new_count },
    .other = 0x0
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_nested_v3(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR0]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_210_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_210_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_4_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 20
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[DOTCOMPOUNDLITERAL_SROA_4_0__SROA_IDX]], align 4, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_5_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 24
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_5_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_nested_v3(struct nested_and_outer_cb* ptr,
                              char* __bidi_indexable new_ptr,
                              int new_count) {
  *ptr = (struct nested_and_outer_cb) {
    .nested = (struct cb){.buf = new_ptr, .count = new_count },
    .other = 0x0,
    .buf = new_ptr,
    .count = new_count
  };
}

// CHECK-LABEL: define dso_local void @array_of_struct_init(
// CHECK-SAME: ptr nocapture noundef readonly [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[ARR:%.*]] = alloca [2 x %struct.cb], align 8
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 32, ptr nonnull [[ARR]]) #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[ARR]], align 8, !tbaa [[TBAA8:![0-9]+]]
// CHECK-NEXT:    [[BUF:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[BUF]], align 8, !tbaa [[TBAA10:![0-9]+]]
// CHECK-NEXT:    [[ARRAYINIT_ELEMENT:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[ARRAYINIT_ELEMENT]], align 8, !tbaa [[TBAA8]]
// CHECK-NEXT:    [[BUF2:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 24
// CHECK-NEXT:    store ptr null, ptr [[BUF2]], align 8, !tbaa [[TBAA10]]
// CHECK-NEXT:    call void @consume_cb_arr(ptr noundef nonnull [[ARR]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 32, ptr nonnull [[ARR]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void array_of_struct_init(char* __bidi_indexable new_ptr,
                          int new_count) {
  struct cb arr[] = (struct cb[]) {
    {.count = new_count, .buf = new_ptr},
    {.count = 0x0, .buf = 0x0}
  };
  consume_cb_arr(&arr);
}


// CHECK-LABEL: define dso_local void @assign_via_ptr_other_data_side_effect(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    [[CALL:%.*]] = tail call i32 @get_int() #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 [[CALL]], ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_other_data_side_effect(struct cb_with_other_data* ptr,
                                           int new_count,
                                           char* __bidi_indexable new_ptr) {
  *ptr = (struct cb_with_other_data) {
    .count = new_count,
    .buf = new_ptr,
    // Side effect. Generates a warning but it is suppressed for this test
    .other = get_int()
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_other_data_side_effect_zero_ptr(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[CALL:%.*]] = tail call i32 @get_int() #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr null, ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 [[CALL]], ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_other_data_side_effect_zero_ptr(struct cb_with_other_data* ptr,
                                           int new_count,
                                           char* __bidi_indexable new_ptr) {
  *ptr = (struct cb_with_other_data) {
    .count = new_count,
    .buf = 0x0,
    // Side effect. Generates a warning but it is suppressed for this test
    .other = get_int()
  };
}

// CHECK-LABEL: define dso_local void @call_arg_transparent_union(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TMP:%.*]] = alloca [[UNION_TRANSPARENTUNION:%.*]], align 8
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[AGG_TMP]], align 8, !tbaa [[TBAA11:![0-9]+]]
// CHECK-NEXT:    [[BUF:%.*]] = getelementptr inbounds i8, ptr [[AGG_TMP]], i64 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[BUF]], align 8, !tbaa [[TBAA13:![0-9]+]]
// CHECK-NEXT:    [[OTHER:%.*]] = getelementptr inbounds i8, ptr [[AGG_TMP]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[OTHER]], align 8, !tbaa [[TBAA14:![0-9]+]]
// CHECK-NEXT:    call void @receive_transparent_union(ptr noundef nonnull [[AGG_TMP]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void call_arg_transparent_union(int new_count,
                                char* __bidi_indexable new_ptr) {
  receive_transparent_union(
    (struct cb_with_other_data) {
      .count = new_count,
      .buf = new_ptr,
      .other = 0x0
    }
  );
}

// CHECK-LABEL: define dso_local void @call_arg_transparent_union_untransparently(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[BYVAL_TEMP:%.*]] = alloca [[UNION_TRANSPARENTUNION:%.*]], align 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 24, ptr nonnull [[BYVAL_TEMP]]) #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[BYVAL_TEMP]], align 8
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0_BYVAL_TEMP_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[BYVAL_TEMP]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0_BYVAL_TEMP_SROA_IDX]], align 8
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0_BYVAL_TEMP_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[BYVAL_TEMP]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0_BYVAL_TEMP_SROA_IDX]], align 8
// CHECK-NEXT:    call void @receive_transparent_union(ptr noundef nonnull [[BYVAL_TEMP]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 24, ptr nonnull [[BYVAL_TEMP]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void call_arg_transparent_union_untransparently(int new_count,
                                char* __bidi_indexable new_ptr) {
  receive_transparent_union(
    (union TransparentUnion) {
      .cb = {
        .count = new_count,
        .buf = new_ptr,
        .other = 0x0
      }
    }
  );
}

// =============================================================================
// Tests with __counted_by source ptr
//
// =============================================================================
// CHECK-LABEL: define dso_local void @assign_via_ptr_from_ptr(
// CHECK-SAME: ptr nocapture noundef readnone [[PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_from_ptr(struct cb* ptr) {
  *ptr = (struct cb) {
    .count = ptr->count,
    .buf = ptr->buf
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_from_cb(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]], ptr noundef [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR6]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[NEW_PTR]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_from_cb(struct cb* ptr, int new_count,
                            char* __counted_by(new_count) new_ptr) {
  *ptr = (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @assign_operator_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void assign_operator_from_cb(int new_count,
                             char* __counted_by(new_count) new_ptr) {
  struct cb new;
  new = (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}


// CHECK-LABEL: define dso_local void @local_var_init_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void local_var_init_from_cb(int new_count,
                            char* __counted_by(new_count) new_ptr) {
  struct cb new = (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @call_arg_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr noundef [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_0_0_INSERT_EXT:%.*]] = zext i32 [[NEW_COUNT]] to i64
// CHECK-NEXT:    [[DOTFCA_0_INSERT:%.*]] = insertvalue [2 x i64] poison, i64 [[DOTCOMPOUNDLITERAL_SROA_0_0_INSERT_EXT]], 0
// CHECK-NEXT:    [[TMP0:%.*]] = ptrtoint ptr [[NEW_PTR]] to i64
// CHECK-NEXT:    [[DOTFCA_1_INSERT:%.*]] = insertvalue [2 x i64] [[DOTFCA_0_INSERT]], i64 [[TMP0]], 1
// CHECK-NEXT:    tail call void @consume_cb([2 x i64] [[DOTFCA_1_INSERT]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void call_arg_from_cb(int new_count,
                      char* __counted_by(new_count) new_ptr) {
  consume_cb((struct cb) {
    .count = new_count,
    .buf = new_ptr
  });
}

// CHECK-LABEL: define dso_local [2 x i64] @return_cb_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr noundef [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[RETVAL_SROA_0_0_INSERT_EXT:%.*]] = zext i32 [[NEW_COUNT]] to i64
// CHECK-NEXT:    [[DOTFCA_0_INSERT:%.*]] = insertvalue [2 x i64] poison, i64 [[RETVAL_SROA_0_0_INSERT_EXT]], 0
// CHECK-NEXT:    [[TMP0:%.*]] = ptrtoint ptr [[NEW_PTR]] to i64
// CHECK-NEXT:    [[DOTFCA_1_INSERT:%.*]] = insertvalue [2 x i64] [[DOTFCA_0_INSERT]], i64 [[TMP0]], 1
// CHECK-NEXT:    ret [2 x i64] [[DOTFCA_1_INSERT]]
//
struct cb return_cb_from_cb(int new_count,
                            char* __counted_by(new_count) new_ptr) {
  return (struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @construct_not_used_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR1]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    ret void
//
void construct_not_used_from_cb(int new_count,
                                char* __counted_by(new_count) new_ptr) {
  (void)(struct cb) {
    .count = new_count,
    .buf = new_ptr
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_nested_from_cb(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], ptr noundef [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR6]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[NEW_PTR]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_nested_from_cb(struct nested_cb* ptr,
                                   char* __counted_by(new_count) new_ptr,
                                   int new_count) {
  *ptr = (struct nested_cb) {
    .nested = {.buf = new_ptr, .count = new_count },
    .other = 0x0
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_nested_v2_from_cb(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], ptr noundef [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR6]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[NEW_PTR]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_nested_v2_from_cb(struct nested_cb* ptr,
                                      char* __counted_by(new_count)new_ptr,
                                      int new_count) {
  *ptr = (struct nested_cb) {
    .nested = (struct cb){.buf = new_ptr, .count = new_count },
    .other = 0x0
  };
}

// CHECK-LABEL: define dso_local void @array_of_struct_init_from_cb(
// CHECK-SAME: ptr noundef [[NEW_PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[ARR:%.*]] = alloca [2 x %struct.cb], align 8
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 32, ptr nonnull [[ARR]]) #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[ARR]], align 8, !tbaa [[TBAA8]]
// CHECK-NEXT:    [[BUF:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 8
// CHECK-NEXT:    store ptr [[NEW_PTR]], ptr [[BUF]], align 8, !tbaa [[TBAA10]]
// CHECK-NEXT:    [[ARRAYINIT_ELEMENT:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[ARRAYINIT_ELEMENT]], align 8, !tbaa [[TBAA8]]
// CHECK-NEXT:    [[BUF2:%.*]] = getelementptr inbounds i8, ptr [[ARR]], i64 24
// CHECK-NEXT:    store ptr null, ptr [[BUF2]], align 8, !tbaa [[TBAA10]]
// CHECK-NEXT:    call void @consume_cb_arr(ptr noundef nonnull [[ARR]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 32, ptr nonnull [[ARR]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void array_of_struct_init_from_cb(char* __counted_by(new_count) new_ptr,
                                  int new_count) {
  struct cb arr[] = (struct cb[]) {
    {.count = new_count, .buf = new_ptr},
    {.count = 0x0, .buf = 0x0}
  };
  consume_cb_arr(&arr);
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_other_data_side_effect_from_cb(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]], ptr noundef [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[CALL:%.*]] = tail call i32 @get_int() #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr [[NEW_PTR]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 [[CALL]], ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_other_data_side_effect_from_cb(struct cb_with_other_data* ptr,
                                                   int new_count,
                                                   char* __counted_by(new_count) new_ptr) {
  *ptr = (struct cb_with_other_data) {
    .count = new_count,
    .buf = new_ptr,
    // Side effect. Generates a warning but it is suppressed for this test
    .other = get_int()
  };
}

// CHECK-LABEL: define dso_local void @assign_via_ptr_other_data_side_effect_zero_ptr_from_cb(
// CHECK-SAME: ptr nocapture noundef writeonly [[PTR:%.*]], i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readnone [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[CALL:%.*]] = tail call i32 @get_int() #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[PTR]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 8
// CHECK-NEXT:    store ptr null, ptr [[DOTCOMPOUNDLITERAL_SROA_21_0__SROA_IDX]], align 8, !tbaa [[TBAA6]]
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[PTR]], i64 16
// CHECK-NEXT:    store i32 [[CALL]], ptr [[DOTCOMPOUNDLITERAL_SROA_3_0__SROA_IDX]], align 8, !tbaa [[TBAA2]]
// CHECK-NEXT:    ret void
//
void assign_via_ptr_other_data_side_effect_zero_ptr_from_cb(struct cb_with_other_data* ptr,
                                           int new_count,
                                           char* __counted_by(new_count) new_ptr) {
  *ptr = (struct cb_with_other_data) {
    .count = new_count,
    .buf = 0x0,
    // Side effect. Generates a warning but it is suppressed for this test
    .other = get_int()
  };
}

// CHECK-LABEL: define dso_local void @call_arg_transparent_union_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[AGG_TMP:%.*]] = alloca [[UNION_TRANSPARENTUNION:%.*]], align 8
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[AGG_TMP]], align 8, !tbaa [[TBAA11]]
// CHECK-NEXT:    [[BUF:%.*]] = getelementptr inbounds i8, ptr [[AGG_TMP]], i64 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[BUF]], align 8, !tbaa [[TBAA13]]
// CHECK-NEXT:    [[OTHER:%.*]] = getelementptr inbounds i8, ptr [[AGG_TMP]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[OTHER]], align 8, !tbaa [[TBAA14]]
// CHECK-NEXT:    call void @receive_transparent_union(ptr noundef nonnull [[AGG_TMP]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void call_arg_transparent_union_from_cb(int new_count,
                                        char* __bidi_indexable new_ptr) {
  receive_transparent_union(
    (struct cb_with_other_data) {
      .count = new_count,
      .buf = new_ptr,
      .other = 0x0
    }
  );
}


// CHECK-LABEL: define dso_local void @call_arg_transparent_union_untransparently_from_cb(
// CHECK-SAME: i32 noundef [[NEW_COUNT:%.*]], ptr nocapture noundef readonly [[NEW_PTR:%.*]]) local_unnamed_addr #[[ATTR3]] {
// CHECK-NEXT:  [[ENTRY:.*:]]
// CHECK-NEXT:    [[BYVAL_TEMP:%.*]] = alloca [[UNION_TRANSPARENTUNION:%.*]], align 8
// CHECK-NEXT:    [[AGG_TEMP_SROA_0_0_COPYLOAD:%.*]] = load ptr, ptr [[NEW_PTR]], align 8
// CHECK-NEXT:    call void @llvm.lifetime.start.p0(i64 24, ptr nonnull [[BYVAL_TEMP]]) #[[ATTR7]]
// CHECK-NEXT:    store i32 [[NEW_COUNT]], ptr [[BYVAL_TEMP]], align 8
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_21_0_BYVAL_TEMP_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[BYVAL_TEMP]], i64 8
// CHECK-NEXT:    store ptr [[AGG_TEMP_SROA_0_0_COPYLOAD]], ptr [[DOTCOMPOUNDLITERAL_SROA_21_0_BYVAL_TEMP_SROA_IDX]], align 8
// CHECK-NEXT:    [[DOTCOMPOUNDLITERAL_SROA_3_0_BYVAL_TEMP_SROA_IDX:%.*]] = getelementptr inbounds i8, ptr [[BYVAL_TEMP]], i64 16
// CHECK-NEXT:    store i32 0, ptr [[DOTCOMPOUNDLITERAL_SROA_3_0_BYVAL_TEMP_SROA_IDX]], align 8
// CHECK-NEXT:    call void @receive_transparent_union(ptr noundef nonnull [[BYVAL_TEMP]]) #[[ATTR7]]
// CHECK-NEXT:    call void @llvm.lifetime.end.p0(i64 24, ptr nonnull [[BYVAL_TEMP]]) #[[ATTR7]]
// CHECK-NEXT:    ret void
//
void call_arg_transparent_union_untransparently_from_cb(int new_count,
                                char* __bidi_indexable new_ptr) {
  receive_transparent_union(
    (union TransparentUnion) {
      .cb = {
        .count = new_count,
        .buf = new_ptr,
        .other = 0x0
      }
    }
  );
}
//.
// CHECK: [[TBAA2]] = !{[[META3:![0-9]+]], [[META3]], i64 0}
// CHECK: [[META3]] = !{!"int", [[META4:![0-9]+]], i64 0}
// CHECK: [[META4]] = !{!"omnipotent char", [[META5:![0-9]+]], i64 0}
// CHECK: [[META5]] = !{!"Simple C/C++ TBAA"}
// CHECK: [[TBAA6]] = !{[[META7:![0-9]+]], [[META7]], i64 0}
// CHECK: [[META7]] = !{!"any pointer", [[META4]], i64 0}
// CHECK: [[TBAA8]] = !{[[META9:![0-9]+]], [[META3]], i64 0}
// CHECK: [[META9]] = !{!"cb", [[META3]], i64 0, [[META7]], i64 8}
// CHECK: [[TBAA10]] = !{[[META9]], [[META7]], i64 8}
// CHECK: [[TBAA11]] = !{[[META12:![0-9]+]], [[META3]], i64 0}
// CHECK: [[META12]] = !{!"cb_with_other_data", [[META3]], i64 0, [[META7]], i64 8, [[META3]], i64 16}
// CHECK: [[TBAA13]] = !{[[META12]], [[META7]], i64 8}
// CHECK: [[TBAA14]] = !{[[META12]], [[META3]], i64 16}
//.
