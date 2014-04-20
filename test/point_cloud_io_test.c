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

TEST(load_point_cloud, reads_a_point_cloud_by_parsing_the_header_and_setting_the_correct_cloud_size) {
  FILE *in_stream = tmpfile();
  fprintf(in_stream, "# size=3\n");
  fprintf(in_stream, "12345\t67890\n");
  fprintf(in_stream, "45678\t90123\n");
  fprintf(in_stream, "86793\t57364\n");
  rewind(in_stream);

  point_cloud test_cloud;
  load_point_cloud(in_stream, &test_cloud);

  ASSERT_EQ(cloud_size_t(3), test_cloud.size);
}

TEST(load_point_cloud, reads_a_point_cloud_by_storing_all_the_points) {
  FILE *in_stream = tmpfile();
  fprintf(in_stream, "# size=3\n");
  fprintf(in_stream, "12345\t67890\n");
  fprintf(in_stream, "45678\t90123\n");
  fprintf(in_stream, "86793\t57364\n");
  rewind(in_stream);

  point_cloud test_cloud;
  load_point_cloud(in_stream, &test_cloud);

  ASSERT_EQ(coord_t(12345), test_cloud.points[0].x);
  ASSERT_EQ(coord_t(67890), test_cloud.points[0].y);

  ASSERT_EQ(coord_t(86793), test_cloud.points[2].x);
  ASSERT_EQ(coord_t(57364), test_cloud.points[2].y);
}
