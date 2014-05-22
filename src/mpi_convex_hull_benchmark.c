#include "mpi_convex_hull_benchmark.h"

#define TIMESCALE 1e3
#define PERCENT 1e2
#define BENCHMARK_FMT "%d,%ld,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;\n"

void benchmark_start_total_time() {
  _total_time_start = MPI_Wtime();
}

void benchmark_stop_total_time() {
  _total_time_end = MPI_Wtime();
}

double benchmark_total_time() {
  return (_total_time_end - _total_time_start) * TIMESCALE;
}

void benchmark_start_subhull_time() {
  _subhull_time_start = MPI_Wtime();
}

void benchmark_stop_subhull_time() {
  _subhull_time_end = MPI_Wtime();
}

double benchmark_subhull_time() {
  return (_subhull_time_end - _subhull_time_start) * TIMESCALE;
}

void benchmark_start_scatter_time() {
  _scatter_time_start = MPI_Wtime();
}

void benchmark_stop_scatter_time() {
  _scatter_time_end = MPI_Wtime();
}

double benchmark_scatter_time() {
  return (_scatter_time_end - _scatter_time_start) * TIMESCALE;
}

void benchmark_start_merge_time() {
  _merge_time_start = MPI_Wtime();
}

void benchmark_stop_merge_time() {
  _merge_time_end = MPI_Wtime();
}

double benchmark_merge_time() {
  return (_merge_time_end - _merge_time_start) * TIMESCALE;
}

void benchmark_tangent_time_step_start() {
  _tangent_time_step_start = MPI_Wtime();
}

void benchmark_tangent_time_step_end() {
  _tangent_time_step_end = MPI_Wtime();
  _tangent_time_total += _tangent_time_step_end - _tangent_time_step_start;
}

double benchmark_tangent_time() {
  return _tangent_time_total * TIMESCALE;
}

void benchmark_comm_time_step_start() {
  _comm_time_step_start = MPI_Wtime();
}

void benchmark_comm_time_step_end() {
  _comm_time_step_end = MPI_Wtime();
  _comm_time_total += _comm_time_step_end - _comm_time_step_start;
}

double benchmark_comm_time() {
  return _comm_time_total * TIMESCALE;
}

double benchmark_comm_ratio() {
  return benchmark_comm_time() / benchmark_total_time() * PERCENT;
}

void save_benchmark(FILE *out, int cpu_count, cloud_size_t input_size, double total_time, double subhull_time, double scatter_time, double merge_time, double tangent_time, double comm_time, double comm_ratio) {
  fprintf(out, BENCHMARK_FMT, cpu_count, input_size, total_time, subhull_time, scatter_time, merge_time, tangent_time, comm_time, comm_ratio);
}
