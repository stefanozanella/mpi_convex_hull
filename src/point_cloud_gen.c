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
