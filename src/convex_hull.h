#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H

#include "point_cloud_io.h"

/**
 * Tells which direction a point r is with respect to the line passing between
 * the two other points p and q.
 */
typedef enum {
  TURN_RIGHT = -1,
  TURN_NONE = 0,
  TURN_LEFT = 1,
} turn_t;

turn_t turn(point p, point q, point r);

/**
 * Adds given point to an existing convex hull, discarding all points that
 * fall inside the new convex hull. The function assumes that the point are
 * ordered lexicographically.
 */
void update_hull(point_cloud *hull, point p);

/**
 * Calculates the convex hull of a given point cloud using the Graham's Scan
 * algorithm.
 */
void convex_hull_graham_scan(point_cloud *cloud, point_cloud *hull);

#endif
