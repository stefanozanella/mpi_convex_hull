# vim: noexpandtab

test_files = test/*_test.c $(libs_objects:.o=.c) test/test_helper.c

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

$(gmock_exec) : $(bindir) $(gtest_libs) $(gmock_libs) $(test_files)
	g++ -I$(gtest_include_dir) -I$(gmock_include_dir) -I$(srcdir)\
		-L$(gtest_build_dir) -L$(gmock_build_dir) $(gmock_main)\
		$(test_files) -lgmock -lgtest -lpthread -o $(gmock_exec)

test : $(gmock_exec)
	$(gmock_exec)

.PHONY: test
