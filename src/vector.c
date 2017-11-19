#include "vector.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

void cutil_vector_init(cutil_vector *vector, unsigned int item_size) {
	vector->_item_size = item_size;
	vector->data = NULL;
	cutil_vector_clear(vector);
}

void cutil_vector_initp(cutil_vector *vector) {
	cutil_vector_init(vector, sizeof(uintptr_t));
}

void cutil_vector_destroy(cutil_vector *vector) {
	cutil_vector_clear(vector);
}

void cutil_vector_clear(cutil_vector *vector) {
	if (vector->data) {
		free(vector->data);
	}

	vector->_capacity = 1;
	vector->_size = 0;
	vector->data = malloc(vector->_item_size);
}

void _grow_vector(cutil_vector *vector) {
	if (vector->_size == vector->_capacity) {
		unsigned int new_capacity = vector->_capacity * 2;

		void* new_data = malloc(vector->_item_size * new_capacity);
		memcpy(new_data, vector->data, vector->_size * vector->_item_size);
		free(vector->data);

		vector->_capacity = new_capacity;
		vector->data = new_data;
	}
}

void cutil_vector_push(cutil_vector *vector, void* data) {
	_grow_vector(vector);

	unsigned int byte_offset = vector->_size * vector->_item_size;
	memcpy((char*)vector->data + (byte_offset), data, vector->_item_size);

	vector->_size += 1;
}

void cutil_vector_pushp(cutil_vector *vector, void* data) {
	uintptr_t int_ptr = (uintptr_t)data;
	cutil_vector_push(vector, &int_ptr);
}

void cutil_vector_pop(cutil_vector *vector) {
	if (vector->_size > 0) {
		vector->_size -= 1;
	}
}

bool cutil_vector_get(cutil_vector *vector, unsigned int index, void *out) {
	if (index < vector->_size) {
		unsigned int byte_offset = index * vector->_item_size;
		memcpy(out, (char*)vector->data + byte_offset, vector->_item_size);

		return true;
	}
	else {
		return false;
	}
}

bool cutil_vector_getp(cutil_vector *vector, unsigned int index, void **out) {
	uintptr_t ptr;
	if (cutil_vector_get(vector, index, &ptr)) {
		*out = (void *)ptr;
		return true;
	}
	else {
		*out = NULL;
		return false;
	}
}