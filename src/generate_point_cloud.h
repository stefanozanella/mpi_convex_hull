#ifndef GENERATE_POINT_CLOUD_H
#define GENERATE_POINT_CLOUD_H

#include "point_cloud.h"

#include <stdio.h>

typedef struct {
  cloud_size_t cloud_size;
  point (*point_generator)(cloud_size_t index, cloud_size_t size);
  const char *dest_file;
  const char *hull_file;
} options;

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
