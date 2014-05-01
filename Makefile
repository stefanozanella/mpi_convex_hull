# vim: noexpandtab

CPUS = 4
JOBFILE = jobs/mpi_convex_hull.job

common_libs_objects = src/point_cloud.o src/point_cloud_io.o src/convex_hull.o

mpi_convex_hull_libs_objects = src/mpi_convex_hull.o
mpi_convex_hull_objects = src/mpi_convex_hull_main.o $(mpi_convex_hull_libs_objects) $(common_libs_objects)
mpi_convex_hull_executable = $(bindir)/mpi_convex_hull

generate_point_cloud_libs_objects = src/generate_point_cloud.o
generate_point_cloud_objects = src/generate_point_cloud_main.o $(generate_point_cloud_libs_objects) $(common_libs_objects)
generate_point_cloud_executable = $(bindir)/generate_point_cloud

bindir = bin
rundir = run
datadir = data
srcdir = src

mpcc := $(shell which mpcc 2> /dev/null)
mpicc := $(shell which mpicc 2> /dev/null)

ifneq ($(mpcc),)
	CC := $(mpcc)
endif
	
ifneq ($(mpicc),)
	CC := $(mpicc)
endif

ifeq ($(mpicc)$(mpcc),)
  CC = $(error Cannot find a suitable MPI compiler. Please install OpenMPI or \
		build the project on a suitable machine)
endif

CFLAGS = -std=c99
CLIBS = -lm

$(mpi_convex_hull_executable) : $(mpi_convex_hull_objects) $(bindir)
	$(CC) $(CFLAGS) -o $(mpi_convex_hull_executable) $(mpi_convex_hull_objects) $(CLIBS)

$(generate_point_cloud_executable) : $(generate_point_cloud_objects) $(bindir)
	$(CC) $(CFLAGS) -o $(generate_point_cloud_executable) $(generate_point_cloud_objects) $(CLIBS)

$(bindir) :
	mkdir $(bindir)

all : $(mpi_convex_hull_executable) $(generate_point_cloud_executable)

clean :
	-rm src/*.o bin/*

clean_data :
	-rm data/*

clear : clean
	-rm -rf $(rundir) $(datadir)

run :
	mpirun -np $(CPUS) $(mpi_convex_hull_executable) $(datadir)/cloud.dat $(datadir)/hull.dat

submit : $(mpi_convex_hull_executable)
	-mkdir -p $(rundir)
	llsubmit $(JOBFILE)

deploy :
	rsync -aPv . splab:mpi_convex_hull --exclude="*.sw[po]" --exclude="*.o" --exclude="tmp" --exclude="bin"

gen_data: $(generate_point_cloud_executable)
	-mkdir -p data
	$(generate_point_cloud_executable) 1000000 $(datadir)/cloud.dat $(datadir)/reference_hull.dat

plot_data:
	GNUTERM=x11 gnuplot -e "cloud='$(datadir)/cloud.dat'; hull='$(datadir)/hull.dat'" ext/gnuplot.plg

include test/Makefile

.PHONY : all, clean, clear, run, submit, deploy, gen_data, plot_data
