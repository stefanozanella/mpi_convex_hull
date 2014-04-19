#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "generate_point_cloud.h"
#include "point_cloud_io.h"
#include "convex_hull.h"

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

  ASSERT_EQ(0, test_cloud.points[0].x);
  ASSERT_EQ(0, test_cloud.points[0].y);

  ASSERT_EQ(9, test_cloud.points[9].x);
  ASSERT_EQ(9, test_cloud.points[9].y);
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
  ASSERT_STREQ("0\t0\n", line);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("1\t1\n", line);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("2\t2\n", line);

  fclose(out_stream);
}

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

TEST(turn, tells_when_three_points_are_collinear) {
  // We pick three point from the straight line y = 2x + 1
  point p = { 0, 10 };
  point q = { 10, 30 };
  point r = { 20, 50 };

  ASSERT_EQ(TURN_NONE, turn(p, q, r));
}

TEST(turn, tells_when_three_points_make_a_left_turn) {
  // We pick three point from the straight line y = 2x + 1
  point p = { 20, 50 };
  point q = { 30, 70 };
  point r = { 35, 90 };

  ASSERT_EQ(TURN_LEFT, turn(p, q, r));
}

TEST(turn, tells_when_three_points_make_a_right_turn) {
  // We pick three point from the straight line y = 2x + 1
  point p = { 40, 90 };
  point q = { 50, 110 };
  point r = { 65, 130 };

  ASSERT_EQ(TURN_RIGHT, turn(p, q, r));
}

TEST(update_hull, just_adds_given_point_when_the_hull_is_empty) {
  point_cloud hull;
  point p = { 10, 20 };

  init_point_cloud(&hull, 0, 2);

  ASSERT_EQ(0, hull.size);

  update_hull(&hull, p);

  ASSERT_EQ(1, hull.size);
  ASSERT_EQ(10, hull.points[0].x);
  ASSERT_EQ(20, hull.points[0].y);
}

TEST(update_hull, just_adds_given_point_when_the_hull_has_a_single_point) {
  point_cloud hull;
  point p = { 0, 10 };
  point q = { 10, 20 };

  init_point_cloud(&hull, 0, 2);
  push(&hull, p);

  update_hull(&hull, q);

  ASSERT_EQ(2, hull.size);
  ASSERT_EQ(10, hull.points[1].x);
  ASSERT_EQ(20, hull.points[1].y);
}

TEST(update_hull, discards_point_in_the_hull_that_correspond_to_right_turns) {
  point_cloud hull;
  point p = { 0, 0 };
  point q = { 10, 10 };
  point r = { 20, 5 };

  init_point_cloud(&hull, 0, 3);
  push(&hull, p);
  push(&hull, q);

  update_hull(&hull, r);

  ASSERT_EQ(2, hull.size);

  // Assert that p is in the hull
  ASSERT_EQ(0, hull.points[0].x);
  ASSERT_EQ(0, hull.points[0].y);

  // Assert that r is in the hull
  ASSERT_EQ(20, hull.points[1].x);
  ASSERT_EQ(5, hull.points[1].y);
}

TEST(update_hull, just_adds_given_point_if_it_makes_a_left_turn_with_the_latest_hull_edge) {
  point_cloud hull;
  point p = { 0, 0 };
  point q = { 10, -10 };
  point r = { 20, 30 };

  init_point_cloud(&hull, 0, 3);
  push(&hull, p);
  push(&hull, q);

  update_hull(&hull, r);

  ASSERT_EQ(3, hull.size);

  // Assert that p is in the hull
  ASSERT_EQ(0, hull.points[0].x);
  ASSERT_EQ(0, hull.points[0].y);

  // Assert that q is in the hull
  ASSERT_EQ(10, hull.points[1].x);
  ASSERT_EQ(-10, hull.points[1].y);

  // Assert that r is in the hull
  ASSERT_EQ(20, hull.points[2].x);
  ASSERT_EQ(30, hull.points[2].y);
}

TEST(update_hull, can_start_comparisons_from_an_arbitrary_point_in_the_hull) {
  point_cloud hull;
  point p = { 0, 0 };
  point q = { 10, -10 };
  point r = { 20, 30 };
  point s = { 10, 35 };
  point t = { 5, 20 };

  init_point_cloud(&hull, 0, 5);
  push(&hull, p);
  push(&hull, q);
  push(&hull, r);

  update_hull(&hull, s);
  update_hull(&hull, t);

  ASSERT_EQ(5, hull.size);

  // Assert that p is in the hull
  ASSERT_EQ(0, hull.points[0].x);
  ASSERT_EQ(0, hull.points[0].y);

  // Assert that q is in the hull
  ASSERT_EQ(10, hull.points[1].x);
  ASSERT_EQ(-10, hull.points[1].y);

  // Assert that r is in the hull
  ASSERT_EQ(20, hull.points[2].x);
  ASSERT_EQ(30, hull.points[2].y);

  // Assert that s is in the hull
  ASSERT_EQ(10, hull.points[3].x);
  ASSERT_EQ(35, hull.points[3].y);

  // Assert that t is in the hull
  ASSERT_EQ(5, hull.points[4].x);
  ASSERT_EQ(20, hull.points[4].y);
}

TEST(convex_hull_graham_scan, calculates_the_convex_hull_of_a_given_cloud) {
  point_cloud hull, cloud;
  init_point_cloud(&cloud, 0, 10);
  init_point_cloud(&hull, 0, 10);

  // Convex hull
  push(&cloud, (point){ 0, 0 });
  push(&cloud, (point){ 100, -100 });
  push(&cloud, (point){ 200, -50 });
  push(&cloud, (point){ 100, 50 });
  push(&cloud, (point){ 50, 40 });
  // Inner points
  push(&cloud, (point){ 150, -50 });
  push(&cloud, (point){ 120, 10 });
  push(&cloud, (point){ 70, 30 });
  push(&cloud, (point){ 25, 19 });
  push(&cloud, (point){ 50, -30 });

  convex_hull_graham_scan(&cloud, &hull);

  // We save the first point twice both at the start and at the end of the
  // hull. This gives a nice way to visualize the hull in gnuplot with as a
  // complete poloygon.
  EXPECT_EQ(6, hull.size);

  EXPECT_EQ(0, hull.points[0].x);
  EXPECT_EQ(0, hull.points[0].y);

  EXPECT_EQ(100, hull.points[1].x);
  EXPECT_EQ(-100, hull.points[1].y);

  EXPECT_EQ(200, hull.points[2].x);
  EXPECT_EQ(-50, hull.points[2].y);

  EXPECT_EQ(100, hull.points[3].x);
  EXPECT_EQ(50, hull.points[3].y);

  EXPECT_EQ(50, hull.points[4].x);
  EXPECT_EQ(40, hull.points[4].y);
}
