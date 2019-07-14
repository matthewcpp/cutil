#include "cutil/vector.h"

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

void cutil_vector_destroy(cutil_vector* vector) {
	cutil_vector_reset(vector);
    free(vector);
}

void cutil_vector_clear(cutil_vector* vector) {
	if (vector->_data && vector->_trait->destroy_func) {
		size_t i;
		
		for (i = 0; i < vector->_size; i++) {
			void* object = (char*)vector->_data + (i * vector->_trait->size);

			vector->_trait->destroy_func(object, vector->_trait->user_data);
		}
	}

	vector->_size = 0;
}

void cutil_vector_reset(cutil_vector* vector) {
	cutil_vector_clear(vector);

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

int _grow_vector(cutil_vector* vector) {
	if (vector->_size == vector->_capacity) {
		size_t new_capacity = (vector->_capacity > 0) ? vector->_capacity * 2U : 1U;
		void* new_data = realloc(vector->_data, vector->_trait->size * new_capacity);

		if (new_data) {
			vector->_capacity = new_capacity;
			vector->_data = new_data;

			return 1;
		}
		else {
			return 0;
		}
	}

	return 1;
}

void cutil_vector_push_back(cutil_vector* vector, void* data) {
	if (_grow_vector(vector)) {
		size_t byte_offset = vector->_size * vector->_trait->size;
		char* location = ((char*)vector->_data) + byte_offset;

		if (vector->_trait->copy_func) {
			vector->_trait->copy_func(location, data, vector->_trait->user_data);
		}
		else {
			memcpy(location, data, vector->_trait->size);
		}

		vector->_size += 1;
	}
}

void cutil_vector_pop_back(cutil_vector* vector) {
	if (vector->_size > 0) {

		if (vector->_trait->destroy_func) {
			void* object = (char*)vector->_data + ((vector->_size - 1) * vector->_trait->size);

			vector->_trait->destroy_func(object, vector->_trait->user_data);
		}
		
		vector->_size -= 1;
	}
}

int cutil_vector_get(cutil_vector* vector, unsigned int index, void* out) {
	if (index < vector->_size) {
		size_t byte_offset = index * vector->_trait->size;

        memcpy(out, (char*)vector->_data + byte_offset, vector->_trait->size);

		return 1;
	}
	else {
		return 0;
	}
}

cutil_trait* cutil_vector_trait(cutil_vector* vector) {
	return vector->_trait;
}

size_t cutil_vector_capacity(cutil_vector* vector) {
	return vector->_capacity;
}
