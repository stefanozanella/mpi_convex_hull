#include "point_cloud_gen.h"

#include <stdlib.h>
#include <sysexits.h>

point random_point_generator(cloud_size_t index, cloud_size_t size) {
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

  printf("Generating point cloud of size: %ld\n", opts.cloud_size);

  point_cloud pc;
  generate_point_cloud(&pc, opts);

  printf("Storing point cloud into %s\n", opts.dest_file);
  FILE *out;
  if ((out = fopen(opts.dest_file, "w")) == NULL) {
    // TODO: Error handling
    printf("Error opening file %s. Aborting.\n", opts.dest_file);
    exit(EX_IOERR);
  }

  save_point_cloud(&pc, out);
  fclose(out);

  printf("Calculating convex hull for generated cloud.\n");
  point_cloud hull;
  convex_hull_graham_scan(&pc, &hull);

  printf("Storing convex hull into %s\n", opts.hull_file);
  FILE *hull_out;
  if ((hull_out = fopen(opts.hull_file, "w")) == NULL) {
    // TODO: Error handling
    printf("Error opening file %s. Aborting.\n", opts.hull_file);
    exit(EX_IOERR);
  }

  save_point_cloud(&hull, hull_out);
  fclose(hull_out);

  return EX_OK;
}
