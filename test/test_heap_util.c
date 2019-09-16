#include "test_heap_util.h"

#include "test_settings.h"
#include "test_util/defs.h"

#include "heap_private.h"

#include <stdio.h>

/* Precondition: heap should not be null! */
int validate_heap(cutil_heap* heap) {
	size_t heap_size = cutil_heap_size(heap);
	cutil_trait* trait = cutil_heap_trait(heap);
	size_t i;

	char* work_space, *current_item, *parent_item;
	int result = 1;

	if (heap_size <= 1) {
		return result;
	}

	work_space = malloc(trait->size * 2);
	current_item = work_space;
	parent_item = current_item + trait->size;

	for (i = 1; i < heap_size; i++) {
		cutil_vector_get(heap->vector, i, &current_item);
		cutil_vector_get(heap->vector, _cutil_heap_get_parent_index(i), &parent_item);

		if (trait->compare_func(&parent_item, &current_item, trait->user_data) > 0) {
			result = 0;
			break;
		}
	}

	free(work_space);

	return result;
}

cutil_heap* create_heap_from_array(int* values, int count) {
	cutil_heap* heap = cutil_heap_create(cutil_trait_int());
	int i;

	for (i = 0; i < count; i++) {
		cutil_vector_push_back(heap->vector, values + i);
	}

	return heap;
}

int heaps_are_equal(cutil_heap* a, cutil_heap* b) {
	return cutil_vector_equals(a->vector, b->vector);
}
