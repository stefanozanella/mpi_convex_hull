# vim: noexpandtab

CPUS = 1 2 4
RUN_CPUS = 4
SIZES = 100000 200000 500000
JOB_TEMPLATE = jobs/mpi_convex_hull.tpl

common_libs_objects = src/point_cloud.o src/point_cloud_io.o src/convex_hull.o

mpi_convex_hull_libs_objects = src/mpi_convex_hull.o src/mpi_convex_hull_benchmark.o
mpi_convex_hull_objects = src/mpi_convex_hull_main.o $(mpi_convex_hull_libs_objects) $(common_libs_objects)
mpi_convex_hull_executable = $(bindir)/mpi_convex_hull

generate_point_cloud_libs_objects = src/generate_point_cloud.o
generate_point_cloud_objects = src/generate_point_cloud_main.o $(generate_point_cloud_libs_objects) $(common_libs_objects)
generate_point_cloud_executable = $(bindir)/generate_point_cloud

bindir = bin
rundir = run
datadir = data
srcdir = src

jobfiles = $(foreach size, $(SIZES), $(foreach cpu, $(CPUS), jobs/mpi_convex_hull_$(size)_$(cpu).job))

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

SHELL = /bin/bash

$(mpi_convex_hull_executable) : $(mpi_convex_hull_objects) $(bindir)
	$(CC) $(CFLAGS) -o $(mpi_convex_hull_executable) $(mpi_convex_hull_objects) $(CLIBS)

$(generate_point_cloud_executable) : $(generate_point_cloud_objects) $(bindir)
	$(CC) $(CFLAGS) -o $(generate_point_cloud_executable) $(generate_point_cloud_objects) $(CLIBS)

$(bindir) :
	mkdir $(bindir)

$(jobfiles) :
	for size in $(SIZES); do \
		for cpu in $(CPUS); do \
			sed -e "s/%cpus%/$${cpu}/g" -e "s/%size%/$${size}/g" $(JOB_TEMPLATE) > jobs/mpi_convex_hull_$${size}_$${cpu}.job; \
		done; \
	done

all : $(mpi_convex_hull_executable) $(generate_point_cloud_executable)

clean :
	-rm src/*.o bin/*

clean_data :
	-rm data/*

clear : clean
	-rm -rf $(rundir) $(datadir)

submit : $(mpi_convex_hull_executable) $(jobfiles)
	-mkdir -p $(rundir)
	for job in $(jobfiles); do \
		llsubmit $${job}; \
	done

deploy :
	rsync -aPv . splab:mpi_convex_hull --exclude=".git" --exclude="*.sw[po]" --exclude="*.o" --exclude="tmp" --exclude="bin" --exclude="data/*.cvs"

run :
	mpirun -np $(RUN_CPUS) $(mpi_convex_hull_executable) $(datadir)/cloud_100000.dat $(datadir)/hull_100000.dat $(datadir)/benchmark_100000.cvs

gen_data: $(generate_point_cloud_executable)
	-mkdir -p data
	for size in $(SIZES); do \
		$(generate_point_cloud_executable) $${size} $(datadir)/cloud_$${size}.dat $(datadir)/reference_hull_$${size}.dat; \
	done

plot_data:
	GNUTERM=x11 gnuplot -e "cloud='$(datadir)/cloud_100000.dat'; hull='$(datadir)/reference_hull_100000.dat'" ext/gnuplot.plg

include test/Makefile

.PHONY : all, clean, clear, run, submit, deploy, gen_data, plot_data, jobs
