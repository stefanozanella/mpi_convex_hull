# vim: noexpandtab

CPUS = 4
JOBFILE = jobs/mpi_convex_hull.job

mpi_convex_hull_objects = src/mpi_convex_hull.o
mpi_convex_hull_executable = $(bindir)/mpi_convex_hull

generate_point_cloud_objects = src/generate_point_cloud.o src/point_cloud_gen.o
generate_point_cloud_executable = $(bindir)/generate_point_cloud

bindir = bin
rundir = run

test_files = test/generate_point_cloud_test.c

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

$(mpi_convex_hull_executable) : $(mpi_convex_hull_objects) $(bindir)
	$(CC) -o $(mpi_convex_hull_executable) $(mpi_convex_hull_objects)

$(generate_point_cloud_executable) : $(generate_point_cloud_objects) $(bindir)
	$(CC) -o $(generate_point_cloud_executable) $(generate_point_cloud_objects)

$(bindir) :
	mkdir $(bindir)

all : $(mpi_convex_hull_executable) $(generate_point_cloud_executable)

clean :
	-rm $(mpi_convex_hull_objects) $(mpi_convex_hull_executable) \
		$(generate_point_cloud_objects) $(generate_point_cloud_executable)

clear : clean
	-rm -rf $(rundir)

run :
	mpirun -np $(CPUS) $(mpi_convex_hull_executable)

submit : $(mpi_convex_hull_executable)
	-mkdir -p $(rundir)
	llsubmit $(JOBFILE)

deploy :
	rsync -aPv . splab:mpi_convex_hull --exclude="*.sw[po]" --exclude="tmp" --exclude="bin"

# ###############
# Testing harness
# ###############

tmp_dir = tmp
gtest_dir = $(tmp_dir)/gtest-1.7.0
gtest_pkg = $(tmp_dir)/gtest-1.7.0.zip
gtest_url = https://googletest.googlecode.com/files/gtest-1.7.0.zip
gtest_build_dir = $(gtest_dir)/build
gtest_libs = $(gtest_build_dir)/libgtest.a $(gtest_build_dir)/libgtest_main.a
gtest_include_dir = $(gtest_dir)/include
gtest_main = $(gtest_dir)/src/gtest_main.cc
gtest_exec = bin/test

$(gtest_pkg) :
	-mkdir -p $(tmp_dir)
	curl -L -o $(gtest_pkg) $(gtest_url)

$(gtest_dir) : $(gtest_pkg)
	unzip $(gtest_pkg) -d $(tmp_dir)
	touch $(gtest_dir)

$(gtest_libs) : $(gtest_dir)
	mkdir $(gtest_build_dir)
	cd $(gtest_build_dir) && cmake .. && make

$(gtest_exec) : $(gtest_libs) $(test_files) src/point_cloud_gen.c
	g++ -I$(gtest_include_dir) -Isrc -L$(gtest_build_dir) $(gtest_main) src/point_cloud_gen.c\
		$(test_files) -lgtest -lpthread -o $(gtest_exec)

test : $(gtest_exec)
	$(gtest_exec)

.PHONY : all, clean, clear, run, submit, deploy, test
