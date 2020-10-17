#ifndef MMPL_EXPANSION_QUEUE_H
#define MMPL_EXPANSION_QUEUE_H

// MMPL
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>

namespace mmpl
{

template <typename StateT, typename ValueT> struct StateValue
{
  static_assert(is_value<ValueT>(), "ValueT must be a valid metric value type");

  StateValue(const StateValue&) = default;

  /**
   * @brief Initialization constructor
   *
   * @param _state  state obect
   * @param _value  associated metric value
   */
  StateValue(const StateT& _state, const ValueT& _value) : state{_state}, value{_value} {}

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
  inline bool operator<(const StateValue& other) const { return this->value < other.value; }

  /**
   * @brief State metric value GT comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is greater than value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator>(const StateValue& other) const { return this->value > other.value; }

  /**
   * @brief State metric value EQ comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is equal to value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator==(const StateValue& other) const { return this->value == other.value; }

  /**
   * @brief State metric value NE comparison overload
   *
   * @param other  state value object
   *
   * @retval true  if metric value associated with <code>other</code> is not equal to value of <code>*this</code>
   * @retval false  otherwise
   */
  inline bool operator!=(const StateValue& other) const { return !this->operator==(other); }
};


template <typename ExpansionQueueT> struct ExpansionQueueTraits;


template <typename ExpansionQueueT>
using expansion_queue_state_t = typename ExpansionQueueTraits<ExpansionQueueT>::StateType;


template <typename ExpansionQueueT>
using expansion_queue_value_t = typename ExpansionQueueTraits<ExpansionQueueT>::ValueType;


/**
 * @brief Defines and interface for an object used to query state expansion
 */
template <typename DerivedT> struct ExpansionQueueBase
{
public:
  /// Planning state type
  using StateType = expansion_queue_state_t<DerivedT>;

  /// Planning metric value type
  using ValueType = expansion_queue_value_t<DerivedT>;

  /**
   * @brief Resets internal state of table
   */
  inline void reset() { this->derived()->reset_impl(); }

  /**
   * @brief Checks if there is anything left in queue
   */
  inline bool empty() const { return this->derived()->empty_impl(); }

  /**
   * @brief Places state and associated value into queue
   *
   * @param state  state to enqueue
   * @param total_value  total value associated with \p state
   */
  inline void enqueue(const StateType& state, const ValueType& total_value)
  {
    this->derived()->enqueue_impl(state, total_value);
  }

  /**
   * @brief Returns next state/value pair from queue; removes from queue
   */
  inline StateValue<StateType, ValueType> next() { return this->derived()->next_impl(); }

private:
  static_assert(is_value<ValueType>(), MMPL_STATIC_ASSERT_MSG("ValueType must be a valid metric value type"));

  IMPLEMENT_CRTP_BASE_CLASS(ExpansionQueueBase, DerivedT);
};


template <typename ExpansionQueueT>
struct is_expansion_queue
    : std::integral_constant<bool, std::is_base_of<ExpansionQueueBase<ExpansionQueueT>, ExpansionQueueT>::value>
{};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_QUEUE_H
