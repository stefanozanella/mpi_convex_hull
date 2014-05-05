#include "mpi_convex_hull_benchmark.h"

void benchmark_start_total_time() {
  _total_time_start = MPI_Wtime();
}

void benchmark_stop_total_time() {
  _total_time_end = MPI_Wtime();
}

double benchmark_total_time() {
  return _total_time_end - _total_time_start;
}

void benchmark_comm_time_step_start() {
  _comm_time_step_start = MPI_Wtime();
}

void benchmark_comm_time_step_end() {
  _comm_time_step_end = MPI_Wtime();
  _comm_time_total += _comm_time_step_end - _comm_time_step_start;
}

double benchmark_comm_time() {
  return _comm_time_total;
}
