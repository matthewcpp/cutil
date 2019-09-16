#ifndef CUTIL_TEST_HEAP_UTIL_H

#include "cutil/heap.h"
#include "heap_private.h"

int validate_heap(cutil_heap* heap);
int heaps_are_equal(cutil_heap* a, cutil_heap* b);
cutil_heap* create_heap_from_array(int* values, int count);

#endif
