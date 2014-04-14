#ifndef GENERATE_POINT_CLOUD_H
#define GENERATE_POINT_CLOUD_H

#include <stdio.h>

typedef unsigned long cloud_size_t;
typedef double coord_t;

typedef struct {
  coord_t x, y;
} point;

typedef struct {
  cloud_size_t cloud_size;
  point (*point_generator)(cloud_size_t index, cloud_size_t size);
  const char *dest_file;
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

#endif
