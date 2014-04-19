#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_helper.h"
#include "point_cloud_io.h"

TEST(save_point_cloud, serializes_a_point_cloud_by_writing_a_header_containing_the_cloud_size) {
  options opts = { 34, &incremental_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  FILE *out_stream = tmpfile();

  save_point_cloud(&test_cloud, out_stream);

  rewind(out_stream);
  char header[256];
  fgets(header, 256, out_stream);

  ASSERT_STREQ("# size=34\n", header);

  fclose(out_stream);
}

TEST(save_point_cloud, serializes_a_point_cloud_by_writing_each_point_on_a_separate_line) {
  options opts = { 3, &incremental_generator };
  point_cloud test_cloud;
  generate_point_cloud(&test_cloud, opts);

  FILE *out_stream = tmpfile();

  save_point_cloud(&test_cloud, out_stream);

  rewind(out_stream);
  char line[256];
  // Header, we can discard it for this test
  fgets(line, 256, out_stream);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("0\t0\n", line);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("1\t1\n", line);

  fgets(line, 256, out_stream);
  ASSERT_STREQ("2\t2\n", line);

  fclose(out_stream);
}
