#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#include "point_cloud.h"
#include "generate_point_cloud.h"

point const_generator(cloud_size_t, cloud_size_t);
point incremental_generator(cloud_size_t index, cloud_size_t); 

#endif
