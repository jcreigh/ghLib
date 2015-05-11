#include "Util.h"
#include "gtest/gtest.h"

bool IsWithin(double n, double expected, double tolerance) {
  return std::abs(n - expected) < tolerance;
}

TEST(Util, Coerce) {
  ASSERT_EQ(ghLib::Coerce(-2, -1, 1), -1);
  ASSERT_EQ(ghLib::Coerce(50, -5, 5), 5);
  ASSERT_EQ(ghLib::Coerce(-3.0, -2.0, -1.0), -2.0);
  ASSERT_EQ(ghLib::Coerce(127, 0, 255), 127);
  ASSERT_EQ(ghLib::Coerce(0.5, 0.0, 1.0), 0.5);
  ASSERT_EQ(ghLib::Coerce(0, 0, 0), 0);
  ASSERT_EQ(ghLib::Coerce(0, -100, -1), -1);
  ASSERT_EQ(ghLib::Coerce(-1, -1, 10000), -1);
}

TEST(Util, Interpolate) {
  ASSERT_EQ(ghLib::Interpolate(-2.0, -1.0, 1.0, 0, 255), 0);
  ASSERT_EQ(ghLib::Interpolate(0.0, -1.0, 1.0, 0, 255), 127);
  ASSERT_TRUE(IsWithin(ghLib::Interpolate(127, 0, 255, -1.0, 1.0), 0.0, 0.01));
  ASSERT_TRUE(IsWithin(ghLib::Interpolate(127, 0, 255, 0.0, 1.0), 0.5, 0.01));
}

TEST(Util, Deadband) {
  ASSERT_EQ((int)(100 * ghLib::Deadband(-2.0, 0.15)), -100);
  ASSERT_EQ((int)(100 * ghLib::Deadband(-1.0, 0.15)), -100);
  ASSERT_EQ((int)(100 * ghLib::Deadband(-0.5, 0.15)), -41);
  ASSERT_EQ((int)(100 * ghLib::Deadband(-0.2, 0.15)), -5);
  ASSERT_EQ((int)(100 * ghLib::Deadband(-0.15, 0.15)), 0);
  ASSERT_EQ((int)(100 * ghLib::Deadband(-0.1, 0.15)), 0);
  ASSERT_EQ((int)(100 * ghLib::Deadband(-0.005, 0.15)), 0);

  ASSERT_EQ((int)(100 * ghLib::Deadband(2.0, 0.15)), 100);
  ASSERT_EQ((int)(100 * ghLib::Deadband(1.0, 0.15)), 100);
  ASSERT_EQ((int)(100 * ghLib::Deadband(0.5, 0.15)), 41);
  ASSERT_EQ((int)(100 * ghLib::Deadband(0.2, 0.15)), 5);
  ASSERT_EQ((int)(100 * ghLib::Deadband(0.15, 0.15)), 0);
  ASSERT_EQ((int)(100 * ghLib::Deadband(0.1, 0.15)), 0);
  ASSERT_EQ((int)(100 * ghLib::Deadband(0.005, 0.15)), 0);

  ASSERT_EQ((int)(100 * ghLib::Deadband(0.0, 0.15)), 0);
}
