# vim: noexpandtab

CPUS = 4
JOBFILE = jobs/mpi_convex_hull.job

mpi_convex_hull_objects = src/mpi_convex_hull.o
mpi_convex_hull_executable = $(bindir)/mpi_convex_hull

generate_point_cloud_objects = src/generate_point_cloud.o src/point_cloud_gen.o
generate_point_cloud_executable = $(bindir)/generate_point_cloud

bindir = bin
rundir = run
datadir = data

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

CFLAGS = -std=c99

$(mpi_convex_hull_executable) : $(mpi_convex_hull_objects) $(bindir)
	$(CC) $(CFLAGS) -o $(mpi_convex_hull_executable) $(mpi_convex_hull_objects)

$(generate_point_cloud_executable) : $(generate_point_cloud_objects) $(bindir)
	$(CC) $(CFLAGS) -o $(generate_point_cloud_executable) $(generate_point_cloud_objects)

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
	$(generate_point_cloud_executable) 123456 $(datadir)/cloud_123456.dat

# ###############
# Testing harness
# ###############

tmp_dir = tmp

gmock_url = https://googlemock.googlecode.com/files/gmock-1.7.0.zip
gmock_dir = $(tmp_dir)/gmock-1.7.0
gmock_pkg = $(tmp_dir)/gmock-1.7.0.zip
gmock_build_dir = $(gmock_dir)/build
gmock_include_dir = $(gmock_dir)/include
gmock_libs = $(gmock_build_dir)/libgmock.a $(gmock_build_dir)/libgmock_main.a
gmock_main = $(gmock_dir)/src/gmock_main.cc

gtest_build_dir = $(gmock_build_dir)/gtest
gtest_libs = $(gtest_build_dir)/libgtest.a $(gtest_build_dir)/libgtest_main.a
gtest_include_dir = $(gmock_dir)/gtest/include

gmock_exec = bin/test

$(gmock_pkg) :
	-mkdir -p $(tmp_dir)
	curl -L -o $(gmock_pkg) $(gmock_url)

$(gmock_dir) : $(gmock_pkg)
	unzip $(gmock_pkg) -d $(tmp_dir)
	touch $(gmock_pkg)

$(gmock_libs) $(gtest_libs): $(gmock_dir)
	mkdir $(gmock_build_dir)
	cd $(gmock_build_dir) && cmake .. && make

$(gmock_exec) : $(bindir) $(gtest_libs) $(gmock_libs) $(test_files) src/point_cloud_gen.c
	g++ -I$(gtest_include_dir) -I$(gmock_include_dir) -Isrc \
		-L$(gtest_build_dir) -L$(gmock_build_dir) $(gmock_main) src/point_cloud_gen.c\
		$(test_files) -lgmock -lgtest -lpthread -o $(gmock_exec)

test : $(gmock_exec)
	$(gmock_exec)

.PHONY : all, clean, clear, run, submit, deploy, test, gen_data
