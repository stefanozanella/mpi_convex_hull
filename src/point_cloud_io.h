#ifndef POINT_CLOUD_IO_H
#define POINT_CLOUD_IO_H

#include "point_cloud.h"

#include <stdio.h>

#define POINT_FMT "%lld\t%lld\n"
#define CLOUD_SIZE_FMT "# size=%ld\n"

/**
 * Stores a point cloud in a Gnuplot-compatible text format.
 */
void save_point_cloud(point_cloud *pc, FILE *out_stream);

/**
 * Loads a point cloud reading it from the given file.
 */
void load_point_cloud(FILE *in_stream, point_cloud *pc);

#endif
