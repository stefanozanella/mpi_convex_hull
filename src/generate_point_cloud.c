#include "point_cloud_gen.h"

#include <stdlib.h>
#include <sysexits.h>

point random_point_generator(int index, coord_t limit) {
  point p = { 0, 0 };

  return p;
}

int main(int argc, const char **argv) {
  if (argc < 2) {
    print_usage(argv[0], stderr);
    exit(EX_USAGE);
  }

  options opts;
  parse_args(argc, argv, &opts);
  opts.point_generator = &random_point_generator;

  printf("Point cloud size: %ld\n", opts.cloud_size);

  point_cloud pc;
  generate_point_cloud(&pc, opts);

  return 0;
}
