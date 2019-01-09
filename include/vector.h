#ifndef CUTIL_VECTOR_H
#define CUTIL_VECTOR_H

#include "trait.h"

typedef struct cutil_vector cutil_vector;

cutil_vector* cutil_vector_create(cutil_trait* trait);

void cutil_vector_uninit(cutil_vector* vector);
void cutil_vector_destroy(cutil_vector* vector);

size_t cutil_vector_size(cutil_vector* vector);
void cutil_vector_clear(cutil_vector* vector);
void cutil_vector_push(cutil_vector* vector, void* data);
void cutil_vector_pop(cutil_vector* vector);
int cutil_vector_get(cutil_vector* vector, unsigned int index, void* out);

cutil_trait* cutil_vector_trait(cutil_vector* vector);
size_t cutil_vector_capacity(cutil_vector* vector);

#endif
