#ifndef MMPL_PLANNER_CODE_OSTREAM_H
#define MMPL_PLANNER_CODE_OSTREAM_H

// C++ Standard Library
#include <ostream>

// MMPL
#include <mmpl/planner_code.h>

namespace mmpl
{

/**
 * @brief Human-readable <code>std::ostream</code> overload for PlannerCode
 */
inline std::ostream& operator<<(std::ostream& os, const PlannerCode code)
{
  switch (static_cast<PlannerCode::Value>(code))
  {
  case PlannerCode::GOAL_FOUND:
    return os << "GOAL_FOUND";
  case PlannerCode::INFEASIBLE:
    return os << "INFEASIBLE";
  case PlannerCode::SEARCHING:
    return os << "SEARCHING";
  default:
    break;
  }
  return os << "PlannerCode<invalid>";
}

}  // namespace mmpl

#endif  // MMPL_PLANNER_CODE_OSTREAM_H
