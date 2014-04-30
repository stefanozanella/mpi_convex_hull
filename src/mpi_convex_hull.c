#include "mpi_convex_hull.h"
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
  printf(ANSI_COLOR_GREEN "==> master: Loaded point cloud with %ld points\n" ANSI_COLOR_RESET, input_cloud.size);

  init_mpi_runtime();
  broadcast_cloud_size(&input_cloud.size);

  point_cloud sub_cloud;
  scatter_cloud(&input_cloud, &sub_cloud, rank, cpu_count);
  printf(ANSI_COLOR_GREEN "==> master: Received point cloud chunk of size %ld\n" ANSI_COLOR_RESET, sub_cloud.size);

  printf(ANSI_COLOR_GREEN "==> master: Calculating convex hull for sub cloud\n" ANSI_COLOR_RESET);
  point_cloud sub_hull;
  convex_hull_graham_scan(&sub_cloud, &sub_hull);

  /* DEBUG */
  FILE *sub_cloud_out;
  char sub_cloud_file[256];
  FILE *sub_hull_out;
  char sub_hull_file[256];
  sprintf(sub_hull_file, "data/sub_hull_%d.dat", rank);
  sprintf(sub_cloud_file, "data/sub_cloud_%d.dat", rank);
  if ((sub_hull_out = fopen(sub_hull_file, "w")) == NULL) {
    /* TODO: Error handling */
    printf("Error opening file %s. Aborting.\n", sub_hull_file);
    return EX_IOERR;
  }
  if ((sub_cloud_out = fopen(sub_cloud_file, "w")) == NULL) {
    /* TODO: Error handling */
    printf("Error opening file %s. Aborting.\n", sub_cloud_file);
    return EX_IOERR;
  }
  save_point_cloud(&sub_hull, sub_hull_out);
  save_point_cloud(&sub_cloud, sub_cloud_out);
  fclose(sub_hull_out);
  fclose(sub_cloud_out);
  /* DEBUG */

  point_cloud final_hull;
  init_point_cloud(&final_hull, 0, input_cloud.size);

  find_leftmost(&(sub_hull.points[0]), &(final_hull.points[0]));

  printf(ANSI_COLOR_GREEN "==> master: Found leftmost point: (%ld, %ld)\n" ANSI_COLOR_RESET, final_hull.points[0].x, final_hull.points[0].y);

  int k = 0;
  do {
    point *q = find_right_tangent(&sub_hull, &(final_hull.points[k]));
    find_next_point_in_hull(q, &(final_hull.points[k]), &(final_hull.points[k+1]));
    k++;

    printf(ANSI_COLOR_GREEN "==> master: Found next point in hull: (%ld, %ld)\n" ANSI_COLOR_RESET, final_hull.points[k].x, final_hull.points[k].y);
  } while (compare_point(&(final_hull.points[k]), &(final_hull.points[0])));

  /* The last point is equal to the first one, so we delete it from the final
   * hull */
  pop(&final_hull);

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

  /* DEBUG */
  FILE *sub_cloud_out;
  char sub_cloud_file[256];
  FILE *sub_hull_out;
  char sub_hull_file[256];
  sprintf(sub_hull_file, "data/sub_hull_%d.dat", rank);
  sprintf(sub_cloud_file, "data/sub_cloud_%d.dat", rank);
  if ((sub_hull_out = fopen(sub_hull_file, "w")) == NULL) {
    /* TODO: Error handling */
    printf("Error opening file %s. Aborting.\n", sub_hull_file);
    return EX_IOERR;
  }
  if ((sub_cloud_out = fopen(sub_cloud_file, "w")) == NULL) {
    /* TODO: Error handling */
    printf("Error opening file %s. Aborting.\n", sub_cloud_file);
    return EX_IOERR;
  }
  save_point_cloud(&sub_hull, sub_hull_out);
  save_point_cloud(&sub_cloud, sub_cloud_out);
  fclose(sub_hull_out);
  fclose(sub_cloud_out);
  /* DEBUG */

  point first_in_hull;
  find_leftmost(&(sub_hull.points[0]), &first_in_hull);

  printf(ANSI_COLOR_YELLOW "==> slave %d: Found leftmost point: (%ld, %ld)\n" ANSI_COLOR_RESET, rank, first_in_hull.x, first_in_hull.y);

  point p = first_in_hull;
  do {
  point *q = find_right_tangent(&sub_hull, &p);
  find_next_point_in_hull(q, &p, &p);
  //printf(ANSI_COLOR_YELLOW "==> slave %d: Found next point in hull: (%ld, %ld)\n" ANSI_COLOR_RESET, rank, p.x, p.y);
  } while (compare_point(&p, &first_in_hull));

  return EX_OK;
}

