#include "mpi_convex_hull.h"
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int exit_code = mpi_convex_hull(argc, argv);
  MPI_Finalize();

  return exit_code;
}
