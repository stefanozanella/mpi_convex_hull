#include "mpi_convex_hull.h"
#include "mpi_convex_hull_benchmark.h"

#include "convex_hull.h"
#include "point_cloud.h"
#include "point_cloud_io.h"

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <sysexits.h>

#define MAX_CPUS 256

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
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
  if (argc < 4) {
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
  printf(ANSI_COLOR_GREEN "==> master: Loaded point cloud with %ld points\n" ANSI_COLOR_RESET, input_cloud.size);

  benchmark_start_total_time();

  init_mpi_runtime();
  broadcast_cloud_size(&input_cloud.size);

  point_cloud sub_cloud;
  scatter_cloud(&input_cloud, &sub_cloud, rank, cpu_count);
  printf(ANSI_COLOR_GREEN "==> master: Received point cloud chunk of size %ld\n" ANSI_COLOR_RESET, sub_cloud.size);

  printf(ANSI_COLOR_GREEN "==> master: Calculating convex hull for sub cloud\n" ANSI_COLOR_RESET);
  point_cloud sub_hull;
  convex_hull_graham_scan(&sub_cloud, &sub_hull);

  point_cloud final_hull;
  init_point_cloud(&final_hull, 0, input_cloud.size);

  find_leftmost(&(sub_hull.points[0]), &(final_hull.points[0]));

  printf(ANSI_COLOR_GREEN "==> master: Found leftmost point: (%ld, %ld)\n" ANSI_COLOR_RESET, final_hull.points[0].x, final_hull.points[0].y);

  int k = 0;
  point max;
  do {
    max = *find_right_tangent(&sub_hull, &(final_hull.points[k]));

    for (int m = 2; m <= cpu_count; m = m << 1) {
        point received;
        MPI_Recv(&received, 1, mpi_point, rank + (m >> 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        max = *max_angle(&max, &received, &(final_hull.points[k]));
    }

    final_hull.points[k+1] = max;

    MPI_Bcast(&(final_hull.points[k+1]), 1, mpi_point, 0, MPI_COMM_WORLD);
    k++;

    printf(ANSI_COLOR_GREEN "==> master: Found next point in hull: (%ld, %ld)\n" ANSI_COLOR_RESET, final_hull.points[k].x, final_hull.points[k].y);
  } while (compare_point(&(final_hull.points[k]), &(final_hull.points[0])));

  /* The last point is equal to the first one, so we delete it from the final
   * hull */
  final_hull.size = k;

  benchmark_stop_total_time();

  const char *benchmark_filename = argv[3];
  FILE *benchmark_out;
  if ((benchmark_out = fopen(benchmark_filename, "w")) == NULL) {
    printf("Error opening file %s. Aborting.\n", benchmark_filename);
    return EX_IOERR;
  }
  save_benchmark(benchmark_out, cpu_count, input_cloud.size, benchmark_total_time(), benchmark_comm_time(), benchmark_comm_ratio());
  fclose(benchmark_out);

  const char *output_filename = argv[2];
  FILE *hull_out;
  if ((hull_out = fopen(output_filename, "w")) == NULL) {
    printf("Error opening file %s. Aborting.\n", output_filename);
    return EX_IOERR;
  }
  save_point_cloud(&final_hull, hull_out);
  fclose(hull_out);

  return EX_OK;
}

int convex_hull_slave(int argc, char const **argv, int rank, int cpu_count) {
  init_mpi_runtime();
  cloud_size_t cloud_size;
  broadcast_cloud_size(&cloud_size);
  printf(ANSI_COLOR_YELLOW "\t slave %d: Advertised point cloud size: %ld\n" ANSI_COLOR_RESET, rank, cloud_size);

  point_cloud input_cloud;
  init_void_point_cloud(&input_cloud, cloud_size);

  point_cloud sub_cloud;
  scatter_cloud(&input_cloud, &sub_cloud, rank, cpu_count);
  printf(ANSI_COLOR_YELLOW "==> slave %d: Received point cloud chunk of size %ld\n" ANSI_COLOR_RESET, rank, sub_cloud.size);

  printf(ANSI_COLOR_YELLOW "==> slave %d: Calculating convex hull for sub cloud\n" ANSI_COLOR_RESET, rank);
  point_cloud sub_hull;
  convex_hull_graham_scan(&sub_cloud, &sub_hull);

  point first_in_hull;
  find_leftmost(&(sub_hull.points[0]), &first_in_hull);

  point p = first_in_hull;

  do {
    point max = *find_right_tangent(&sub_hull, &p);
    for (int k = 2; k <= cpu_count; k = k << 1) {
      if (rank % k == 0) {
        point received;
        MPI_Recv(&received, 1, mpi_point, rank + (k >> 1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        max = *max_angle(&max, &received, &p);
      } else if (rank % (k >> 1) == 0) {
        MPI_Send(&max, 1, mpi_point, rank - (k >> 1), 0, MPI_COMM_WORLD);
      }
    }

    MPI_Bcast(&p, 1, mpi_point, 0, MPI_COMM_WORLD);

  } while (compare_point(&p, &first_in_hull));

  return EX_OK;
}

void broadcast_cloud_size(cloud_size_t *size) {
  benchmark_comm_time_step_start();
  MPI_Bcast(size, 1, MPI_LONG, 0, MPI_COMM_WORLD);
  benchmark_comm_time_step_end();
}

void scatter_cloud(point_cloud *input_cloud, point_cloud *sub_cloud, int rank, int cpu_count) {
  int chunk_sizes[MAX_CPUS], offsets[MAX_CPUS];
  setup_scatter_params(input_cloud->size, cpu_count, chunk_sizes, offsets);

  init_point_cloud(sub_cloud, chunk_sizes[rank], chunk_sizes[rank]);

  benchmark_comm_time_step_start();
  MPI_Scatterv(input_cloud->points, chunk_sizes, offsets, mpi_point, sub_cloud->points, sub_cloud->size, mpi_point, 0, MPI_COMM_WORLD);
  benchmark_comm_time_step_end();
}

void find_leftmost(point *local_point, point *leftmost) {
  benchmark_comm_time_step_start();
  MPI_Allreduce(local_point, leftmost, 1, mpi_point, MPI_MIN_POINT, MPI_COMM_WORLD);
  benchmark_comm_time_step_end();
}

/* TODO: Maybe passing the points by reference to 'turn()' will improve
 * performance
 */
point *find_right_tangent(point_cloud *cloud, point *reference) {
  cloud_size_t start = 0;
  cloud_size_t end = cloud->size;

  turn_t start_prev = turn(*reference, cloud->points[start], cloud->points[end - 1]);
  turn_t start_next = turn(*reference, cloud->points[start], cloud->points[(start + 1) % end]);

  while (start < end) {
    cloud_size_t pivot = (start + end) / 2;
    turn_t pivot_prev = turn(*reference, cloud->points[pivot], cloud->points[(pivot - 1) % cloud->size]);
    turn_t pivot_next = turn(*reference, cloud->points[pivot], cloud->points[(pivot + 1) % cloud->size]);

    turn_t pivot_side = turn(*reference, cloud->points[start], cloud->points[pivot]);

    if ((pivot_prev != TURN_RIGHT) && (pivot_next != TURN_RIGHT)) {
      start = pivot;
      break;
    } else if ((pivot_side == TURN_LEFT && start_next == TURN_RIGHT) || (start_prev == start_next) || (pivot_side == TURN_RIGHT && pivot_prev == TURN_RIGHT)) {
      end = pivot;
    } else {
      start = pivot + 1;
      start_prev = (turn_t) - pivot_next;
      start_next = turn(*reference, cloud->points[start], cloud->points[(start + 1) % cloud->size]);
    }
  }

  if (!compare_point(&cloud->points[start], reference)) {
    start = (start + 1) % cloud->size;
  }

  return &cloud->points[start];
}

void init_mpi_runtime() {
  int blockcounts[1] = { 2 };
  MPI_Aint offsets[1] = { 0 };
  MPI_Datatype types[1] = { MPI_LONG };

  MPI_Type_struct(1, blockcounts, offsets, types, &mpi_point);
  MPI_Type_commit(&mpi_point);

  MPI_Op_create(&mpi_min_point_op, 1, &MPI_MIN_POINT);
}

void setup_scatter_params(cloud_size_t input_size, int dest_count, int *sizes, int *offsets) {
  int chunk_size = ceil(input_size/ (float) dest_count);
  int last_chunk_size = input_size - chunk_size * (dest_count - 1);

  for (int k = 0; k < dest_count - 1; k++) {
    sizes[k] = chunk_size;
    offsets[k] = k * chunk_size;
  }

  sizes[dest_count - 1] = last_chunk_size;
  offsets[dest_count - 1] = chunk_size * (dest_count - 1);
}

void mpi_min_point_op(void *invec, void *inoutvec, int *len, MPI_Datatype *type) {
  point* in = (point*) invec;
  point* inout = (point*) inoutvec;

  if (compare_point(&(in[0]), &(inout[0])) < 0) {
    inout[0] = in[0];
  }
}

// TODO This looks ugly
point *max_angle(point *x, point *y, point *reference) {
  turn_t measure = turn(*reference, *x, *y);
  if (measure == TURN_RIGHT || (measure == TURN_NONE && dist(*reference, *x) > dist(*reference, *y))) {
    return y;
  } else {
    return x;
  }
}

void print_usage(const char* prog_name) {
  printf("Usage: %s <input_file> <output_file>\n", prog_name);
  printf("Parameters:\n");
  printf("  input_file: path of the file containing point cloud data\n");
  printf("  output_file: path of the file where the calculated convex hull will be saved\n");
  printf("  benchmark_file: path of the file where benchmark metrics will be written\n");
}
