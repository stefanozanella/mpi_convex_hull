#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "point_cloud_io.h"
#include "convex_hull.h"

#define MAX_CPUS 256

/*
 * Represents a point in MPI communications
 */
MPI_Datatype mpi_point_t;

/*
 * Collective minimum reduction of a scattered set of points
 */
MPI_Op MPI_POINT_MIN;

/*
 * Wraps MPI initialization/finalization code, so that independent apps don't
 * need to take care about that boilerplate.
 */
int exec_mpi_app(int argc, char* argv[]);

/*
 * MPI utility functions for custom data types and the likes
 */
void mpi_initialize_runtime();
void mpi_init_point_type();
void mpi_init_point_min_op();
void mpi_point_min_op(void *invec, void *inoutvec, int *len, MPI_Datatype *datatype);

/*
 * MPI Convex Hull
 */
int mpi_convex_hull(int argc, char* argv[]);
int ch_master(int argc, char* argv[], int rank, int cpu_count);
int ch_slave(int argc, char* argv[], int rank, int cpu_count);
void bcast_point_cloud_size(int* size);
void setup_scatter_params(int array_size, int dest_count, int *sizes, int *offsets);
point_cloud_t chan_step_1(point_cloud_t pc, int rank, int cpu_count);
void collective_find_leftmost(point_t *local_leftmost, point_t *leftmost);
void print_usage(char *app_name);
