#ifndef MMPL_EXPANSION_TABLE_H
#define MMPL_EXPANSION_TABLE_H

// C++ Standard Library
#include <ostream>
#include <unordered_map>

// CRTP
#include <crtp/crtp.h>

// MMPL
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>


namespace mmpl
{

template<typename ExpansionTableT>
struct ExpansionTableTraits;


template<typename ExpansionTableT>
using expansion_table_state_t = typename ExpansionTableTraits<ExpansionTableT>::StateType;


template<typename ExpansionTableT>
using expansion_table_value_t = typename ExpansionTableTraits<ExpansionTableT>::ValueType;


/**
 * @brief Defines and interface for an object used to query state expansion
 */
template<typename DerivedT>
struct ExpansionTableBase
{
public:
  /// Planning state type
  using StateType = expansion_table_state_t<DerivedT>;

  /// Planning metric value type
  using ValueType = expansion_table_value_t<DerivedT>;

  /**
   * @brief Resets internal state of table
   */
  inline void reset()
  {
    CRTP_INDIRECT_M(reset)();
  }

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
    return CRTP_INDIRECT_M(expand)(parent, child, total_value);
  }

  /**
   * @brief Check if state has been previously expanded
   *
   * @param query  query state
   *
   * @retval true  if <code>query</code> state has been expanded
   * @retval false  otherwise
   */
  inline bool is_expanded(const StateType& query) const
  {
    return CRTP_INDIRECT_M(is_expanded)(query);
  }

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
    return CRTP_INDIRECT_M(get_parent)(query);
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
    return CRTP_INDIRECT_M(get_total_value)(query);
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
    return is_expanded(query) ? CRTP_INDIRECT_M(get_total_value)(query) : Invalid<ValueType>::value;
  }

private:
  static_assert(is_value<ValueType>(), MMPL_STATIC_ASSERT_MSG("ValueType must be a valid metric value type"));

  IMPLEMENT_CRTP_BASE_CLASS(ExpansionTableBase, DerivedT);
};


template<typename OutputIteratorT, typename ExpansionTableT>
void generate_reverse_path(OutputIteratorT output,
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
}


template<typename ExpansionTableT>
struct is_expansion_table :
  std::integral_constant<bool, std::is_base_of<ExpansionTableBase<ExpansionTableT>, ExpansionTableT>::value> {};


template<typename StateT, typename ValueT>
class UnorderedExpansionTable : public ExpansionTableBase<UnorderedExpansionTable<StateT, ValueT>>
{
private:
  /**
   * @copydoc ExpansionTableBase::reset
   */
  inline void CRTP_OVERRIDE_M(reset)()
  {
    child_parent_table_.clear();
    child_cost_table_.clear();
  }

  /**
   * @copydoc ExpansionTableBase::expand
   */
  inline bool CRTP_OVERRIDE_M(expand)(const StateT& parent, const StateT& child, const ValueT& total_value)
  {
    return child_parent_table_.emplace(child, parent).second and
           child_cost_table_.emplace(child, total_value).second;
  }

  /**
   * @copydoc ExpansionTableBase::is_expanded
   */
  inline bool CRTP_OVERRIDE_M(is_expanded)(const StateT& query) const
  {
    return child_parent_table_.find(query) != child_parent_table_.end();
  }

  /**
   * @copydoc ExpansionTableBase::get_parent
   */
  inline StateT CRTP_OVERRIDE_M(get_parent)(const StateT& query) const
  {
    return child_parent_table_.find(query)->second;
  }

  /**
   * @copydoc ExpansionTableBase::get_total_value
   */
  inline ValueT CRTP_OVERRIDE_M(get_total_value)(const StateT& query) const
  {
    return child_cost_table_.find(query)->second;
  }

  /// [child, total_cost] mapping
  std::unordered_map<StateT, ValueT, state_default_hash_t<StateT>> child_cost_table_;

  /// [child, parent] mapping
  std::unordered_map<StateT, StateT, state_default_hash_t<StateT>> child_parent_table_;

  IMPLEMENT_CRTP_DERIVED_CLASS(ExpansionTableBase, UnorderedExpansionTable);
};


template<typename StateT, typename ValueT>
struct ExpansionTableTraits<UnorderedExpansionTable<StateT, ValueT>>
{
  using StateType = StateT;
  using ValueType = ValueT;
};


template<typename UnderlyingT>
class ExpansionTableOutputStreamHook : public ExpansionTableBase<ExpansionTableOutputStreamHook<UnderlyingT>>
{
public:
  ExpansionTableOutputStreamHook(std::ostream& os,
                                 const bool on_expansion = true,
                                 const bool on_parent_lookup = true,
                                 UnderlyingT&& underlying = UnderlyingT{}) :
    os_{std::addressof(os)},
    expansion_count_{0UL},
    on_expansion_{on_expansion},
    on_parent_lookup_{on_parent_lookup},
    underlying_{std::move(underlying)}
  {}

private:
  using StateType = expansion_table_state_t<UnderlyingT>;
  using ValueType = expansion_table_value_t<UnderlyingT>;

  /**
   * @copydoc ExpansionTableBase::reset
   */
  inline void CRTP_OVERRIDE_M(reset)()
  {
    expansion_count_ = 0UL;
    (*os_) << "table reset" << std::endl;
    underlying_.reset();
  }

  /**
   * @copydoc ExpansionTableBase::expand
   */
  inline bool CRTP_OVERRIDE_M(expand)(const StateType& parent, const StateType& child, const ValueType& total_value)
  {
    if (underlying_.expand(parent, child, total_value))
    {
      ++expansion_count_; 
    }
    else
    {
      return false;
    }
    if (on_expansion_)
    {
      (*os_) << "expand : (count = " << expansion_count_ << ") " << parent << " --> " << child << ", value : " << total_value << std::endl;
    }
    return true;
  }

  /**
   * @copydoc ExpansionTableBase::is_expanded
   */
  inline bool CRTP_OVERRIDE_M(is_expanded)(const StateType& query) const
  {
    return underlying_.is_expanded(query);
  }

  /**
   * @copydoc ExpansionTableBase::get_parent
   */
  inline StateType CRTP_OVERRIDE_M(get_parent)(const StateType& query) const
  {
    const StateType parent = underlying_.get_parent(query);
    if (on_parent_lookup_)
    {
      (*os_) << "get_parent: " << parent << " --> " << query << std::endl;
    }
    return parent;
  }

  /**
   * @copydoc ExpansionTableBase::get_total_value
   */
  inline ValueType CRTP_OVERRIDE_M(get_total_value)(const StateType& query) const
  {
    return underlying_.get_total_value(query);
  }

  /// Logger
  std::ostream* os_;

  /// Expansion count
  std::size_t expansion_count_;

  /// Toggles logs on expansion
  bool on_expansion_;

  /// Toggles on parent lookup
  bool on_parent_lookup_;

  /// Underlying expansion table
  UnderlyingT underlying_;

  IMPLEMENT_CRTP_DERIVED_CLASS(ExpansionTableBase, ExpansionTableOutputStreamHook);
};


template<typename UnderlyingT>
struct ExpansionTableTraits<ExpansionTableOutputStreamHook<UnderlyingT>> : ExpansionTableTraits<UnderlyingT> {};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_TABLE_H
