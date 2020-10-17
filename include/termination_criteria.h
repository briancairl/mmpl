#ifndef MMPL_TERMINATION_CRITERIA_H
#define MMPL_TERMINATION_CRITERIA_H

// C++ Standard Library
#include <type_traits>
#include <utility>

// MMPL
#include <mmpl/crtp.h>
#include <mmpl/state.h>

namespace mmpl
{

template <typename T> struct TerminationCriteriaTraits;


template <typename TerminationCriteriaT>
using termination_criteria_state_t = typename TerminationCriteriaTraits<TerminationCriteriaT>::StateType;


template <typename DerivedT> struct TerminationCriteriaBase
{
public:
  using StateType = termination_criteria_state_t<DerivedT>;

  inline bool is_terminal(const StateType& query) const { return this->derived()->is_terminal_impl(query); }

  template<typename ExpansionTableT>
  inline bool is_terminal(ExpansionTableT&& exp_table, const StateType& query) const
  {
    return this->derived()->is_terminal_impl(std::forward<ExpansionTableT>(exp_table), query);
  }

private:
  IMPLEMENT_CRTP_BASE_CLASS(TerminationCriteriaBase, DerivedT);
};


template <typename StateT>
class SingleGoalTerminationCriteria : public TerminationCriteriaBase<SingleGoalTerminationCriteria<StateT>>
{
public:
  explicit SingleGoalTerminationCriteria(const StateT& _state) : terminal_state_{_state} {}

private:
  inline bool is_terminal_impl(const StateT& query) const { return terminal_state_ == query; }

  StateT terminal_state_;

  friend class TerminationCriteriaBase<SingleGoalTerminationCriteria<StateT>>;
};


template <typename StateT> struct TerminationCriteriaTraits<SingleGoalTerminationCriteria<StateT>>
{
  using StateType = StateT;
  static constexpr bool is_expansion_aware = false;
};

}  // namespace mmpl

#endif  // MMPL_TERMINATION_CRITERIA_H
