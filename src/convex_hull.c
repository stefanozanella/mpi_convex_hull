#include "convex_hull.h"

#include <stdlib.h>
#include <math.h>

turn_t turn(point p, point q, point r) {
  coord_t measure = (q.x - p.x)*(r.y - p.y) - (r.x - p.x)*(q.y - p.y);
  return measure == 0 ? TURN_NONE :
    signbit(measure) ? TURN_RIGHT : TURN_LEFT;
}

void update_hull(point_cloud *hull, point p) {
  while (hull->size > 1 &&
      turn(hull->points[hull->size - 2], hull->points[hull->size - 1], p) != TURN_LEFT) {
    pop(hull);
  }

  // NOTE: Here we are allowing a point to appear more than once in the cloud
  push(hull, p);
}

void convex_hull_graham_scan(point_cloud *cloud, point_cloud *hull) {
  qsort(cloud->points, cloud->size, sizeof(point), &compare_point);

  init_point_cloud(hull, cloud->size + 1);

  for (cloud_size_t k = 0; k < cloud->size; k++) {
    update_hull(hull, cloud->points[k]);
  }

  // Since cloud_size_t is unsigned, we must shift the looped range by +1, then
  // compensating the offset in the actual indexing (since k >= 0 would always
  // be true)
  for (cloud_size_t k = cloud->size ; k > 0; k--) {
    update_hull(hull, cloud->points[k-1]);
  }
}
