#ifndef CUTIL_VECTOR_PRIVATE_H
#define CUTIL_VECTOR_PRIVATE_H

struct cutil_vector{
    void* data;
    size_t size;
    size_t capacity;
    cutil_trait* trait;
};

#endif
