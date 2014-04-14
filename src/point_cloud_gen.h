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
  point (*point_generator)(int index, coord_t limit);
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

#endif
