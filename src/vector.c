#include "vector.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct cutil_vector{
	void* _data;
	size_t _size;
	size_t _capacity;
	cutil_trait* _trait;
};

cutil_vector* cutil_vector_create(cutil_trait* trait){
	cutil_vector* vector = malloc(sizeof(cutil_vector));
    
	vector->_trait = trait;
	vector->_data = NULL;
	vector->_capacity = 0;
	vector->_size = 0;

	return vector;
}

void cutil_vector_uninit(cutil_vector* vector){
    cutil_vector_clear(vector);
}

void cutil_vector_destroy(cutil_vector* vector) {
    cutil_vector_uninit(vector);
    free(vector);
}

void cutil_vector_clear(cutil_vector* vector) {
	if (vector->_data) {
		free(vector->_data);
	}

	vector->_capacity = 0;
	vector->_size = 0;
	vector->_data = NULL;
}

size_t cutil_vector_size(cutil_vector* vector){
    return vector->_size;
}

void _grow_vector(cutil_vector* vector) {
	if (vector->_size == vector->_capacity) {
		size_t new_capacity = (vector->_capacity > 0) ? vector->_capacity * 2U : 1U;

		void* new_data = malloc(vector->_trait->size * new_capacity);
		memcpy(new_data, vector->_data, vector->_size * vector->_trait->size);

		if(vector->_data){
			free(vector->_data);
		}

		vector->_capacity = new_capacity;
		vector->_data = new_data;
	}
}

void cutil_vector_push(cutil_vector* vector, void* data) {
	_grow_vector(vector);

	size_t byte_offset = vector->_size * vector->_trait->size;
	vector->_trait->copy_func((char*)vector->_data + (byte_offset), data, vector->_trait->user_data);

	vector->_size += 1;
}

void cutil_vector_pop(cutil_vector* vector) {
	if (vector->_size > 0) {
		void* object = (char*)vector->_data + ((vector->_size - 1) * vector->_trait->size);
		vector->_trait->pre_destroy_func(object, vector->_trait->user_data);
		vector->_size -= 1;
	}
}

bool cutil_vector_get(cutil_vector* vector, unsigned int index, void* out) {
	if (index < vector->_size) {
		size_t byte_offset = index * vector->_trait->size;
		memcpy(out, (char*)vector->_data + byte_offset, vector->_trait->size);

		return true;
	}
	else {
		return false;
	}
}

cutil_trait* cutil_vector_trait(cutil_vector* vector) {
	return vector->_trait;
}

size_t cutil_vector_capacity(cutil_vector* vector) {
	return vector->_capacity;
}
