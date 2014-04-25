#include "generate_point_cloud.h"

#include <stdlib.h>

cloud_size_t parse_cloud_size(const char *num) {
  char *end;
  cloud_size_t ret = strtoul(num, &end, 10);

  /* TODO: Error handling */
  /* TODO: If negative, errno == ERANGE */
  if (end != NULL && end[0] != '\0') {
    ret = 0;
  }

  return ret;
}

int parse_args(int argc, const char **argv, options *opts) {
  opts->cloud_size = parse_cloud_size(argv[1]);

  if (argc > 2) {
    opts->dest_file = argv[2];
  } else {
    opts->dest_file = "data/cloud.dat";
  }

  if (argc > 3) {
    opts->hull_file = argv[3];
  } else {
    opts->hull_file = "data/hull.dat";
  }

  /* TODO: Error handling */

  return 0;
}

void print_usage(const char *prog_name, FILE *outstream) {
  fprintf(outstream, "Usage: %s <size>\n", prog_name);
  fprintf(outstream, "Parameters:\n");
  fprintf(outstream, "  size: number of test points to generate\n");
}

void generate_point_cloud(point_cloud *pc, options opts) {
  init_point_cloud(pc, opts.cloud_size, opts.cloud_size);

  for (cloud_size_t k = 0; k < pc->size; k++) {
    pc->points[k] = opts.point_generator(k, pc->size);
  }
}
