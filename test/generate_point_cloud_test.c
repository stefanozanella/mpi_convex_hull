#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_helper.h"
#include "generate_point_cloud.h"

TEST(parse_cloud_size, returns_the_integral_representation_of_given_string) {
  ASSERT_EQ(cloud_size_t(123456), parse_cloud_size("123456"));
}

TEST(parse_cloud_size, returns_zero_if_string_is_invalid) {
  ASSERT_EQ(cloud_size_t(0), parse_cloud_size("543.675"));
  ASSERT_EQ(cloud_size_t(0), parse_cloud_size("not_a_number"));
}

TEST(parse_args, stores_the_1st_argument_as_cloud_size) {
  options opts;
  const char *args[2];
  args[0] = "program_name";
  args[1] = "345";

  parse_args(2, args, &opts);

  ASSERT_EQ(345, opts.cloud_size);
}

TEST(parse_args, stores_the_2nd_argument_as_dest_file) {
  options opts;
  const char *args[3];
  args[0] = "program_name";
  args[1] = "345";
  args[2] = "path/to/out/file.dat";

  parse_args(3, args, &opts);

  ASSERT_STREQ("path/to/out/file.dat", opts.dest_file);
}

TEST(parse_args, sets_a_default_dest_file_if_none_specified) {
  options opts;
  const char *args[2];
  args[0] = "program_name";
  args[1] = "345";

  parse_args(2, args, &opts);

  ASSERT_STREQ("data/cloud.dat", opts.dest_file);
}

TEST(parse_args, stores_the_3rd_argument_as_hull_file) {
  options opts;
  const char *args[4];
  args[0] = "program_name";
  args[1] = "345";
  args[2] = "path/to/out/file.dat";
  args[3] = "path/to/hull/output.dat";

  parse_args(4, args, &opts);

  ASSERT_STREQ("path/to/hull/output.dat", opts.hull_file);
}

TEST(parse_args, sets_a_default_hull_file_if_none_specified) {
  options opts;
  const char *args[3];
  args[0] = "program_name";
  args[1] = "345";
  args[2] = "path/to/out/file.dat";

  parse_args(3, args, &opts);

  ASSERT_STREQ("data/hull.dat", opts.hull_file);
}

TEST(generate_point_cloud, generates_a_point_cloud_of_given_size) {
  options opts = { 12, &const_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  ASSERT_EQ(12, test_cloud.size);
}

TEST(generate_point_cloud, generate_a_point_cloud_using_given_point_generation_function) {
  options opts = { 10, &incremental_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  ASSERT_EQ(0, test_cloud.points[0].x);
  ASSERT_EQ(0, test_cloud.points[0].y);

  ASSERT_EQ(9, test_cloud.points[9].x);
  ASSERT_EQ(9, test_cloud.points[9].y);
}
