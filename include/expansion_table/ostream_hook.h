#ifndef MMPL_EXPANSION_TABLE_OSTREAM_HOOK_H
#define MMPL_EXPANSION_TABLE_OSTREAM_HOOK_H

// C++ Standard Library
#include <cstdint>
#include <ostream>

// MMPL
#include <mmpl/expansion_table.h>

namespace mmpl::expansion_table
{

/**
 * @brief Configuration options for OStreamHook
 */
struct OStreamHookOptions
{
  static constexpr std::uint32_t ON_EXPANSION = 1 << 1;
  static constexpr std::uint32_t ON_PARENT_LOOKUP = 1 << 2;
};

/**
 * @brief Expansion table wrapper which provides instrospection through <code>ostream</code> calls
 *
 *        Also counts the total number of state expansions. This is mainly menat for naive debugging.
 */
template <typename UnderlyingT, std::uint32_t FLAGS = OStreamHookOptions::ON_EXPANSION | OStreamHookOptions::ON_PARENT_LOOKUP>
class OStreamHook : public ExpansionTableBase<OStreamHook<UnderlyingT>>
{
public:
  OStreamHook(
    std::ostream& os,
    const bool on_expansion = true,
    const bool on_parent_lookup = true,
    UnderlyingT&& underlying = UnderlyingT{}) :
      os_{std::addressof(os)},
      expansion_count_{0UL},
      underlying_{std::move(underlying)}
  {}

private:
  using StateType = expansion_table_state_t<UnderlyingT>;
  using ValueType = expansion_table_value_t<UnderlyingT>;

  /**
   * @copydoc ExpansionTableBase::reset
   */
  inline void reset_impl()
  {
    expansion_count_ = 0UL;
    (*os_) << "table reset" << std::endl;
    underlying_.reset();
  }

  /**
   * @copydoc ExpansionTableBase::expand
   */
  inline bool expand_impl(const StateType& parent, const StateType& child, const ValueType& total_value)
  {
    if (underlying_.expand(parent, child, total_value))
    {
      ++expansion_count_;
    }
    else
    {
      return false;
    }
    if constexpr (FLAGS & OStreamHookOptions::ON_EXPANSION)
    {
      (*os_) << "expand : (count = " << expansion_count_ << ") " << parent << " --> " << child
             << ", value : " << total_value << std::endl;
    }
    return true;
  }

  /**
   * @copydoc ExpansionTableBase::is_expanded
   */
  inline bool is_expanded_impl(const StateType& query) const { return underlying_.is_expanded(query); }

  /**
   * @copydoc ExpansionTableBase::get_parent
   */
  inline StateType get_parent_impl(const StateType& query) const
  {
    const StateType parent = underlying_.get_parent(query);
    if constexpr (FLAGS & OStreamHookOptions::ON_PARENT_LOOKUP)
    {
      (*os_) << "get_parent: " << parent << " --> " << query << std::endl;
    }
    return parent;
  }

  /**
   * @copydoc ExpansionTableBase::get_total_value
   */
  inline ValueType get_total_value_impl(const StateType& query) const { return underlying_.get_total_value(query); }

  /// Logger
  std::ostream* os_;

  /// Expansion count
  std::size_t expansion_count_;

  /// Underlying expansion table
  UnderlyingT underlying_;

  friend ExpansionTableBase<OStreamHook<UnderlyingT>>;
};

}  // namespace mmpl::expansion_table

namespace  mmpl
{

template <typename UnderlyingT>
struct ExpansionTableTraits<expansion_table::OStreamHook<UnderlyingT>> : ExpansionTableTraits<UnderlyingT>
{};

}  // namespace mmpl

#endif  // MMPL_EXPANSION_TABLE_OSTREAM_HOOK_H
