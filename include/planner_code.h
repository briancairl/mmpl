#ifndef MMPL_PLANNER_CODE_H
#define MMPL_PLANNER_CODE_H

// C++ Standard Library
#include <cstdint>

namespace mmpl
{

/**
 * @brief Planner result code representation
 */
struct PlannerCode
{
  enum Value : std::uint8_t
  {
    GOAL_FOUND,
    INFEASIBLE,
    SEARCHING,
  };

  constexpr PlannerCode(Value _value = Value::SEARCHING) : value{_value} {}

  constexpr operator Value() const { return value; }

  constexpr operator bool() const { return value == Value::GOAL_FOUND; }

  Value value;
};

}  // namespace mmpl

#endif  // MMPL_PLANNER_CODE_H
