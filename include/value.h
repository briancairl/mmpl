#ifndef MMPL_VALUE_H
#define MMPL_VALUE_H

// C++ Standard Library
#include <limits>
#include <type_traits>

namespace mmpl
{

/**
 * @brief Traits object used to check if <code>ValueT</code> is usable is a planner value type
 */
template<typename ValueT>
struct is_value :
  std::integral_constant<bool, std::is_integral<ValueT>::value or std::is_floating_point<ValueT>::value> {};


/**
 * @brief Special object used to represent a NULL-value for a given <code>ValueT</code>
 */
template<typename ValueT>
struct Null
{
  static constexpr ValueT value = static_cast<ValueT>(0);
};


/**
 * @brief Special object used to represent an invalid value for a given <code>ValueT</code>
 */
template<typename ValueT>
struct Invalid
{
  static constexpr ValueT value = std::numeric_limits<ValueT>::max();
};


/**
 * @brief Compound value type representing a value + a heuristic value
 *
 *        The purpose of this compound object template is for support of planning guided
 *        by an an evaluation function which includes a hueristic value of the form:
 *        \f$f(x) = g(x) + h(x)\f$ where \f$g(x)\f$ is the "true" value from the state \f$x\f$
 *        to a goal state, and \f$h(x)\f$ is the estimated (heuristic) value from the state \f$x\f$
 *        to a goal state.
 */
template<typename ValueT, typename HeuristicT = ValueT>
class HeuristicValue
{
public:
  static_assert(is_value<ValueT>(), "ValueT must be a valid metric value type");
  static_assert(is_value<HeuristicT>(), "HeuristicT must be a valid metric value type");

  /**
   * @brief Intialization constructor
   */
  constexpr HeuristicValue(ValueT _g_value, HeuristicT _h_value) :
    value{_g_value + _h_value},
    heuristic{_h_value}
  {}

  constexpr operator ValueT() const
  {
    return this->g();
  }

  constexpr ValueT g() const
  {
    return this->value - this->heuristic;
  }

  constexpr ValueT f() const
  {
    return this->value;
  }

  constexpr ValueT h() const
  {
    return this->heuristic;
  }

  constexpr bool operator<(const HeuristicValue& other) const
  {
    return this->value < other.value;
  }

  constexpr bool operator>(const HeuristicValue& other) const
  {
    return this->value > other.value;
  }

  constexpr bool operator==(const HeuristicValue& other) const
  {
    return this->value == other.value;
  }

  constexpr bool operator!=(const HeuristicValue& other) const
  {
    return this->value != other.value;
  }

  constexpr HeuristicValue operator-(int) const
  {
    return HeuristicValue{-this->value, -this->heuristic};
  }

  constexpr const HeuristicValue& operator+=(const HeuristicValue& other)
  {
    this->value += other.value;
    this->heuristic += other.heuristic;
    return *this;
  }

  constexpr const HeuristicValue& operator-=(const HeuristicValue& other)
  {
    this->value -= other.value;
    this->heuristic -= other.heuristic;
    return *this;
  }

  template<typename ScalarT>
  constexpr const HeuristicValue& operator*=(const ScalarT& scaling)
  {
    this->value *= scaling;
    this->heuristic *= scaling;
    return *this;
  }

  friend constexpr HeuristicValue operator+(const HeuristicValue& lhs, const HeuristicValue& rhs)
  {
    return HeuristicValue{construct_raw, lhs.value + rhs.value, lhs.heuristic + rhs.heuristic};
  }

  friend constexpr HeuristicValue operator-(const HeuristicValue& lhs, const HeuristicValue& rhs)
  {
    return HeuristicValue{construct_raw, lhs.value - rhs.value, lhs.heuristic - rhs.heuristic};
  }

  template<typename ScalarT>
  friend constexpr HeuristicValue operator*(const HeuristicValue& lhs, const ScalarT& scaling)
  {
    return HeuristicValue{construct_raw,
                          static_cast<ValueT>(lhs.value * scaling),
                          static_cast<HeuristicT>(lhs.heuristic * scaling)};
  }


  template<typename ScalarT>
  friend constexpr HeuristicValue operator*(const ScalarT& scaling, const HeuristicValue& rhs)
  {
    return HeuristicValue{construct_raw,
                          static_cast<ValueT>(rhs.value * scaling),
                          static_cast<HeuristicT>(rhs.heuristic * scaling)};
  }

private:
  /// Raw value construction tag type
  struct construct_raw_t {};

  /// Raw value constructor tag value
  static constexpr construct_raw_t construct_raw{};

  /**
   * @brief Used to assign internal fields as-is
   */
  constexpr HeuristicValue(construct_raw_t, ValueT _value, HeuristicT _heuristic) :
    value{_value},
    heuristic{_heuristic}
  {}

  /// True value + hueristic value
  ValueT value;

  /// Hueristic value
  HeuristicT heuristic;
};


template<typename ValueT, typename HeuristicT>
struct is_value<HeuristicValue<ValueT, HeuristicT>> :
  std::integral_constant<bool, is_value<ValueT>::value and is_value<HeuristicT>::value> {};


template<typename ValueT, typename HeuristicT>
struct Null<HeuristicValue<ValueT, HeuristicT>>
{
  static constexpr HeuristicValue<ValueT, HeuristicT> value{Null<ValueT>::value, Null<HeuristicT>::value};
};


template<typename ValueT, typename HeuristicT>
struct Invalid<HeuristicValue<ValueT, HeuristicT>>
{
  static constexpr HeuristicValue<ValueT, HeuristicT> value{Invalid<ValueT>::value, Invalid<HeuristicT>::value};
};

}  // namespace mmpl

#endif  // MMPL_VALUE_H
