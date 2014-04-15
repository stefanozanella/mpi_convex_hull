#include "point_cloud_gen.h"

#include <stdlib.h>
#include <sysexits.h>
#include <math.h>
#include <time.h>

void init_cloud_generation() {
  srand(time(NULL));
}

coord_t rand_coord(coord_t limit) {
  coord_t safe_limit = limit == 0 ? 1l : limit;

  return ((float) rand() / (float) RAND_MAX - 0.5) * safe_limit * 1e2;
}

point random_point(coord_t limit) {
  return (point) { rand_coord(limit), rand_coord(limit) };
}

/*
 * If we allow each point to be picked in the same range, as the number of
 * points increase, the final shape will strongly resemble a square. Instead,
 * if we partition the number of points so that each subset of points is picked
 * from a range that is larger/smaller than the previous ones, we will obtain a
 * more randomic shape. In particular, we'll have less chances to have
 * collinear points on the convex hull, and its shape will be more fuzzy.
 * To improve the fuzziness around the edges, we'll use a logarithmic function;
 * the choice of the exact expression is rather customary and has been done on
 * an inspection basis (looking at the final picture of the point cloud).
 */
point random_point_generator(cloud_size_t index, cloud_size_t size) {
  return random_point(1.0 - log((1.0 + index/1e2) / (size/1e2)));
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

  init_cloud_generation();
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
