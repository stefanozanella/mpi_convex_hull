#ifndef MPI_CONVEX_HULL_BENCHMARK_H
#define MPI_CONVEX_HULL_BENCHMARK_H

#include <mpi.h>

/*
 * Total running time benchmarking
 */
static double _total_time_start;
static double _total_time_end;

void benchmark_start_total_time();
void benchmark_stop_total_time();
double benchmark_total_time();

/*
 * Inter-process communication time benchmarking
 */
static double _comm_time_total;
static double _comm_time_step_start;
static double _comm_time_step_end;

void benchmark_comm_time_step_start();
void benchmark_comm_time_step_end();
double benchmark_comm_time();

#endif
