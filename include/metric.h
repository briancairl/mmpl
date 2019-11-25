#ifndef MMPL_METRIC_H
#define MMPL_METRIC_H

// C++ Standard Library

// CRTP
#include <crtp/crtp.h>

// MMPL
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>

namespace mmpl
{

template<typename T>
struct MetricTraits;


template<typename MetricT>
using metric_value_t = typename MetricTraits<MetricT>::ValueType;


template<typename MetricT>
using metric_state_t = typename MetricTraits<MetricT>::StateType;


template<typename DerivedT>
class MetricBase
{
public:
  using ValueType = metric_value_t<DerivedT>;
  using StateType = metric_state_t<DerivedT>;

  inline ValueType operator()(const StateType& parent, const StateType& child)
  {
    return CRTP_INDIRECT_M(get_value)(parent, child);
  }

private:
  static_assert(is_state<StateType>(), MMPL_STATIC_ASSERT_MSG("StateType must be a valid derivative type of StateBase"));
  static_assert(is_value<ValueType>(), MMPL_STATIC_ASSERT_MSG("ValueType must be a valid metric value type"));

  IMPLEMENT_CRTP_BASE_CLASS(MetricBase, DerivedT);
};


template<typename MetricT>
struct is_metric : std::integral_constant<bool, std::is_base_of<MetricBase<MetricT>, MetricT>::value> {};


}  // namespace mmpl

#endif  // MMPL_METRIC_H
