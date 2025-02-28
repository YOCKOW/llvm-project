//===-- ThreadPlanStack.h ---------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_TARGET_THREADPLANSTACK_H
#define LLDB_TARGET_THREADPLANSTACK_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "llvm/Support/RWMutex.h"

#include "lldb/Target/Target.h"
#include "lldb/Target/Thread.h"
#include "lldb/lldb-private-forward.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

// The ThreadPlans have a thread for use when they are asked all the ThreadPlan
// state machine questions, but they should never cache any pointers from their
// owning lldb_private::Thread.  That's because we want to be able to detach
// them from an owning thread, then reattach them by TID.
// The ThreadPlanStack holds the ThreadPlans for a given TID.  All its methods
// are private, and it should only be accessed through the owning thread.  When
// it is detached from a thread, all you can do is reattach it or delete it.
class ThreadPlanStack {
  friend class lldb_private::Thread;

public:
  ThreadPlanStack(const Thread &thread, bool make_empty = false);
  ~ThreadPlanStack() = default;

  using PlanStack = std::vector<lldb::ThreadPlanSP>;

  void DumpThreadPlans(Stream &s, lldb::DescriptionLevel desc_level,
                       bool include_internal) const;

  size_t CheckpointCompletedPlans();

  void RestoreCompletedPlanCheckpoint(size_t checkpoint);

  void DiscardCompletedPlanCheckpoint(size_t checkpoint);

  void ThreadDestroyed(Thread *thread);

  void PushPlan(lldb::ThreadPlanSP new_plan_sp);

  lldb::ThreadPlanSP PopPlan();

  lldb::ThreadPlanSP DiscardPlan();

  // If the input plan is nullptr, discard all plans.  Otherwise make sure this
  // plan is in the stack, and if so discard up to and including it.
  void DiscardPlansUpToPlan(ThreadPlan *up_to_plan_ptr);

  void DiscardAllPlans();

  void DiscardConsultingControllingPlans();

  lldb::ThreadPlanSP GetCurrentPlan() const;

  lldb::ThreadPlanSP GetCompletedPlan(bool skip_private = true) const;

  lldb::ThreadPlanSP GetPlanByIndex(uint32_t plan_idx,
                                    bool skip_private = true) const;

  lldb::ValueObjectSP GetReturnValueObject(bool &is_error) const;
  
  lldb::ExpressionVariableSP GetExpressionVariable() const;

  bool AnyPlans() const;

  bool AnyCompletedPlans() const;

  bool AnyDiscardedPlans() const;

  bool IsPlanDone(ThreadPlan *plan) const;

  bool WasPlanDiscarded(ThreadPlan *plan) const;

  ThreadPlan *GetPreviousPlan(ThreadPlan *current_plan) const;

  ThreadPlan *GetInnermostExpression() const;

  void WillResume();

  /// Clear the Thread* cache that each ThreadPlan contains.
  ///
  /// This is useful in situations like when a new Thread list is being
  /// generated.
  void ClearThreadCache();

  bool IsTID(lldb::tid_t tid) {
    return GetTID() == tid;
  }
  lldb::tid_t GetTID();
  void SetTID(lldb::tid_t tid);

private:
  lldb::ThreadPlanSP DiscardPlanNoLock();
  lldb::ThreadPlanSP GetCurrentPlanNoLock() const;
  void PrintOneStackNoLock(Stream &s, llvm::StringRef stack_name,
                           const PlanStack &stack,
                           lldb::DescriptionLevel desc_level,
                           bool include_internal) const;

  PlanStack m_plans;           ///< The stack of plans this thread is executing.
  PlanStack m_completed_plans; ///< Plans that have been completed by this
                               /// stop.  They get deleted when the thread
                               /// resumes.
  PlanStack m_discarded_plans; ///< Plans that have been discarded by this
                               /// stop.  They get deleted when the thread
                               /// resumes.
  size_t m_completed_plan_checkpoint = 0; // Monotonically increasing token for
                                          // completed plan checkpoints.
  std::unordered_map<size_t, PlanStack> m_completed_plan_store;
  mutable llvm::sys::RWMutex m_stack_mutex;

  // ThreadPlanStacks shouldn't be copied.
  ThreadPlanStack(ThreadPlanStack &rhs) = delete;
};

class ThreadPlanStackMap {
public:
  ThreadPlanStackMap(Process &process) : m_process(process) {}
  ~ThreadPlanStackMap() = default;

  // Prune the map using the current_threads list.
  void Update(ThreadList &current_threads, bool delete_missing,
              bool check_for_new = true);

  void AddThread(Thread &thread) {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    lldb::tid_t tid = thread.GetID();
    // If we already have a ThreadPlanStack for this thread, use it.
    if (m_plans_list.find(tid) != m_plans_list.end())
      return;

    m_plans_up_container.emplace_back(
        std::make_unique<ThreadPlanStack>(thread));
    m_plans_list.emplace(tid, m_plans_up_container.back().get());
  }

