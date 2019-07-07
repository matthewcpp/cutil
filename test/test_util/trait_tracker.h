#ifndef TEST_TRAIT_TRACKER_H
#define TEST_TRAIT_TRACKER_H

#include "cutil/trait.h"

#include <stddef.h>

cutil_trait* cutil_test_create_trait_tracker(cutil_trait* trait);
void cutil_test_destroy_trait_tracker(cutil_trait* trait);

size_t cutil_test_trait_tracker_copy_count(cutil_trait* trait);
size_t cutil_test_trait_tracker_compare_count(cutil_trait* trait);
size_t cutil_test_trait_tracker_destroy_count(cutil_trait* trait);

#endif 
