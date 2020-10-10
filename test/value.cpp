
// C++ Standard Library
#include <limits>

// GTest
#include <gtest/gtest.h>

// TwoD
#include <mmpl/value.h>


using namespace mmpl;


TEST(SignedIntValue, Null) { ASSERT_EQ(Null<int>::value, static_cast<int>(0)); }


TEST(SignedIntValue, Invalid) { ASSERT_EQ(Invalid<int>::value, std::numeric_limits<int>::max()); }


TEST(UnsignedIntValue, Null) { ASSERT_EQ(Null<unsigned>::value, static_cast<unsigned>(0)); }


TEST(UnsignedIntValue, Invalid) { ASSERT_EQ(Invalid<unsigned>::value, std::numeric_limits<unsigned>::max()); }


TEST(FloatValue, Null) { ASSERT_EQ(Null<float>::value, static_cast<float>(0)); }


TEST(FloatValue, Invalid) { ASSERT_EQ(Invalid<float>::value, std::numeric_limits<float>::max()); }


TEST(HeuristicValue, Null)
{
  static constexpr auto v = Null<HeuristicValue<float, int>>::value;
  ASSERT_EQ(v.f(), static_cast<float>(0));
  ASSERT_EQ(v.h(), static_cast<int>(0));
}


TEST(HeuristicValue, Invalid)
{
  static constexpr auto v = Invalid<HeuristicValue<float, int>>::value;
  ASSERT_EQ(v.f(), std::numeric_limits<float>::max());
  ASSERT_EQ(v.h(), std::numeric_limits<int>::max());
}


TEST(HeuristicValue, Add)
{
  const auto result = HeuristicValue<float, int>{1.f, 2} + HeuristicValue<float, int>{3.f, 4};

  ASSERT_EQ(result.f(), 10.f);
  ASSERT_EQ(result.h(), 6);
  ASSERT_EQ(result.g(), 4.f);
}


TEST(HeuristicValue, Subtract)
{
  const auto result = HeuristicValue<float, int>{1.f, 2} - HeuristicValue<float, int>{3.f, 4};

  ASSERT_EQ(result.f(), -4.f);
  ASSERT_EQ(result.h(), -2);
  ASSERT_EQ(result.g(), -2.f);
}


TEST(HeuristicValue, LHSScaling)
{
  const auto result = HeuristicValue<float, int>{1.f, 2} * 2;

  ASSERT_EQ(result.f(), 6.f);
  ASSERT_EQ(result.h(), 4);
  ASSERT_EQ(result.g(), 2.f);
}


TEST(HeuristicValue, RHSScaling)
{
  const auto result = 3 * HeuristicValue<float, int>{1.f, 2};

  ASSERT_EQ(result.f(), 9.f);
  ASSERT_EQ(result.h(), 6);
  ASSERT_EQ(result.g(), 3.f);
}


TEST(HeuristicValue, CompoundAdd)
{
  HeuristicValue<float, int> v{5.f, 2};

  v += HeuristicValue<float, int>{1.f, 2};

  ASSERT_EQ(v.f(), 10.f);
  ASSERT_EQ(v.h(), 4);
  ASSERT_EQ(v.g(), 6.f);
}


TEST(HeuristicValue, CompoundSubtract)
{
  HeuristicValue<float, int> v{5.f, 2};

  v -= HeuristicValue<float, int>{1.f, 2};

  ASSERT_EQ(v.f(), 4.f);
  ASSERT_EQ(v.h(), 0);
  ASSERT_EQ(v.g(), 4.f);
}


TEST(HeuristicValue, CompoundScaling)
{
  HeuristicValue<float, int> v{5.f, 2};

  v *= 3;

  ASSERT_EQ(v.f(), 21.f);
  ASSERT_EQ(v.h(), 6);
  ASSERT_EQ(v.g(), 15.f);
}


int main(int argc, char** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
