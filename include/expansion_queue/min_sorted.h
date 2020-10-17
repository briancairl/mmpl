#ifndef MMPL_EXPANSION_QUEUE_MIN_SORTE_H
#define MMPL_EXPANSION_QUEUE_MIN_SORTE_H

// C++ Standard Library
#include <cstdint>
#include <memory>
#include <queue>
#include <vector>

// MMPL
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>

namespace mmpl::expansion_queue
{

/**
 * @brief Expansion queue based on a min-sorted <code>std::priority_queue</code> (heap)
 */
template <typename StateT, typename ValueT, typename StateValueAllocatorT = std::allocator<StateValue<StateT, ValueT>>>
class MinSorted : public ExpansionQueueBase<MinSorted<StateT, ValueT, StateValueAllocatorT>>
{
public:
  MinSorted() = default;

  explicit MinSorted(std::size_t reserved) :
      queue_{std::greater<StateValueType>{}, [reserved]() -> std::vector<StateValueType, StateValueAllocatorT> {
               std::vector<StateValueType, StateValueAllocatorT> c;
               c.reserved(reserved);
               return c;
             }()}
  {}

private:
  using StateValueType = StateValue<StateT, ValueT>;

  /**
   * @copydoc ExpansionQueueBase::reset
   */
  inline void reset_impl()
  {
    std::priority_queue<StateValueType, std::vector<StateValueType, StateValueAllocatorT>, std::greater<StateValueType>>
      swap_queue;
    queue_.swap(swap_queue);
  }

  /**
   * @copydoc ExpansionQueueBase::empty
   */
  inline bool empty_impl() const { return queue_.empty(); }

  /**
   * @copydoc ExpansionQueueBase::enqueue
   */
  inline void enqueue_impl(const StateT& state, const ValueT& total_value) { queue_.emplace(state, total_value); }

  /**
   * @copydoc ExpansionQueueBase::next
   */
  inline StateValueType next_impl()
  {
    const auto v = queue_.top();
    queue_.pop();
    return v;
  }

  /// Underlying priority queue
  std::priority_queue<StateValueType, std::vector<StateValueType, StateValueAllocatorT>, std::greater<StateValueType>>
    queue_;

  friend class ExpansionQueueBase<MinSorted<StateT, ValueT, StateValueAllocatorT>>;
};

}  // namespace mmpl::expansion_queue

namespace mmpl
{

template <typename StateT, typename ValueT, typename StateValueAllocatorT>
struct ExpansionQueueTraits<expansion_queue::MinSorted<StateT, ValueT, StateValueAllocatorT>>
{
  using StateType = StateT;
  using ValueType = ValueT;
};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_QUEUE_MIN_SORTE_H
