// C++ Standard Library
#include <array>
#include <cstring>
#include <chrono>
#include <iostream>
#include <iomanip>

// TwoD
#include <twod/coordinates.h>
#include <twod/grid.h>
#include <twod/stream.h>

// MMPL
#include <mmpl/planner.h>
#include <mmpl/metric.h>
#include <mmpl/state.h>
#include <mmpl/state_space.h>
#include <mmpl/termination_criteria.h>


using namespace twod;
using namespace mmpl;


class State2D;
class ManhattanDistanceState2D;
class GridStateSpace2D;


namespace mmpl
{

template<>
struct StateTraits<::State2D>
{
  using IDType = std::size_t;
};


template<>
struct MetricTraits<::ManhattanDistanceState2D>
{
  using StateType = State2D;
  using ValueType = int;
};


template<>
struct StateSpaceTraits<::GridStateSpace2D>
{
  using StateType = State2D;
};

}  // namespace mmpl


class State2D : public StateBase<State2D>
{
public:
  State2D(int _x, int _y) :
    indices_{_x, _y}
  {}

  inline operator Indices() const
  {
    return indices_;
  }

private:
  /// Indices
  Indices indices_;

  /**
   * @copydoc StateBase::id
   */
  inline std::size_t CRTP_OVERRIDE_M(id)() const
  {
    static_assert(sizeof(std::size_t) == sizeof(Indices), "Cannot pack Indices into std::size_t");
    std::size_t hash_value = 0;
    std::memcpy(std::addressof(hash_value), std::addressof(indices_), sizeof(Indices));
    return hash_value;
  }

  /**
   * @copydoc StateBase::operator==
   */
  inline bool CRTP_OVERRIDE_M(equals)(const State2D& other) const
  {
    return this->indices_ == other.indices_;
  }

  friend class GridStateSpace2D;

  friend class ManhattanDistanceState2D;

  friend inline std::ostream& operator<<(std::ostream& os, const State2D& state)
  {
    return os << '(' << state.indices_ << ')';
  }

  IMPLEMENT_CRTP_DERIVED_CLASS(StateBase, State2D);
};


class ManhattanDistanceState2D : public MetricBase<ManhattanDistanceState2D>
{
private:
  /**
   * @copydoc MetricBase::get_value
   */
  inline int CRTP_OVERRIDE_M(get_value)(const State2D& parent, const State2D& child) const
  {
    return std::abs(parent.indices_.x - child.indices_.x) +
           std::abs(parent.indices_.y - child.indices_.y);
  }

  IMPLEMENT_CRTP_DERIVED_CLASS(MetricBase, ManhattanDistanceState2D);
};


class GridStateSpace2D : public StateSpaceBase<GridStateSpace2D>
{
public:
  explicit GridStateSpace2D(Extents extents) :
    bounds_{Indices{0, 0}, extents}
  {}

private:
  /// Planning bounds
  Bounds bounds_;

  /**
   * @copydoc StateSpaceBase::for_each_child
   */
  template<typename UnaryChildFn>
  inline bool CRTP_OVERRIDE_M(for_each_child)(const State2D& parent, UnaryChildFn&& child_fn)
  {
    for (int dx : std::array<int, 2>{-1, 1})
    {
      for (int dy : std::array<int, 2>{-1, 1})
      {
        const State2D state{dx + parent.indices_.x, dy + parent.indices_.y};
        if (bounds_.within(state))
        {
          child_fn(state);
        }
      }
    }
    return true;
  }

  IMPLEMENT_CRTP_DERIVED_CLASS(StateSpaceBase, GridStateSpace2D);
};


int main(int argc, char** argv)
{

  const State2D goal{10, 4}, start{3, 5};

  using ExpansionQueueType = MinSortedExpansionQueue<State2D, int>;
  using ExpansionTableType = ExpansionTableOutputStreamHook<UnorderedExpansionTable<State2D, int>>;

  // Create the planner
  ShortestPathPlanner<State2D, int, ExpansionQueueType, ExpansionTableType> planner{
    ExpansionQueueType{},
    ExpansionTableType{std::cout}
  };

  // Create the metric
  ManhattanDistanceState2D metric;

  // Create a state-space representing object
  GridStateSpace2D state_space{Extents{15, 15}};

  // Setup a stopping criteria object
  SingleGoalTerminationCriteria criteria{goal};

  const auto t_start = std::chrono::high_resolution_clock::now();

  // Search until a termination
  const auto [code, iterations] = run_plan(planner, metric, state_space, start, goal);

  // Get approximate time to plan
  using DFloat = std::chrono::duration<float>;
  std::cout << "t plan: " << std::chrono::duration_cast<DFloat>(std::chrono::high_resolution_clock::now() - t_start).count() << std::endl;

  // Show some meta-information
  std::cout << "code  : " << code << std::endl;
  std::cout << "iters : " << iterations << std::endl;
  std::cout << "start : " << start << std::endl;
  std::cout << "goal  : " << goal << std::endl;

  // Reconstruct planning path if goal is found
  std::vector<State2D> path;
  if (code == PlannerCode::GOAL_FOUND)
  {
    generate_reverse_path(std::back_inserter(path), goal, planner.expansion_table());
  }

  // Log about the planned path
  std::cout << "path  : (" << path.size() << " states)" << std::endl;
  for (const auto& state : path)
  {
    std::cout << state;
    if (state == goal)
    {
      std::cout << " goal";
    }
    else if (state == start)
    {
      std::cout << " start";
    }
    std::cout << std::endl;
  }

  return 0;
}
