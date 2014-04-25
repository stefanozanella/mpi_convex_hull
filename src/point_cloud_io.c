#include "point_cloud_io.h"

void save_point_cloud(point_cloud *pc, FILE *out_stream) {
  fprintf(out_stream, CLOUD_SIZE_FMT, pc->size);

  for (cloud_size_t k = 0; k < pc->size; k++) {
    fprintf(out_stream, POINT_FMT, pc->points[k].x, pc->points[k].y);
  }

  /* Here we replicate the first point so that we can plot a closed polyline in
   * Gnuplot. This last point is actually ignored when reading it back
   */
  fprintf(out_stream, POINT_FMT, pc->points[0].x, pc->points[0].y);
}

void load_point_cloud(FILE *in_stream, point_cloud *pc) {
  cloud_size_t size = 0;
  fscanf(in_stream, CLOUD_SIZE_FMT, &size);

  init_point_cloud(pc, size, size);

  for (cloud_size_t k = 0; k < pc->size; k++) {
    point pt;
    fscanf(in_stream, POINT_FMT, &pt.x, &pt.y);
    pc->points[k] = pt;
  }
}
