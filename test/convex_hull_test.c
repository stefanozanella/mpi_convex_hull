#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_helper.h"
#include "convex_hull.h"

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

  EXPECT_EQ(5, hull.size);

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