void broadcast_cloud_size(cloud_size_t *size) {
  MPI_Bcast(size, 1, MPI_LONG, 0, MPI_COMM_WORLD);
}

void scatter_cloud(point_cloud *input_cloud, point_cloud *sub_cloud, int rank, int cpu_count) {
  int chunk_sizes[MAX_CPUS], offsets[MAX_CPUS];
  setup_scatter_params(input_cloud->size, cpu_count, chunk_sizes, offsets);

  init_point_cloud(sub_cloud, chunk_sizes[rank], chunk_sizes[rank]);

  MPI_Scatterv(input_cloud->points, chunk_sizes, offsets, mpi_point, sub_cloud->points, sub_cloud->size, mpi_point, 0, MPI_COMM_WORLD);
}

void find_leftmost(point *local_point, point *leftmost) {
  MPI_Allreduce(local_point, leftmost, 1, mpi_point, MPI_MIN_POINT, MPI_COMM_WORLD);
}

/* TODO: Double check this method, it's probably the single most important
 * point to optimize
 * TODO: Maybe passing the points by reference to 'turn()' will improve
 * performance
 */
point *find_right_tangent(point_cloud *cloud, point *reference) {
  //printf("\tcurrent reference point: (%ld, %ld)\n", reference->x, reference->y);

  cloud_size_t start = 0;
  cloud_size_t end = cloud->size;

  turn_t start_prev = turn(*reference, cloud->points[start], cloud->points[end - 1]);
  turn_t start_next = turn(*reference, cloud->points[start], cloud->points[(start + 1) % end]);

  while (start < end) {
    //printf("\tsearching in range %ld .. %ld\n", start, end);
    cloud_size_t pivot = (start + end) / 2;
    turn_t pivot_prev = turn(*reference, cloud->points[pivot], cloud->points[(pivot - 1) % cloud->size]);
    turn_t pivot_next = turn(*reference, cloud->points[pivot], cloud->points[(pivot + 1) % cloud->size]);

    turn_t pivot_side = turn(*reference, cloud->points[start], cloud->points[pivot]);

    if ((pivot_prev != TURN_RIGHT) && (pivot_next != TURN_RIGHT)) {
      //return &cloud->points[pivot];
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
    start++;
  }

  //printf("\treturning point at index %ld: (%ld, %ld)\n", start, cloud->points[start].x, cloud->points[start].y);

  return &cloud->points[start];
}

void find_next_point_in_hull(point *local_point, point *last_found, point *next_hull_point) {
  last_point_in_hull = *last_found;
  MPI_Allreduce(local_point, next_hull_point, 1, mpi_point, MPI_MAX_ANGLE, MPI_COMM_WORLD);
}

void init_mpi_runtime() {
  int blockcounts[1] = { 2 };
  MPI_Aint offsets[1] = { 0 };
  MPI_Datatype types[1] = { MPI_LONG };

  MPI_Type_struct(1, blockcounts, offsets, types, &mpi_point);
  MPI_Type_commit(&mpi_point);

  MPI_Op_create(&mpi_min_point_op, 1, &MPI_MIN_POINT);
  MPI_Op_create(&mpi_max_angle_op, 1, &MPI_MAX_ANGLE);
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

void mpi_max_angle_op(void *invec, void *inoutvec, int *len, MPI_Datatype *type) {
  /* TODO: This might be optimized by inlining the cast */
  point* in = (point*) invec;
  point* inout = (point*) inoutvec;

  turn_t measure = turn(last_point_in_hull, inout[0], in[0]);
  if (measure == TURN_RIGHT || (measure == TURN_NONE && dist(last_point_in_hull, in[0]) > dist(last_point_in_hull, inout[0]))) {
    inout[0] = in[0];
  }

}

void print_usage(const char* prog_name) {
  printf("Usage: %s <input_file>\n", prog_name);
  printf("Parameters:\n");
  printf("  input_file: path of the file containing point cloud data\n");
}
