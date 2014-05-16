#ifndef MPI_CONVEX_HULL_H
#define MPI_CONVEX_HULL_H

#include <mpi.h>
#include "point_cloud.h"

/**
 * High level application coordination routines
 */
int mpi_convex_hull(int argc, char const **argv);
int convex_hull_master(int argc, char const **argv, int rank, int cpu_count);
int convex_hull_slave(int argc, char const **argv, int rank, int cpu_count);

/**
 * Subtasks routines
 */
void broadcast_cloud_size(cloud_size_t *size);
void scatter_cloud(point_cloud *input_cloud, point_cloud *sub_cloud, int rank, int cpu_count);
void find_leftmost(point *local_point, point *leftmost);
point *find_right_tangent(point_cloud *cloud, point *reference);
point *max_angle(point *x, point *y, point *reference);

/**
 * Communication helpers
 */
MPI_Datatype mpi_point;
MPI_Op MPI_MIN_POINT;
void init_mpi_runtime();
void setup_scatter_params(cloud_size_t input_size, int dest_count, int *sizes, int *offsets);
void mpi_min_point_op(void *invec, void *inoutvec, int *len, MPI_Datatype *type);
point last_point_in_hull;

/**
 * Help screen
 */
void print_usage(const char* prog_name);

#endif
