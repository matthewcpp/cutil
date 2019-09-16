#ifndef CUTIL_BTREE_PRIVATE_H
#define CUTIL_BTREE_PRIVATE_H

#include "cutil/vector.h"

#include <stdlib.h>

struct cutil_heap {
	cutil_vector* vector;
};

size_t _cutil_heap_get_parent_index(size_t index);

#endif 
