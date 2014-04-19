#include "point_cloud.h"

#include <stdlib.h>

void init_point_cloud(point_cloud *pc, int max_length) {
  pc->size = 0;
  pc->points = (point*) malloc(max_length * sizeof(point));
}

int compare_point(const void *ptr_a, const void *ptr_b) {
  point a = *(point *)ptr_a;
  point b = *(point *)ptr_b;

  coord_t dx = a.x - b.x;
  return dx ? dx : a.y - b.y;
}

void push(point_cloud *cloud, point p) {
  cloud->points[cloud->size++] = p;
}

void pop(point_cloud *cloud) {
  cloud->size--;
}
