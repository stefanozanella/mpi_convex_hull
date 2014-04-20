#include "mpi_convex_hull.h"
#include "point_cloud.h"
#include "point_cloud_io.h"

#include <mpi.h>
#include <stdio.h>
#include <sysexits.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_BOLD_WHITE   "\x1b[1;37m"
#define ANSI_COLOR_RESET   "\x1b[0m"

int mpi_convex_hull(int argc, char const **argv) {
  int rank, size;
  int exit_code = 0;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  switch (rank) {
    case 0:
      exit_code = convex_hull_master(argc, argv, rank, size);
      break;
    default:
      exit_code = convex_hull_slave(argc, argv, rank, size);
      break;
  }

  return exit_code;
}

int convex_hull_master(int argc, char const **argv, int rank, int cpu_count) {
  if (argc < 2) {
    print_usage(argv[0]);
    return EX_USAGE;
  }

  const char *input_filename = argv[1];
  FILE *point_cloud_file;
  if ((point_cloud_file = fopen(input_filename, "r")) == NULL) {
    printf(ANSI_COLOR_RED "Error: cannot open file %s. Aborting.\n" ANSI_COLOR_RESET, input_filename);
    return EX_IOERR;
  }

  point_cloud input_cloud;
  load_point_cloud(point_cloud_file, &input_cloud);
  printf(ANSI_COLOR_BOLD_WHITE "==> Loaded point cloud with %ld points\n" ANSI_COLOR_RESET, input_cloud.size);

  return EX_OK;
}

int convex_hull_slave(int argc, char const **argv, int rank, int cpu_count) {
  return EX_OK;
}

void print_usage(const char* prog_name) {
  printf("Usage: %s <input_file>\n", prog_name);
  printf("Parameters:\n");
  printf("  input_file: path of the file containing point cloud data\n");
}
