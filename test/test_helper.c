#include "test_helper.h"

point const_generator(cloud_size_t, cloud_size_t) {
  point ret = { 0, 0 };
  return ret;
}

point incremental_generator(cloud_size_t index, cloud_size_t) {
  point ret = { index, index };
  return ret;
}
