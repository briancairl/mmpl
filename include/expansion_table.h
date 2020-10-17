#ifndef MMPL_EXPANSION_TABLE_H
#define MMPL_EXPANSION_TABLE_H

// MMPL
#include <mmpl/crtp.h>
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>

namespace mmpl
{

template <typename ExpansionTableT> struct ExpansionTableTraits;


template <typename ExpansionTableT>
using expansion_table_state_t = typename ExpansionTableTraits<ExpansionTableT>::StateType;


template <typename ExpansionTableT>
using expansion_table_value_t = typename ExpansionTableTraits<ExpansionTableT>::ValueType;


/**
 * @brief Defines and interface for an object used to query state expansion
 */
template <typename DerivedT> struct ExpansionTableBase
{
public:
  /// Planning state type
  using StateType = expansion_table_state_t<DerivedT>;

  /// Planning metric value type
  using ValueType = expansion_table_value_t<DerivedT>;

  /**
   * @brief Resets internal state of table
   */
  inline void reset() { this->derived()->reset_impl(); }

  /**
   * @brief Sets next expanded state
   *
   * @param parent  parent state
   * @param child  parent state
   * @param total_value  total value associated with \p child
   *
   * @retval true  if <code>child</code> state was expanded
   * @retval false  otherwise
   */
  inline bool expand(const StateType& parent, const StateType& child, const ValueType& total_value)
  {
    return this->derived()->expand_impl(parent, child, total_value);
  }

  /**
   * @brief Check if state has been previously expanded
   *
   * @param query  query state
   *
   * @retval true  if <code>query</code> state has been expanded
   * @retval false  otherwise
   */
  inline bool is_expanded(const StateType& query) const { return this->derived()->is_expanded_impl(query); }

  /**
   * @brief Returns predecessor state for a given <code>query</code> state
   *
   * @param query  query state
   *
   * @return returns a predecessor state of <code>query</code>
   */
  inline StateType get_parent(const StateType& query) const
  {
    MMPL_RUNTIME_ASSERT(is_expanded(query));
    return this->derived()->get_parent_impl(query);
  }

  /**
   * @brief Returns accumulated metric value <code>query</code> state
   *
   * @param query  query state
   *
   * @return returns total value accumulated up to <code>query</code>
   *
   * @warn Expects the following precondition to be satisfied: <code>is_expanded(query) == true</code>
   */
  inline ValueType get_total_value(const StateType& query) const
  {
    MMPL_RUNTIME_ASSERT(is_expanded(query));
    return this->derived()->get_total_value_impl(query);
  }

  /**
   * @brief Returns accumulated metric value <code>query</code> state
   *
   * @param query  query state
   *
   * @return returns total value accumulated up to <code>query</code>
   * @note Returns max cost value if state is not expanded
   */
  inline ValueType try_get_total_value(const StateType& query) const
  {
    return is_expanded(query) ? this->derived()->get_total_value_impl(query) : Invalid<ValueType>::value;
  }

private:
  static_assert(is_value<ValueType>(), MMPL_STATIC_ASSERT_MSG("ValueType must be a valid metric value type"));

  IMPLEMENT_CRTP_BASE_CLASS(ExpansionTableBase, DerivedT);
};


template <typename OutputIteratorT, typename ExpansionTableT>
OutputIteratorT generate_reverse_path(
  OutputIteratorT output,
  expansion_table_state_t<ExpansionTableT> terminal,
  const ExpansionTableBase<ExpansionTableT>& expansion_table)
{
  using ValueType = expansion_table_value_t<ExpansionTableT>;

  *(++output) = terminal;
  while (expansion_table.get_total_value(terminal) != Null<ValueType>::value)
  {
    terminal = expansion_table.get_parent(terminal);
    *(++output) = terminal;
  }
  return output;
}


template <typename OutputIteratorT, typename LastOutputIteratorT, typename ExpansionTableT>
OutputIteratorT generate_reverse_path(
  OutputIteratorT output,
  LastOutputIteratorT last,
  expansion_table_state_t<ExpansionTableT> terminal,
  const ExpansionTableBase<ExpansionTableT>& expansion_table)
{
  using ValueType = expansion_table_value_t<ExpansionTableT>;

  if (output == last)
  {
    return output;
  }
  else
  {
    *(++output) = terminal;
  }

  while (output != last and expansion_table.get_total_value(terminal) != Null<ValueType>::value)
  {
    terminal = expansion_table.get_parent(terminal);
    *(++output) = terminal;
  }
  return output;
}


template <typename ExpansionTableT>
struct is_expansion_table
    : std::integral_constant<bool, std::is_base_of<ExpansionTableBase<ExpansionTableT>, ExpansionTableT>::value>
{};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_TABLE_H
