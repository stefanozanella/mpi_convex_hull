#ifndef MPI_CONVEX_HULL_H
#define MPI_CONVEX_HULL_H

/**
 * High level application coordination routines
 */
int mpi_convex_hull(int argc, char const **argv);
int convex_hull_master(int argc, char const **argv, int rank, int cpu_count);
int convex_hull_slave(int argc, char const **argv, int rank, int cpu_count);

/**
 * Help screen
 */
void print_usage(const char* prog_name);

#endif
