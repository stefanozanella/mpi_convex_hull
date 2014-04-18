# vim: noexpandtab

CPUS = 4
JOBFILE = jobs/mpi_convex_hull.job

libs_objects = src/point_cloud_gen.o

mpi_convex_hull_objects = src/mpi_convex_hull.o
mpi_convex_hull_executable = $(bindir)/mpi_convex_hull

generate_point_cloud_objects = src/generate_point_cloud.o $(libs_objects)
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
	-rm $(mpi_convex_hull_objects) $(mpi_convex_hull_executable) \
		$(generate_point_cloud_objects) $(generate_point_cloud_executable)

clear : clean
	-rm -rf $(rundir) $(datadir)

run :
	mpirun -np $(CPUS) $(mpi_convex_hull_executable)

submit : $(mpi_convex_hull_executable)
	-mkdir -p $(rundir)
	llsubmit $(JOBFILE)

deploy :
	rsync -aPv . splab:mpi_convex_hull --exclude="*.sw[po]" --exclude="tmp" --exclude="bin"

gen_data: $(generate_point_cloud_executable)
	-mkdir -p data
	$(generate_point_cloud_executable) 1000000 $(datadir)/cloud.dat $(datadir)/hull.dat

plot_data:
	GNUTERM=x11 gnuplot -e "cloud='$(datadir)/cloud.dat'; hull='$(datadir)/hull.dat'" ext/gnuplot.plg

include test/Makefile

.PHONY : all, clean, clear, run, submit, deploy, gen_data, plot_data
