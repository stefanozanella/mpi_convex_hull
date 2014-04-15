#include "point_cloud_gen.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
  } else {
    opts->dest_file = "data/cloud.dat";
  }

  if (argc > 3) {
    opts->hull_file = argv[3];
  } else {
    opts->hull_file = "data/hull.dat";
  }

  // TODO: Error handling

  return 0;
}

void print_usage(const char *prog_name, FILE *outstream) {
  fprintf(outstream, "Usage: %s <size>\n", prog_name);
  fprintf(outstream, "Parameters:\n");
  fprintf(outstream, "  size: number of test points to generate\n");
}

void init_point_cloud(point_cloud *pc, int max_length) {
  pc->size = 0;
  pc->points = (point*) malloc(max_length * sizeof(point));
}

void generate_point_cloud(point_cloud *pc, options opts) {
  pc->size = opts.cloud_size;
  pc->points = (point*) malloc(pc->size * sizeof(point));

  for (int k = 0; k < pc->size; k++) {
    pc->points[k] = opts.point_generator(k, pc->size);
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

turn_t turn(point p, point q, point r) {
  // NOTE: This might need some optimization, there might be a cleaner way to
  // do this...
  // NOTE: I'm not 100% sure comparing against 0 is a good idea. Previously, I
  // was using fpclassify(), but its implementation seems to just perform the
  // same check to see if a number is 0...
  // HINT: We might not even need to return fixed values from the enum. We
  // might just compare the result against 0 outside of the function.
  coord_t measure = (q.x - p.x)*(r.y - p.y) - (r.x - p.x)*(q.y - p.y);
  return measure == 0 ? TURN_NONE :
    signbit(measure) ? TURN_RIGHT : TURN_LEFT;
}

void update_hull(point_cloud *hull, point p) {
  while (hull->size > 1 &&
      turn(hull->points[hull->size - 2], hull->points[hull->size - 1], p) != TURN_LEFT) {
    pop(hull);
  }

  // NOTE: Here we are allowing a point to appear more than once in the cloud
  push(hull, p);
}

void push(point_cloud *hull, point p) {
  hull->points[hull->size++] = p;
}

void pop(point_cloud *hull) {
  hull->size--;
}

void convex_hull_graham_scan(point_cloud *cloud, point_cloud *hull) {
  qsort(cloud->points, cloud->size, sizeof(point), &compare_point);

  init_point_cloud(hull, cloud->size);

  for (int k = 0; k < cloud->size; k++) {
    update_hull(hull, cloud->points[k]);
  }

  for (int k = cloud->size - 2; k > 0; k--) {
    update_hull(hull, cloud->points[k]);
  }
}
