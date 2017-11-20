#include "vector.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

cutil_vector *cutil_vector_create(unsigned int item_size){
    cutil_vector *vector = malloc(sizeof(cutil_vector));
    cutil_vector_init(vector, item_size);

#ifdef CUTIL_DEBUGGING
    vector->_debug_malloc = true;
#endif

    return vector;
}

cutil_vector *cutil_vector_createp(){
    cutil_vector * vector =  cutil_vector_create(sizeof(uintptr_t));

#ifdef CUTIL_DEBUGGING
    vector->_debug_ptr = true;
#endif

    return vector;
}

void cutil_vector_init(cutil_vector *vector, unsigned int item_size) {
	vector->_item_size = item_size;
	vector->_data = NULL;
    vector->_capacity = 0;
    vector->_size = 0;

#ifdef CUTIL_DEBUGGING
    vector->_debug_malloc = false;
    vector->_debug_ptr = false;
#endif
}

void cutil_vector_initp(cutil_vector *vector) {
	cutil_vector_init(vector, sizeof(uintptr_t));

#ifdef CUTIL_DEBUGGING
    vector->_debug_ptr = true;
#endif
}

void cutil_vector_uninit(cutil_vector *vector){
    cutil_vector_clear(vector);
}

void cutil_vector_destroy(cutil_vector *vector) {
#ifdef CUTIL_DEBUGGING
    assert(vector->_debug_malloc);
#endif

    cutil_vector_uninit(vector);
    free(vector);
}

void cutil_vector_clear(cutil_vector *vector) {
	if (vector->_data) {
		free(vector->_data);
	}

	vector->_capacity = 0;
	vector->_size = 0;
	vector->_data = NULL;
}

unsigned int cutil_vector_size(cutil_vector *vector){
    return vector->_size;
}

void _grow_vector(cutil_vector *vector) {
	if (vector->_size == vector->_capacity) {
		unsigned int new_capacity = (vector->_capacity > 0) ? vector->_capacity * 2 : 1;

		void* new_data = malloc(vector->_item_size * new_capacity);
		memcpy(new_data, vector->_data, vector->_size * vector->_item_size);

		if(vector->_data){
			free(vector->_data);
		}

		vector->_capacity = new_capacity;
		vector->_data = new_data;
	}
}

void cutil_vector_push(cutil_vector *vector, void* data) {
	_grow_vector(vector);

	unsigned int byte_offset = vector->_size * vector->_item_size;
	memcpy((char*)vector->_data + (byte_offset), data, vector->_item_size);

	vector->_size += 1;
}

void cutil_vector_pushp(cutil_vector *vector, void* data) {
#ifdef CUTIL_DEBUGGING
    assert(vector->_debug_ptr);
#endif

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
		memcpy(out, (char*)vector->_data + byte_offset, vector->_item_size);

		return true;
	}
	else {
		return false;
	}
}

bool cutil_vector_getp(cutil_vector *vector, unsigned int index, void **out) {
#ifdef CUTIL_DEBUGGING
    assert(vector->_debug_ptr);
#endif

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