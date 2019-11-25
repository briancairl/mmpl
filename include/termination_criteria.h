#ifndef MMPL_TERMINATION_CRITERIA_H
#define MMPL_TERMINATION_CRITERIA_H

// C++ Standard Library
#include <type_traits>

// CRTP
#include <crtp/crtp.h>

// MMPL
#include <mmpl/state.h>

namespace mmpl
{

template<typename T>
struct TerminationCriteriaTraits;


template<typename TerminationCriteriaT>
using termination_criteria_state_t = typename TerminationCriteriaTraits<TerminationCriteriaT>::StateType;


template<typename DerivedT>
struct TerminationCriteriaBase
{
public:
  using StateType = termination_criteria_state_t<DerivedT>;

  template<typename UnaryChildFn>
  inline bool is_terminal(const StateType& query, UnaryChildFn&& child_fn) const
  {
    return CRTP_INDIRECT_M(is_terminal)(query, std::forward<UnaryChildFn>(child_fn));
  }

private:
  static_assert(is_state<StateType>(), "StateType must be a valid derivative type of StateBase");

  IMPLEMENT_CRTP_BASE_CLASS(TerminationCriteriaBase, DerivedT);
};


template<typename StateT>
class SingleGoalTerminationCriteria : public TerminationCriteriaBase<SingleGoalTerminationCriteria<StateT>>
{
public:
  explicit SingleGoalTerminationCriteria(const StateT& _state) :
    terminal_state_{_state}
  {}

private:
  template<typename UnaryChildFn>
  inline bool CRTP_OVERRIDE_M(is_terminal)(const StateT& query, const UnaryChildFn& child_fn) const
  {
    if (terminal_state_ == query)
    {
      child_fn(terminal_state_);
      return true;
    }
    return false;
  }

  StateT terminal_state_;

  IMPLEMENT_CRTP_DERIVED_CLASS(TerminationCriteriaBase, SingleGoalTerminationCriteria);
};


template<typename StateT>
struct TerminationCriteriaTraits<SingleGoalTerminationCriteria<StateT>>
{
  using StateType = StateT;
};

}  // namespace mmpl

#endif  // MMPL_TERMINATION_CRITERIA_H
