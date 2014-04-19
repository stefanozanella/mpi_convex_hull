#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_helper.h"
#include "point_cloud.h"

TEST(compare_point, tells_when_two_points_are_equal) {
  point a = { 123456, 789012 };
  point b = { 123456, 789012 };

  ASSERT_EQ(0, compare_point(&a, &b));
}

TEST(compare_point, tells_when_the_first_point_is_less_than_the_second_one) {
  point a = { 123456, 789012 };
  point b = { 123458, 789012 };
  point c = { 123456, 789017 };

  ASSERT_LT(compare_point(&a, &b), 0);
  ASSERT_LT(compare_point(&a, &c), 0);
}

TEST(compare_point, tells_when_the_first_point_is_greater_than_the_second_one) {
  point a = { 456789, 312345 };
  point b = { 456784, 312345 };
  point c = { 456789, 312343 };

  ASSERT_GT(compare_point(&a, &b), 0);
  ASSERT_GT(compare_point(&a, &c), 0);
}
