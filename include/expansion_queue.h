#ifndef MMPL_EXPANSION_QUEUE_H
#define MMPL_EXPANSION_QUEUE_H

// C++ Standard Library
#include <cstdint>
#include <functional>
#include <memory>
#include <queue>
#include <vector>

// MMPL
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>

namespace mmpl
{

template<typename StateT, typename ValueT>
struct StateValue
{
  static_assert(is_state<StateT>(), "StateT must be a valid derivative type of StateBase");
  static_assert(is_value<ValueT>(), "ValueT must be a valid metric value type");

  StateValue(const StateValue&) = default;

  /**
   * @brief Initialization constructor
   *
   * @param _state  state obect
   * @param _value  associated metric value
   */
  StateValue(const StateT& _state, const ValueT& _value) :
    state{_state},
    value{_value}
  {}

  /// State type
  StateT state;

  /// Associated metric value type
  ValueT value;

  /**
   * @brief State metric value LT comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is less than value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator<(const StateValue& other) const
  {
    return this->value < other.value;
  }

  /**
   * @brief State metric value GT comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is greater than value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator>(const StateValue& other) const
  {
    return this->value > other.value;
  }

  /**
   * @brief State metric value EQ comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is equal to value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator==(const StateValue& other) const
  {
    return this->value == other.value;
  }

  /**
   * @brief State metric value NE comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is not equal to value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator!=(const StateValue& other) const
  {
    return !this->operator==(other);
  }
};


template<typename ExpansionQueueT>
struct ExpansionQueueTraits;


template<typename ExpansionQueueT>
using expansion_queue_state_t = typename ExpansionQueueTraits<ExpansionQueueT>::StateType;


template<typename ExpansionQueueT>
using expansion_queue_value_t = typename ExpansionQueueTraits<ExpansionQueueT>::ValueType;


/**
 * @brief Defines and interface for an object used to query state expansion
 */
template<typename DerivedT>
struct ExpansionQueueBase
{
public:
  /// Planning state type
  using StateType = expansion_queue_state_t<DerivedT>;

  /// Planning metric value type
  using ValueType = expansion_queue_value_t<DerivedT>;

  /**
   * @brief Resets internal state of table
   */
  inline void reset()
  {
    CRTP_INDIRECT_M(reset)();
  }

  /**
   * @brief Checks if there is anything left in queue
   */
  inline bool empty() const
  {
    return CRTP_INDIRECT_M(empty)();
  }

  /**
   * @brief Places state and associated value into queue
   *
   * @param state  state to enqueue
   * @param total_value  total value associated with \p state
   */
  inline void enqueue(const StateType& state, const ValueType& total_value)
  {
    CRTP_INDIRECT_M(enqueue)(state, total_value);
  }

  /**
   * @brief Returns next state/value pair from queue; removes from queue
   */
  inline StateValue<StateType, ValueType> next()
  {
    return CRTP_INDIRECT_M(next)();
  }

private:
  static_assert(is_state<StateType>(), MMPL_STATIC_ASSERT_MSG("StateType must be a valid derivative type of StateBase"));
  static_assert(is_value<ValueType>(), MMPL_STATIC_ASSERT_MSG("ValueType must be a valid metric value type"));

  IMPLEMENT_CRTP_BASE_CLASS(ExpansionQueueBase, DerivedT);
};


template<typename ExpansionQueueT>
struct is_expansion_queue :
  std::integral_constant<bool, std::is_base_of<ExpansionQueueBase<ExpansionQueueT>, ExpansionQueueT>::value> {};


template<typename StateT, typename ValueT, typename StateValueAllocatorT = std::allocator<StateValue<StateT, ValueT>>>
class MinSortedExpansionQueue : public ExpansionQueueBase<MinSortedExpansionQueue<StateT, ValueT, StateValueAllocatorT>>
{
public:
  MinSortedExpansionQueue() = default;
  
  explicit MinSortedExpansionQueue(std::size_t reserved) :
    queue_
    {
      std::greater<StateValueType>{},
      [reserved]() -> std::vector<StateValueType, StateValueAllocatorT>
      {
        std::vector<StateValueType, StateValueAllocatorT> c;
        c.reserved(reserved);
        return c;
      }()
    }
  {}

private:
  using StateValueType = StateValue<StateT, ValueT>;

  /**
   * @copydoc ExpansionQueueBase::reset
   */
  inline void CRTP_OVERRIDE_M(reset)()
  {
    queue_.clear();
  }

  /**
   * @copydoc ExpansionQueueBase::empty
   */
  inline bool CRTP_OVERRIDE_M(empty)() const
  {
    return queue_.empty();
  }

  /**
   * @copydoc ExpansionQueueBase::enqueue
   */
  inline void CRTP_OVERRIDE_M(enqueue)(const StateT& state, const ValueT& total_value)
  {
    queue_.emplace(state, total_value);
  }

  /**
   * @copydoc ExpansionQueueBase::next
   */
  inline StateValueType CRTP_OVERRIDE_M(next)()
  {
    const auto v = queue_.top();
    queue_.pop();
    return v;
  }

  /// Underlying priority queue
  std::priority_queue<StateValueType,
                      std::vector<StateValueType, StateValueAllocatorT>,
                      std::greater<StateValueType>> queue_;

  IMPLEMENT_CRTP_DERIVED_CLASS(ExpansionQueueBase, MinSortedExpansionQueue);
};


template<typename StateT, typename ValueT, typename StateValueAllocatorT>
struct ExpansionQueueTraits<MinSortedExpansionQueue<StateT, ValueT, StateValueAllocatorT>>
{
  using StateType = StateT;
  using ValueType = ValueT;
};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_QUEUE_H
