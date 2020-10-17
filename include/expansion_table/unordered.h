#ifndef MMPL_EXPANSION_TABLE_UNORDERED_H
#define MMPL_EXPANSION_TABLE_UNORDERED_H

// C++ Standard Library
#include <unordered_map>

// MMPL
#include <mmpl/expansion_table.h>

namespace mmpl::expansion_table
{

/**
 * @brief Expansion table based on a min-sorted <code>std::unordered_map</code> for hash-based state access
 */
template <typename StateT, typename ValueT>
class Unordered : public ExpansionTableBase<Unordered<StateT, ValueT>>
{
private:
  /**
   * @copydoc ExpansionTableBase::reset
   */
  inline void reset_impl()
  {
    child_parent_table_.clear();
    child_cost_table_.clear();
  }

  /**
   * @copydoc ExpansionTableBase::expand
   */
  inline bool expand_impl(const StateT& parent, const StateT& child, const ValueT& total_value)
  {
    return child_parent_table_.emplace(child, parent).second and child_cost_table_.emplace(child, total_value).second;
  }

  /**
   * @copydoc ExpansionTableBase::is_expanded
   */
  inline bool is_expanded_impl(const StateT& query) const
  {
    return child_parent_table_.find(query) != child_parent_table_.end();
  }

  /**
   * @copydoc ExpansionTableBase::get_parent
   */
  inline StateT get_parent_impl(const StateT& query) const { return child_parent_table_.find(query)->second; }

  /**
   * @copydoc ExpansionTableBase::get_total_value
   */
  inline ValueT get_total_value_impl(const StateT& query) const { return child_cost_table_.find(query)->second; }

  /// [child, total_cost] mapping
  std::unordered_map<StateT, ValueT, state_default_hash_t<StateT>> child_cost_table_;

  /// [child, parent] mapping
  std::unordered_map<StateT, StateT, state_default_hash_t<StateT>> child_parent_table_;

  friend class ExpansionTableBase<expansion_table::Unordered<StateT, ValueT>>;
};

}  // namespace mmpl::expansion_table

namespace  mmpl
{

template <typename StateT, typename ValueT> struct ExpansionTableTraits<expansion_table::Unordered<StateT, ValueT>>
{
  using StateType = StateT;
  using ValueType = ValueT;
};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_TABLE_UNORDERED_H
