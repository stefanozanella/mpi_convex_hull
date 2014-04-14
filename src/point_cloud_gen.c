#include "point_cloud_gen.h"

#include <stdlib.h>

#include <stdio.h>

cloud_size_t parse_cloud_size(const char *num) {
  char *end;
  cloud_size_t ret = strtoul(num, &end, 10);

  // TODO: Error handling
  // TODO: If negative, errno == ERANGE
  if (end != NULL && end[0] != '\0') {
    ret = 0;
  }

  return ret;
}

int parse_args(int argc, const char **argv, options *opts) {
  opts->cloud_size = parse_cloud_size(argv[1]);

  if (argc > 2) {
    opts->dest_file = argv[2];
  }
  else {
    opts->dest_file = "data/cloud.dat";
  }

  // TODO: Error handling

  return 0;
}

void print_usage(const char *prog_name, FILE *outstream) {
  fprintf(outstream, "Usage: %s <size>\n", prog_name);
  fprintf(outstream, "Parameters:\n");
  fprintf(outstream, "  size: number of test points to generate\n");
}

void generate_point_cloud(point_cloud *pc, options opts) {
  pc->size = opts.cloud_size;
  pc->points = (point*) malloc(pc->size * sizeof(point));

  for (int k = 0; k < pc->size; k++) {
    pc->points[k] = opts.point_generator(k, 0);
  }
}

void save_point_cloud(point_cloud *pc, FILE *out_stream) {
  fprintf(out_stream, "# size=%ld\n", pc->size);

  for (int k = 0; k < pc->size; k++) {
    fprintf(out_stream, "%.5f\t%.5f\n", pc->points[k].x, pc->points[k].y);
  }
}

int compare_point(const void *ptr_a, const void *ptr_b) {
  // Note that this operation is a bit risky. If the coordinates held by the
  // points are large enough, multiplication by 10^5 might let them overflow.
  // This should be mitigated by setting a limit on the size of the generated
  // numbers for the point cloud.
  point a = *(point *)ptr_a;
  point b = *(point *)ptr_b;

  int dx = ((int)(a.x * 1e5)) - ((int)(b.x * 1e5));

  return dx != 0 ? dx : ((int)(a.y * 1e5)) - ((int)(b.y * 1e5));
}
