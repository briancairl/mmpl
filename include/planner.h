

#ifndef MMPL_PLANNER_H
#define MMPL_PLANNER_H

// C++ Standard Library
#include <initializer_list>
#include <ostream>
#include <type_traits>

// CRTP
#include <crtp/crtp.h>

// MMPL
#include <mmpl/expansion_queue.h>
#include <mmpl/expansion_table.h>
#include <mmpl/metric.h>
#include <mmpl/state_space.h>
#include <mmpl/termination_criteria.h>

namespace mmpl
{

/**
 * @brief PlannerBase type information
 *
 *        Requires the following member types:
 *        - ExpansionTableType (derived from ExpansionTableBase)
 *        - ExpansionQueueType (derived from ExpansionQueueBase)
 *        - ValueType (derived from MetricValueBase)
 *        - StateType (derived from StateBase)
 */
template<typename T>
struct PlannerTraits;


template<typename PlannerT>
using planner_expansion_table_t = typename PlannerTraits<PlannerT>::ExpansionTableType;


template<typename PlannerT>
using planner_expansion_queue_t = typename PlannerTraits<PlannerT>::ExpansionQueueType;


template<typename PlannerT>
using planner_state_t = typename PlannerTraits<PlannerT>::StateType;


template<typename PlannerT>
using planner_value_t = typename PlannerTraits<PlannerT>::ValueType;


struct PlannerCode
{
  enum Value : std::int8_t
  {
    GOAL_FOUND,
    INFEASIBLE,
    SEARCHING,
  };

  constexpr PlannerCode(Value _value = Value::SEARCHING) :
    value{_value}
  {}

  constexpr operator Value() const
  {
    return value;
  }

  constexpr operator bool() const
  {
    return value == Value::GOAL_FOUND;
  }

  Value value;
};


template<typename DerivedT>
class PlannerBase
{
public:
  using ExpansionTableType = planner_expansion_table_t<DerivedT>;
  using ExpansionQueueType = planner_expansion_queue_t<DerivedT>;
  using StateType = planner_state_t<DerivedT>;
  using ValueType = planner_value_t<DerivedT>;

  template<typename MetricT, typename StateSpaceT, typename TerminationCriteriaT>
  PlannerCode update(MetricBase<MetricT>& metric,
                     StateSpaceBase<StateSpaceT>& state_space,
                     TerminationCriteriaBase<TerminationCriteriaT>& termination_criteria)
  {
    // Abort if there is nothing left in the queue
    if (expansion_queue_.empty())
    {
      return PlannerCode::INFEASIBLE;
    }

    // Get previous search predecessor
    const auto pred = expansion_queue_.next();

    // Enqueue next states from active parent
    const auto enqueue_valid =
      [this, &metric, &pred](const StateType& child)
      {
        // Dont enqueue if already expanded
        if (expansion_table_.is_expanded(child))
        {
          return;
        }

        // Get cost from start to child
        const ValueType next_total_value = pred.value + metric(pred.state, child);

        // Update expansion information
        if (expansion_table_.expand(pred.state, child, next_total_value))
        {
          expansion_queue_.enqueue(child, next_total_value);
        }
      };

    // Check if search is terminated
    if (termination_criteria.is_terminal(pred.state))
    {
      return PlannerCode::GOAL_FOUND;
    }
    else if (state_space.for_each_child(pred.state, enqueue_valid))
    {
      return PlannerCode::SEARCHING;
    }
    else
    {
      return PlannerCode::INFEASIBLE;
    }
  }

  inline void reset()
  {
    expansion_queue_.reset();
    expansion_table_.reset();
  }

  inline void enqueue(const StateType& state)
  {
    expansion_queue_.enqueue(state, Null<ValueType>::value);
    expansion_table_.expand(state, state, Null<ValueType>::value);
  }

  inline const ExpansionTableType& expansion_table() const
  {
    return expansion_table_;
  }

  inline const ExpansionQueueType& expansion_queue() const
  {
    return expansion_queue_;
  }

protected:
  template<typename ExpansionQueueT = ExpansionQueueType,
           typename ExpansionTableT = ExpansionTableType>
  explicit PlannerBase(ExpansionQueueT&& queue = ExpansionQueueType{},
                       ExpansionTableT&& expansion = ExpansionTableType{}) :
    expansion_queue_{std::forward<ExpansionQueueT>(queue)},
    expansion_table_{std::forward<ExpansionTableT>(expansion)}
  {}

  /// Expansion queue
  ExpansionQueueType expansion_queue_;

  /// Expanded state lookup
  ExpansionTableType expansion_table_;


private:
  IMPLEMENT_CRTP_BASE_CLASS(PlannerBase, DerivedT);
};


template<typename StateT,
         typename ValueT,
         typename ExpansionQueueT = MinSortedExpansionQueue<StateT, ValueT>,
         typename ExpansionTableT = UnorderedExpansionTable<StateT, ValueT>>
class ShortestPathPlanner : public PlannerBase<ShortestPathPlanner<StateT, ValueT, ExpansionQueueT, ExpansionTableT>>
{
public:
  template<typename... ArgTs>
  explicit ShortestPathPlanner(ArgTs&&... args) :
    PlannerBaseType{std::forward<ArgTs>(args)...}
  {}

  template<typename... ArgTs>
  explicit ShortestPathPlanner(const std::initializer_list<StateT>& start_states, ArgTs&&... args) :
    ShortestPathPlanner{std::forward<ArgTs>(args)...}
  {
    for (const auto& s : start_states)
    {
      PlannerBaseType::enqueue(s);
    }
  }

private:
  IMPLEMENT_CRTP_DERIVED_CLASS(PlannerBase, ShortestPathPlanner);
};


template<typename StateT, typename ValueT, typename ExpansionQueueT, typename ExpansionTableT>
struct PlannerTraits<ShortestPathPlanner<StateT, ValueT, ExpansionQueueT, ExpansionTableT>>
{
  using StateType = StateT;
  using ValueType = ValueT;
  using ExpansionQueueType = ExpansionQueueT;
  using ExpansionTableType = ExpansionTableT;
};


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

#endif  // MMPL_PLANNER_H