  bool RemoveTID(lldb::tid_t tid) {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    auto result = m_plans_list.find(tid);
    if (result == m_plans_list.end())
      return false;
    ThreadPlanStack *removed_stack = result->second;
    m_plans_list.erase(result);
    // Now find it in the stack storage:
    auto end = m_plans_up_container.end();
    auto iter = std::find_if(m_plans_up_container.begin(), end,
        [&] (std::unique_ptr<ThreadPlanStack> &stack) {
          return stack->IsTID(tid);
        });
    if (iter == end)
      return false;

    // Then tell the stack its thread has been destroyed:
    removed_stack->ThreadDestroyed(nullptr);
    // And then remove it from the container so it goes away.
    m_plans_up_container.erase(iter);
    return true;
  }

  ThreadPlanStack *Find(lldb::tid_t tid) {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    auto result = m_plans_list.find(tid);
    if (result == m_plans_list.end())
      return nullptr;
    else
      return result->second;
  }

  /// Clear the Thread* cache that each ThreadPlan contains.
  ///
  /// This is useful in situations like when a new Thread list is being
  /// generated.
  void ClearThreadCache() {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    for (auto &plan_list : m_plans_list)
      plan_list.second->ClearThreadCache();
  }

  // rename to Reactivate?
  void Activate(ThreadPlanStack &stack) {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    // Remove this from the detached plan list:
    auto end = m_detached_plans.end();    
    auto iter = std::find_if(m_detached_plans.begin(), end, 
        [&] (ThreadPlanStack *elem) {
          return elem == &stack; });
    if (iter != end)
      m_detached_plans.erase(iter);

    if (m_plans_list.find(stack.GetTID()) == m_plans_list.end())
      m_plans_list.emplace(stack.GetTID(), &stack);
    else
      m_plans_list.at(stack.GetTID()) = &stack;
  }

  void ScanForDetachedPlanStacks() {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    llvm::SmallVector<lldb::tid_t, 2> invalidated_tids;
    for (auto &pair : m_plans_list)
      if (pair.second->GetTID() != pair.first)
        invalidated_tids.push_back(pair.first);

    for (auto tid : invalidated_tids) {
      auto it = m_plans_list.find(tid);
      ThreadPlanStack *stack = it->second;
      m_plans_list.erase(it);
      m_detached_plans.push_back(stack);
    }
  }

  // This gets the vector of pointers to thread plans that aren't
  // currently running on a thread.  This is generally for thread
  // plans that represent asynchronous operations waiting to be
  // scheduled.
  // The vector will never have null ThreadPlanStacks in it.
  lldb::ThreadPlanSP FindThreadPlanInStack(
      llvm::function_ref<lldb::ThreadPlanSP(ThreadPlanStack &)> fn) {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    for (auto *stack : m_detached_plans)
      if (auto plan = fn(*stack))
        return plan;
    return {};
  }

  void Clear() {
    std::lock_guard<std::recursive_mutex> guard(m_stack_map_mutex);
    for (auto &plan : m_plans_list)
      plan.second->ThreadDestroyed(nullptr);
    m_plans_list.clear();
  }

  // Implements Process::DumpThreadPlans
  void DumpPlans(Stream &strm, lldb::DescriptionLevel desc_level, bool internal,
                 bool ignore_boring, bool skip_unreported);

  // Implements Process::DumpThreadPlansForTID
  bool DumpPlansForTID(Stream &strm, lldb::tid_t tid,
                       lldb::DescriptionLevel desc_level, bool internal,
                       bool ignore_boring, bool skip_unreported);
                       
  bool PrunePlansForTID(lldb::tid_t tid);

private:
  Process &m_process;
  // We don't want to make copies of these ThreadPlanStacks, there needs to be
  // just one of these tracking each piece of work.  But we need to move the
  // work from "attached to a TID" state to "detached" state, which is most
  // conveniently done by having organizing containers for each of the two 
  // states.
  // To make it easy to move these non-copyable entities in and out of the
  // organizing containers, we make the ThreadPlanStacks into unique_ptr's in a 
  // storage container - m_plans_up_container.  Storing unique_ptrs means we
  // can then use the pointer to the ThreadPlanStack in the "organizing"
  // containers, the TID->Stack map m_plans_list, and the detached plans
  // vector m_detached_plans.
  
  using PlansStore = std::vector<std::unique_ptr<ThreadPlanStack>>;
  PlansStore m_plans_up_container;
  std::vector<ThreadPlanStack *> m_detached_plans;
  mutable std::recursive_mutex m_stack_map_mutex;
  using PlansList = std::unordered_map<lldb::tid_t, ThreadPlanStack *>;
  PlansList m_plans_list;
  
};

} // namespace lldb_private

#endif // LLDB_TARGET_THREADPLANSTACK_H
