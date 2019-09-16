#include "cutil/vector.h"
#include "vector_private.h"

#include <stdlib.h>
#include <string.h>

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
		vector->_data = NULL;
	}

	vector->_capacity = 0;
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

void* _get_object(cutil_vector* vector, size_t index) {
	return (char*)vector->_data + (index * vector->_trait->size);
}

void _set_object(cutil_vector* vector, void* location, void* data) {
	if (vector->_trait->copy_func) {
		vector->_trait->copy_func(location, data, vector->_trait->user_data);
	}
	else {
		memcpy(location, data, vector->_trait->size);
	}
}

void cutil_vector_push_back(cutil_vector* vector, void* data) {
	if (_grow_vector(vector)) {
		void* location = _get_object(vector, vector->_size);
		_set_object(vector, location, data);

		vector->_size += 1;
	}
}


void cutil_vector_pop_back(cutil_vector* vector) {
	if (vector->_size > 0) {

		if (vector->_trait->destroy_func) {
			void* object = _get_object(vector, vector->_size - 1);
			vector->_trait->destroy_func(object, vector->_trait->user_data);
		}
		
		vector->_size -= 1;
	}
}

int cutil_vector_get(cutil_vector* vector, size_t index, void* out) {
	if (index < vector->_size) {
		memcpy(out, _get_object(vector, index), vector->_trait->size);

		return 1;
	}
	else {
		return 0;
	}
}

int cutil_vector_set(cutil_vector* vector, size_t index, void* data) {
	if (index < vector->_size) {
		void* object = _get_object(vector, index);

		if (vector->_trait->destroy_func) {
			vector->_trait->destroy_func(object, vector->_trait->user_data);
		}

		_set_object(vector, object, data);

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

void* cutil_vector_data(cutil_vector* vector) {
	return vector->_data;
}

int cutil_vector_equals(cutil_vector* a, cutil_vector* b) {
	size_t i;

	char* a_data = a->_data;
	char* b_data = b->_data;

	if (a->_trait != b->_trait) return 0;
	if (a->_size != b->_size) return 0;

	if (a->_trait->compare_func != NULL) {
        for (i = 0; i < a->_size; i++) {
            if (a->_trait->compare_func(a_data + i * a->_trait->size, b_data + i * b->_trait->size, a->_trait->user_data) != 0)
                return 0;
        }

        return 1;
	}
	else {
	    return memcmp(a->_data, b->_data, a->_size * a->_trait->size) == 0;
	}
}
