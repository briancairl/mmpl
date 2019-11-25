#ifndef MMPL_STATE_SPACE_H
#define MMPL_STATE_SPACE_H

// C++ Standard Library
#include <type_traits>

// CRTP
#include <crtp/crtp.h>

// MMPL
#include <mmpl/metric.h>
#include <mmpl/state.h>
#include <mmpl/support.h>

namespace mmpl
{

template<typename T>
struct StateSpaceTraits;


template<typename StateSpaceT>
using state_space_state_t = typename StateSpaceTraits<StateSpaceT>::StateType;


template<typename DerivedT>
class StateSpaceBase
{
public:
  using StateType = state_space_state_t<DerivedT>;

  template<typename UnaryChildFn>
  inline bool for_each_child(const StateType& parent, UnaryChildFn&& child_fn)
  {
    return CRTP_INDIRECT_M(for_each_child)(parent, std::forward<UnaryChildFn>(child_fn));
  }

private:
  static_assert(is_state<StateType>(), MMPL_STATIC_ASSERT_MSG("StateType must be a valid derivative type of StateBase"));

  IMPLEMENT_CRTP_BASE_CLASS(StateSpaceBase, DerivedT);
};


template<typename StateSpaceT>
struct is_state_space : std::integral_constant<bool, std::is_base_of<StateSpaceBase<StateSpaceT>, StateSpaceT>::value> {};

}  // namespace mmpl

#endif  // MMPL_STATE_SPACE_H
