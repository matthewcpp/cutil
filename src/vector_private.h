#ifndef CUTIL_VECTOR_PRIVATE_H
#define CUTIL_VECTOR_PRIVATE_H

struct cutil_vector{
    void* _data;
    size_t _size;
    size_t _capacity;
    cutil_trait* _trait;
};

#endif
