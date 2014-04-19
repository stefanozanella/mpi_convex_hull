#ifndef POINT_CLOUD_H
#define POINT_CLOUD_H

typedef unsigned long cloud_size_t;
typedef long coord_t;

/**
 * Point, represented by a pair of coordinates
 */
typedef struct {
  coord_t x, y;
} point;

/**
 * Data structure holding the representation of a point cloud
 */
typedef struct {
  point *points;
  cloud_size_t size;
} point_cloud;

/**
 * Initializes a new point cloud with given max length
 */
void init_point_cloud(point_cloud *pc, int max_length);

/**
 * Compares two points lexicographically
 */
int compare_point(const void *a, const void *b);

/**
 * Utility functions that manipulate a point cloud as if it was a stack.
 */
void push(point_cloud *cloud, point p);
void pop(point_cloud *cloud);

#endif
