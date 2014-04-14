#include "point_cloud_gen.h"

#include <stdlib.h>
#include <sysexits.h>

int main(int argc, const char **argv) {
  if (argc < 2) {
    print_usage(argv[0], stderr);
    exit(EX_USAGE);
  }

  options opts;
  parse_args(argc, argv, &opts);

  printf("Point cloud size: %ld\n", opts.cloud_size);

  return 0;
}
