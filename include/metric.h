#ifndef MMPL_METRIC_H
#define MMPL_METRIC_H

// C++ Standard Library

// MMPL
#include <mmpl/crtp.h>
#include <mmpl/state.h>
#include <mmpl/support.h>
#include <mmpl/value.h>

namespace mmpl
{

template <typename T> struct MetricTraits;


template <typename MetricT> using metric_value_t = typename MetricTraits<MetricT>::ValueType;


template <typename MetricT> using metric_state_t = typename MetricTraits<MetricT>::StateType;


template <typename DerivedT> class MetricBase
{
public:
  using ValueType = metric_value_t<DerivedT>;
  using StateType = metric_state_t<DerivedT>;

  inline ValueType operator()(const StateType& parent, const StateType& child)
  {
    return this->derived()->get_value_impl(parent, child);
  }

private:
  static_assert(is_value<ValueType>(), MMPL_STATIC_ASSERT_MSG("ValueType must be a valid metric value type"));

  IMPLEMENT_CRTP_BASE_CLASS(MetricBase, DerivedT);
};


template <typename MetricT>
struct is_metric : std::integral_constant<bool, std::is_base_of<MetricBase<MetricT>, MetricT>::value>
{};


}  // namespace mmpl

#endif  // MMPL_METRIC_H
