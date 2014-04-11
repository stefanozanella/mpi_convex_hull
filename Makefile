# vim: noexpandtab
# .RECIPEPREFIX = >

#BINDIR = bin

#MPICC = $(@which mpcc 2>&1 > /dev/null)
#CFLAGS = -std=c99
#ifeq ($(shell which mpcc 2> /dev/null),)
#	MPICC = mpicc
#	CFLAGS += -Wall -W -pedantic -Wno-unused-parameter
#else
#	MPICC = mpcc
#endif
#RUN = mpirun

#LIBS = -lm
#RUN_OPTS = -np $(PROCS)

#GEN_TEST_DATA_BIN = $(BINDIR)/gen_test_data
#MPI_CONVEX_HULL_BIN = $(BINDIR)/mpi_convex_hull

#COMMON_SOURCE = src/point_cloud_io.c src/point_cloud_geom.c src/convex_hull.c
#GEN_TEST_DATA_SOURCE = src/gen_test_data.c $(COMMON_SOURCE)
#MPI_CONVEX_HULL_SOURCE = src/mpi_convex_hull.c $(COMMON_SOURCE)

#prep:
#	mkdir -p $(BINDIR)

#gen_test_data: $(GEN_TEST_DATA_SOURCE)
#	$(CC) $(CFLAGS) -o $(GEN_TEST_DATA_BIN) $^ $(LIBS)

#mpi_convex_hull: $(MPI_CONVEX_HULL_SOURCE)
#	$(MPICC) $(CFLAGS) -o $(MPI_CONVEX_HULL_BIN) $^ $(LIBS)

#all: prep mpi_convex_hull gen_test_data

#run:
#	$(RUN) $(RUN_OPTS) $(MPI_CONVEX_HULL_BIN) $(in)

#.PHONY : prep clean

CPUS = 4
JOBFILE = jobs/mpi_convex_hull.job

objects = src/main.o
executable = $(bindir)/mpi_convex_hull
bindir = bin
rundir = run

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

$(executable) : $(objects) $(bindir)
	$(CC) -o $(executable) $(objects)

$(bindir) :
	mkdir $(bindir)

.PHONY : clean, clear, run, submit, deploy

clean :
	-rm $(objects) $(executable)

clear : clean
	-rm -rf $(run)

run :
	mpirun -np $(CPUS) $(executable)

submit : $(executable)
	-mkdir -p $(rundir)
	llsubmit $(JOBFILE)

deploy : clean
	rsync -aPv . splab:mpi_convex_hull --exclude="*.sw[po]"
