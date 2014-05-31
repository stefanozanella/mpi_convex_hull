# Parallel Chan's Algorithm

## Compiling
To compile the whole project, issue the following from within project's root
dir:

    make all

This will build two binaries in the `bin/` folder.

## Running the binaries
The output of the compilation phase comprises two binaries:

  * `bin/mpi_convex_hull`: the parallel Chan's algorithm incarnation
  * `bin/generate_point_cloud`: program to generate test data, as well as the
    reference convex hull (to be used to test `mpi_convex_hull` correctness)

The convex hull program can be run in two specific fashions:

  * via `make run`: this make task will start a run of the parallel algorithm
    with an input size of 100k points and 4 CPUs. The number of CPUs can be
    selected by setting the `RUN_CPUS` variable.
  * via LoadLeveler: on the AIX OS running on the IBM Power7, the only option
    is to submit one of the jobs that can be found in the `jobs` folder. These
    can be automatically generated and submitted by invoking `make submit` on
    the target machine.

## A note about LoadLeveler
When running the parallel algorithm via LoadLeveler, a couple of output files
are produced in the `data` directory:
  * `benchmark_X_Y.cvs`: file containing the benchmarking data measured during
    the run
  * `hull_X.dat`: the convex hull calculated during the run. This can be
    compared with the reference hull for the given input size using `diff`, for
    example:

    ```
    diff data/reference_hull_100000.dat data/hull_100000.dat
    ```
    The output should report no differences.

