
// C++ Standard Library
#include <type_traits>

// GTest
#include <gtest/gtest.h>

// TwoD
#include <mmpl/state_space.h>
#include <mmpl/metric.h>

using namespace mmpl;

namespace mmpl
{

class TestStateSpace;
class TestState;

template<>
struct StateTraits<TestState>
{
  using IDType = int;
};


class TestState : public StateBase<TestState>
{
private:
  inline int id_impl() const
  {
    return 1;
  }

  friend class StateBase<TestState>;
};


template<>
struct StateSpaceTraits<TestStateSpace>
{
  using StateType = TestState;
};

class TestStateSpace : public StateSpaceBase<TestStateSpace>
{
public:

private:
  template<typename StateT, typename UnaryFnT>
  inline bool for_each_child_impl(const StateT& parent, const UnaryFnT& child_fn)
  {
    return true;
  }

  friend class StateSpaceBase<TestStateSpace>;
};

}  // namespace mmpl

TEST(StateSpace, ForEachChild)
{
  mmpl::TestStateSpace state_space;

  ASSERT_TRUE(state_space.for_each_child(mmpl::TestState{}, [](const mmpl::TestState& child) {}));
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
