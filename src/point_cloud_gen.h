#ifndef GENERATE_POINT_CLOUD_H
#define GENERATE_POINT_CLOUD_H

#include <stdio.h>

typedef unsigned long cloud_size_t;
typedef long coord_t;

typedef struct {
  coord_t x, y;
} point;

typedef struct {
  cloud_size_t cloud_size;
  point (*point_generator)(cloud_size_t index, cloud_size_t size);
  const char *dest_file;
  const char *hull_file;
} options;

typedef struct {
  point *points;
  cloud_size_t size;
} point_cloud;

/**
 * Parses raw command line options, transforming them into their proper format
 * and storing them into a meaningful structure.
 */
int parse_args(int argc, const char **argv, options *opts);

cloud_size_t parse_cloud_size(const char *num);

/**
 * Initializes a new point cloud with given max length
 */
void init_point_cloud(point_cloud *pc, int max_length);

/**
 * Generates a pseudo-random point cloud.
 */
void generate_point_cloud(point_cloud *pc, options opts);

/**
 * Prints help screen.
 */
void print_usage(const char *prog_name, FILE *outstream);

/**
 * Stores a point cloud in a Gnuplot-compatible text format.
 */
void save_point_cloud(point_cloud *pc, FILE *out_stream);

/**
 * Compares two points lexicographically
 */
int compare_point(const void *a, const void *b);

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
 * Utility functions that manipulate a point cloud as if it was a stack.
 */
void push(point_cloud *hull, point p);
void pop(point_cloud *hull);

/**
 * Calculates the convex hull of a given point cloud using the Graham's Scan
 * algorithm.
 */
void convex_hull_graham_scan(point_cloud *cloud, point_cloud *hull);

#endif
