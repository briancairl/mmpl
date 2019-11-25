
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
  inline int CRTP_OVERRIDE_M(id)() const
  {
    return 1;
  }

  IMPLEMENT_CRTP_DERIVED_CLASS(StateBase, TestState);
};


namespace mmpl
{

template<>
struct StateSpaceTraits<TestStateSpace>
{
  using StateType = TestState;
};

}  // namespace mmpl


class TestStateSpace : public StateSpaceBase<TestStateSpace>
{
public:

private:
  template<typename StateT, typename UnaryFnT>
  inline bool CRTP_OVERRIDE_M(for_each_child)(const StateT& parent, const UnaryFnT& child_fn)
  {
    return true;
  }

  IMPLEMENT_CRTP_DERIVED_CLASS(StateSpaceBase, TestStateSpace);
};

};


TEST(StateSpace, ForEachChild)
{
  TestStateSpace state_space;

  ASSERT_TRUE(state_space.for_each_child(TestState{}, [](const TestState& child) {}));
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
