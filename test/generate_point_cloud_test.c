#include <gtest/gtest.h>

#include "point_cloud_gen.h"

TEST(parse_cloud_size, returns_the_integral_representation_of_given_string) {
  EXPECT_EQ(cloud_size_t(123456), parse_cloud_size("123456"));
}

TEST(parse_cloud_size, returns_zero_if_string_is_invalid) {
  EXPECT_EQ(cloud_size_t(0), parse_cloud_size("543.675"));
  EXPECT_EQ(cloud_size_t(0), parse_cloud_size("not_a_number"));
}

TEST(parse_args, stores_the_first_argument_as_cloud_size) {
  options opts;
  const char *args[2];
  args[0] = "program_name";
  args[1] = "345";

  parse_args(1, args, &opts);

  EXPECT_EQ(345, opts.cloud_size);
}

point const_generator(int, coord_t) {
  point ret = { 0, 0 };
  return ret;
}

TEST(generate_point_cloud, generates_a_point_cloud_of_given_size) {
  options opts = { 12, &const_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  ASSERT_EQ(12, test_cloud.size);
}

point incremental_generator(int index, coord_t) {
  point ret = { index, index };
  return ret;
}

TEST(generate_point_cloud, generate_a_point_cloud_using_given_point_generation_function) {
  options opts = { 10, &incremental_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  ASSERT_DOUBLE_EQ(0, test_cloud.points[0].x);
  ASSERT_DOUBLE_EQ(0, test_cloud.points[0].y);

  ASSERT_DOUBLE_EQ(9, test_cloud.points[9].x);
  ASSERT_DOUBLE_EQ(9, test_cloud.points[9].y);
}
