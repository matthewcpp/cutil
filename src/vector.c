#include "cutil/vector.h"
#include "cutil/allocator.h"

#include "vector_private.h"

#include <stdlib.h>
#include <string.h>

cutil_vector* cutil_vector_create(cutil_trait* trait) {
	cutil_allocator* allocator = cutil_current_allocator();
	cutil_vector* vector = allocator->malloc(sizeof(cutil_vector), allocator->user_data);

	vector->trait = trait;
	vector->data = NULL;
	vector->capacity = 0;
	vector->size = 0;

	return vector;
}

void cutil_vector_destroy(cutil_vector* vector) {
	cutil_allocator* allocator = cutil_current_allocator();

	cutil_vector_reset(vector);
	allocator->free(vector, allocator->user_data);
}

void cutil_vector_clear(cutil_vector* vector) {
	if (vector->data && vector->trait->destroy_func) {
		size_t i;
		
		for (i = 0; i < vector->size; i++) {
			void* object = (char*)vector->data + (i * vector->trait->size);

			vector->trait->destroy_func(object, vector->trait->user_data);
		}
	}

	vector->size = 0;
}

void cutil_vector_reset(cutil_vector* vector) {
	cutil_allocator* allocator = cutil_current_allocator();
	cutil_vector_clear(vector);

	if (vector->data) {
		allocator->free(vector->data, allocator->user_data);
		vector->data = NULL;
	}

	vector->capacity = 0;
}

size_t cutil_vector_size(cutil_vector* vector){
    return vector->size;
}

int _grow_vector(cutil_vector* vector) {
	cutil_allocator* allocator = cutil_current_allocator();

	if (vector->size == vector->capacity) {
		size_t new_capacity = (vector->capacity > 0) ? vector->capacity * 2U : 1U;
		void* new_data = allocator->realloc(vector->data, vector->trait->size * new_capacity, allocator->user_data);

		if (new_data) {
			vector->capacity = new_capacity;
			vector->data = new_data;

			return 1;
		}
		else {
			return 0;
		}
	}

	return 1;
}

void* _get_object(cutil_vector* vector, size_t index) {
	return (char*)vector->data + (index * vector->trait->size);
}

void _set_object(cutil_vector* vector, void* location, void* data) {
	if (vector->trait->copy_func) {
		vector->trait->copy_func(location, data, vector->trait->user_data);
	}
	else {
		memcpy(location, data, vector->trait->size);
	}
}

void cutil_vector_push_back(cutil_vector* vector, void* data) {
	if (_grow_vector(vector)) {
		void* location = _get_object(vector, vector->size);
		_set_object(vector, location, data);

		vector->size += 1;
	}
}


void cutil_vector_pop_back(cutil_vector* vector) {
	if (vector->size > 0) {

		if (vector->trait->destroy_func) {
			void* object = _get_object(vector, vector->size - 1);
			vector->trait->destroy_func(object, vector->trait->user_data);
		}
		
		vector->size -= 1;
	}
}

int cutil_vector_get(cutil_vector* vector, size_t index, void* out) {
	if (index < vector->size) {
		memcpy(out, _get_object(vector, index), vector->trait->size);

		return 1;
	}
	else {
		return 0;
	}
}

int cutil_vector_set(cutil_vector* vector, size_t index, void* data) {
	if (index < vector->size) {
		void* object = _get_object(vector, index);

		if (vector->trait->destroy_func) {
			vector->trait->destroy_func(object, vector->trait->user_data);
		}

		_set_object(vector, object, data);

		return 1;
	}
	else {
		return 0;
	}
}

cutil_trait* cutil_vector_trait(cutil_vector* vector) {
	return vector->trait;
}

size_t cutil_vector_capacity(cutil_vector* vector) {
	return vector->capacity;
}

void* cutil_vector_data(cutil_vector* vector) {
	return vector->data;
}

int cutil_vector_equals(cutil_vector* a, cutil_vector* b) {
	size_t i;

	char* a_data = a->data;
	char* b_data = b->data;

	if (a->trait != b->trait) return 0;
	if (a->size != b->size) return 0;

	if (a->trait->compare_func != NULL) {
        for (i = 0; i < a->size; i++) {
            if (a->trait->compare_func(a_data + i * a->trait->size, b_data + i * b->trait->size, a->trait->user_data) != 0)
                return 0;
        }

        return 1;
	}
	else {
	    return memcmp(a->data, b->data, a->size * a->trait->size) == 0;
	}
}
