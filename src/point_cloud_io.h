#ifndef POINT_CLOUD_IO_H
#define POINT_CLOUD_IO_H

#include "point_cloud.h"

#include <stdio.h>

#define POINT_OUT_FMT "%ld\t%ld\n"
#define CLOUD_SIZE_FMT "# size=%ld\n"

/**
 * Stores a point cloud in a Gnuplot-compatible text format.
 */
void save_point_cloud(point_cloud *pc, FILE *out_stream);

#endif
