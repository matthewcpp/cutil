#pragma once
#include <stdbool.h>

typedef struct cutil_vector {
	void *data;
	unsigned int size;
	unsigned int _capacity;
	unsigned int _item_size;
} cutil_vector;

void cutil_vector_init(cutil_vector *vector, unsigned int item_size);
void cutil_vector_initp(cutil_vector *vector);

void cutil_vector_destroy(cutil_vector *vector);
void cutil_vector_clear(cutil_vector *vector);

void cutil_vector_push(cutil_vector *vector, void* data);
void cutil_vector_pushp(cutil_vector *vector, void* data);

void cutil_vector_pop(cutil_vector *vector);

bool cutil_vector_get(cutil_vector *vector, unsigned int index, void *out);
bool cutil_vector_getp(cutil_vector *vector, unsigned int index, void **out);