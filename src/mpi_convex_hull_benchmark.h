#ifndef MPI_CONVEX_HULL_BENCHMARK_H
#define MPI_CONVEX_HULL_BENCHMARK_H

#include <mpi.h>
#include <stdio.h>

#include "point_cloud.h"

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
double benchmark_comm_ratio();

/*
 * Benchmark file output
 */
void save_benchmark(FILE *out, int cpu_count, cloud_size_t input_size, double total_time, double comm_time, double comm_ratio);
#endif
