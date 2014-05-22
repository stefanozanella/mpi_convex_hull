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
 * Time to calculate the subhull locally
 */
static double _subhull_time_start;
static double _subhull_time_end;

void benchmark_start_subhull_time();
void benchmark_stop_subhull_time();
double benchmark_subhull_time();

/*
 * Time to scatter cloud across processors
 */
static double _scatter_time_start;
static double _scatter_time_end;

void benchmark_start_scatter_time();
void benchmark_stop_scatter_time();
double benchmark_scatter_time();

/*
 * Time to merge subclouds
 */
static double _merge_time_start;
static double _merge_time_end;

void benchmark_start_merge_time();
void benchmark_stop_merge_time();
double benchmark_merge_time();

/*
 * Time spent calculating right tangent on the master
 */
static double _tangent_time_total;
static double _tangent_time_step_start;
static double _tangent_time_step_end;

void benchmark_tangent_time_step_start();
void benchmark_tangent_time_step_end();
double benchmark_tangent_time();

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
void save_benchmark(FILE *out, int cpu_count, cloud_size_t input_size, double total_time, double subhull_time, double scatter_time, double merge_time, double tangent_time, double comm_time, double comm_ratio);
#endif
