#ifndef CUTIL_VECTOR_PRIVATE_H
#define CUTIL_VECTOR_PRIVATE_H

#include "cutil/trait.h"
#include "cutil/allocator.h"

struct cutil_vector{
    void* data;
    size_t size;
    size_t capacity;
    cutil_trait* trait;
};

#endif
