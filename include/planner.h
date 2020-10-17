

#ifndef MMPL_PLANNER_H
#define MMPL_PLANNER_H

// C++ Standard Library
#include <type_traits>
#include <utility>

// MMPL
#include <mmpl/crtp.h>
#include <mmpl/expansion_queue.h>
#include <mmpl/expansion_table.h>
#include <mmpl/metric.h>
#include <mmpl/state_space.h>
#include <mmpl/termination_criteria.h>
#include <mmpl/planner_code.h>

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
template <typename T> struct PlannerTraits;


template <typename PlannerT> using planner_expansion_table_t = typename PlannerTraits<PlannerT>::ExpansionTableType;


template <typename PlannerT> using planner_expansion_queue_t = typename PlannerTraits<PlannerT>::ExpansionQueueType;


template <typename PlannerT> using planner_state_t = typename PlannerTraits<PlannerT>::StateType;


template <typename PlannerT> using planner_value_t = typename PlannerTraits<PlannerT>::ValueType;


template <typename DerivedT> class PlannerBase
{
public:
  using ExpansionTableType = planner_expansion_table_t<DerivedT>;
  using ExpansionQueueType = planner_expansion_queue_t<DerivedT>;
  using StateType = planner_state_t<DerivedT>;
  using ValueType = planner_value_t<DerivedT>;

  template <typename MetricT, typename StateSpaceT, typename TerminationCriteriaT>
  PlannerCode update(
    MetricBase<MetricT>& metric,
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
    const auto enqueue_valid = [this, &metric, &pred](const StateType& child) {
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
    if (is_terminal(termination_criteria, pred.state))
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

  template <typename TerminationCriteriaT>
  inline bool is_terminal(TerminationCriteriaBase<TerminationCriteriaT>& termination_criteria, const StateType& query)
  {
    if constexpr (TerminationCriteriaTraits<TerminationCriteriaT>::is_expansion_aware)
    {

      return termination_criteria.is_terminal(expansion_table_, query);
    }
    else
    {
      return termination_criteria.is_terminal(query);
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

  inline const ExpansionTableType& expansion_table() const { return expansion_table_; }

  inline const ExpansionQueueType& expansion_queue() const { return expansion_queue_; }

protected:
  template <typename ExpansionQueueT = ExpansionQueueType, typename ExpansionTableT = ExpansionTableType>
  explicit PlannerBase(
    ExpansionQueueT&& queue = ExpansionQueueType{},
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


template <
  typename StateT,
  typename ValueT,
  typename ExpansionQueueT,
  typename ExpansionTableT>
class ShortestPathPlanner : public PlannerBase<ShortestPathPlanner<StateT, ValueT, ExpansionQueueT, ExpansionTableT>>
{
  using PlannerBaseType = PlannerBase<ShortestPathPlanner<StateT, ValueT, ExpansionQueueT, ExpansionTableT>>;

public:
  template <typename... ArgTs>
  explicit ShortestPathPlanner(ArgTs&&... args) : PlannerBaseType{std::forward<ArgTs>(args)...}
  {}
};


template <typename PlannerT, typename MetricT, typename StateSpaceT, typename TerminationCriteriaT>
inline std::pair<PlannerCode, std::size_t> run_plan(
  PlannerBase<PlannerT>& planner,
  MetricBase<MetricT>& metric,
  StateSpaceBase<StateSpaceT>& state_space,
  TerminationCriteriaBase<TerminationCriteriaT>& termination_criteria,
  const planner_state_t<PlannerT>& start,
  const planner_state_t<PlannerT>& goal)
{
  planner.enqueue(start);

  PlannerCode code;
  std::size_t iterations{0};

  while (code == PlannerCode::SEARCHING)
  {
    ++iterations;
    code = planner.update(metric, state_space, termination_criteria);
  }

  return std::make_pair(code, iterations);
}


template <typename PlannerT, typename MetricT, typename StateSpaceT>
inline std::pair<PlannerCode, std::size_t> run_plan(
  PlannerBase<PlannerT>& planner,
  MetricBase<MetricT>& metric,
  StateSpaceBase<StateSpaceT>& state_space,
  const planner_state_t<PlannerT>& start,
  const planner_state_t<PlannerT>& goal)
{
  SingleGoalTerminationCriteria<planner_state_t<PlannerT>> criteria{goal};
  return run_plan(planner, metric, state_space, criteria, start, goal);
}


template <typename StateT, typename ValueT, typename ExpansionQueueT, typename ExpansionTableT>
struct PlannerTraits<ShortestPathPlanner<StateT, ValueT, ExpansionQueueT, ExpansionTableT>>
{
  using StateType = StateT;
  using ValueType = ValueT;
  using ExpansionQueueType = ExpansionQueueT;
  using ExpansionTableType = ExpansionTableT;
};

}  // namespace mmpl

#endif  // MMPL_PLANNER_H
