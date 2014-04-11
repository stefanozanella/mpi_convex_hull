#include "mpi_convex_hull.h"

int main(int argc, char* argv[]) {
  return exec_mpi_app(argc, argv);
}

int exec_mpi_app(int argc, char* argv[]) {
  int retval;

  MPI_Init(&argc, &argv);
  mpi_initialize_runtime();
  retval = mpi_convex_hull(argc, argv);
  MPI_Finalize();

  return retval;
}

void mpi_init_point_type() {
  MPI_Datatype types[1];
  int blockcounts[1];
  MPI_Aint offsets[1];

  blockcounts[0] = 2;
  offsets[0] = 0;
  types[0] = MPI_LONG;
  MPI_Type_struct(1, blockcounts, offsets, types, &mpi_point_t);
  MPI_Type_commit(&mpi_point_t);
}

void mpi_init_min_point_op() {
  MPI_Op_create(mpi_point_min_op, 1, &MPI_POINT_MIN);
}

void mpi_initialize_runtime() {
  mpi_init_point_type();
  mpi_init_min_point_op();
}

/*
 * MPI Convex Hull
 */
int mpi_convex_hull(int argc, char* argv[]) {
  int rank, size;
  int retval = 0;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  switch (rank) {
    case 0:
      retval = ch_master(argc, argv, rank, size);
      break;
    default:
      retval = ch_slave(argc, argv, rank, size);
      break;
  }

  return retval;
}

/*
 * The master process, which is in charge to also act as a coordinator for the
 * whole CPU set.
 */
int ch_master(int argc, char* argv[], int rank, int cpu_count) {
  if (argc < 2) {
    print_usage(argv[0]);
    return 1;
  }

  char *point_cloud_file = argv[1];
  FILE *point_cloud_fp;
  if ((point_cloud_fp = fopen(point_cloud_file, "r")) == NULL) {
    printf("\nError: cannot open file %s. Aborting.\n", point_cloud_file);
    return 1;
  }

  int cloud_size = 0;
  if (fscanf(point_cloud_fp, PC_SIZE_FMT, &cloud_size) < 1) {
    printf("Cannot read point cloud size: please start the file with the following line:\n");
    printf("# size=<cloud size>\n");
  }

  printf("Reading point cloud of size %d.\n", cloud_size);
  point_cloud_t point_cloud = load_point_cloud(cloud_size, point_cloud_fp);
  printf("[done]\n");

  bcast_point_cloud_size(&cloud_size);

  point_cloud_t local_hull = chan_step_1(point_cloud, rank, cpu_count);

  // Here we need to account for the fact that the first point in the final
  // hull is just a made up one. In the extreme case where all the point cloud
  // is a convex hull, we'd have an overflow.
  point_cloud_t final_hull = init_point_cloud(cloud_size + 1);

  final_hull.pc[0].x = 0;
  final_hull.pc[0].y = MIN_COORD;

  collective_find_leftmost(&(local_hull.pc[0]), &(final_hull.pc[1]));

  printf("Min: (%ld, %ld)\n", final_hull.pc[1].x, final_hull.pc[1].y);

  // Find point in local hull that maximizes (minimizes) the polar angle with
  // p0 and p1
  point_t local_best = local_hull[1];
  int low = 2, high = local_hull.size, hull_size = 1;
  while (low <= high) {
    int index = (low + high) / 2;

    if (turn_direction(final_hull[hull_size -1], final_hull[hull_size], local_
  }

  return 0;
}

int ch_slave(int argc, char *argv[], int rank, int cpu_count) {
  int cloud_size;
  bcast_point_cloud_size(&cloud_size);
  printf("[%d] Advertised point cloud size: %d\n", rank, cloud_size);

  point_cloud_t local_hull = chan_step_1(null_point_cloud(cloud_size), rank, cpu_count);
  collective_find_leftmost(&(local_hull.pc[0]), NULL);

  return 0;
}

void collective_find_leftmost(point_t *local_leftmost, point_t *leftmost) {
  MPI_Reduce(local_leftmost, leftmost, 1, mpi_point_t, MPI_POINT_MIN, 0, MPI_COMM_WORLD);
}

void mpi_point_min_op(void *invec, void *inoutvec, int *len, MPI_Datatype *datatype) {
  point_t* in = (point_t*)invec;
  point_t* inout = (point_t*)inoutvec;

  if (point_compare(&(in[0]), &(inout[0])) < 0) {
    inout[0] = in[0];
  }
}

void bcast_point_cloud_size(int *size) {
  MPI_Bcast(size, 1, MPI_INT, 0, MPI_COMM_WORLD);
}

void setup_scatter_params(int array_size, int dest_count, int *sizes, int *offsets) {
  int chunk_size = ceil(array_size / (float) dest_count);
  int last_chunk_size = array_size - chunk_size * (dest_count - 1);

  for (int k = 0; k < dest_count - 1; k++) {
    sizes[k] = chunk_size;
    offsets[k] = k * chunk_size;
  }

  sizes[dest_count - 1] = last_chunk_size;
  offsets[dest_count - 1] = (chunk_size * (dest_count - 1));

}

point_cloud_t chan_step_1(point_cloud_t pc, int rank, int cpu_count) {
  int chunk_sizes[MAX_CPUS], offsets[MAX_CPUS];

  setup_scatter_params(pc.size, cpu_count, chunk_sizes, offsets);
  point_cloud_t pc_chunk = init_point_cloud(chunk_sizes[rank]);

  MPI_Scatterv(pc.pc, chunk_sizes, offsets, mpi_point_t, pc_chunk.pc, pc_chunk.size, mpi_point_t, 0, MPI_COMM_WORLD);

  qsort(pc_chunk.pc, pc_chunk.size, sizeof(point_t), &point_compare);
  return convex_hull_monotone_chain(pc_chunk);
}

/*
 * Help screen.
 */
void print_usage(char* prog_name) {
  printf("Usage: %s <input_file>\n", prog_name);
  printf("Parameters:\n");
  printf("  input_file: path of the file containing point cloud data\n");
}
