#ifndef CUTIL_VECTOR_H
#define CUTIL_VECTOR_H

#include "trait.h"

#include <stdbool.h>

typedef struct cutil_vector {
	void* _data;
	unsigned int _size;
	unsigned int _capacity;
	cutil_trait* _trait;

#ifdef CUTIL_DEBUGGING
    bool _debug_malloc;
#endif
} cutil_vector;

cutil_vector* cutil_vector_create(cutil_trait* trait);

void cutil_vector_init(cutil_vector* vector, cutil_trait* trait);

void cutil_vector_uninit(cutil_vector* vector);
void cutil_vector_destroy(cutil_vector* vector);

unsigned int cutil_vector_size(cutil_vector* vector);
void cutil_vector_clear(cutil_vector* vector);
void cutil_vector_push(cutil_vector* vector, void* data);
void cutil_vector_pop(cutil_vector* vector);
bool cutil_vector_get(cutil_vector* vector, unsigned int index, void* out);
cutil_trait* cutil_vector_get_trait(cutil_vector* vector);

#endif
