#include <gtest/gtest.h>

#include "point_cloud_gen.h"

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

point const_generator(cloud_size_t, cloud_size_t) {
  point ret = { 0, 0 };
  return ret;
}

TEST(generate_point_cloud, generates_a_point_cloud_of_given_size) {
  options opts = { 12, &const_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  ASSERT_EQ(12, test_cloud.size);
}

point incremental_generator(cloud_size_t index, cloud_size_t) {
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

TEST(save_point_cloud, serializes_a_point_cloud_by_writing_a_header_containing_the_cloud_size) {
  options opts = { 34, &incremental_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  FILE *out_stream = tmpfile();

  save_point_cloud(&test_cloud, out_stream);

  rewind(out_stream);
  char header[256];
  fgets(header, 256, out_stream);

  ASSERT_STREQ("# size=34\n", header);

  fclose(out_stream);
}

TEST(save_point_cloud, serializes_a_point_cloud_by_writing_each_point_on_a_separate_line) {
  options opts = { 3, &incremental_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  FILE *out_stream = tmpfile();

  save_point_cloud(&test_cloud, out_stream);

  rewind(out_stream);
  char line[256];
  // Header, we can discard it for this test
  fgets(line, 256, out_stream);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("0.00000\t0.00000\n", line);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("1.00000\t1.00000\n", line);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("2.00000\t2.00000\n", line);

  fclose(out_stream);
}
