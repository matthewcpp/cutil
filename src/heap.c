#include "cutil/heap.h"
#include "heap_private.h"
#include "vector_private.h"

#include <string.h>

#define INVALID_CHILD_INDEX ((size_t)(-1))

cutil_heap* cutil_heap_create(cutil_trait* trait) {
    cutil_heap* heap = NULL;
	if (trait->compare_func == NULL) return heap;

	heap = malloc(sizeof(cutil_heap));
	heap->vector = cutil_vector_create(trait);

	return heap;
}

size_t _cutil_heap_get_parent_index(size_t index) {
	return (index - 1) / 2;
}

size_t _cutil_heap_smallest_child_index(cutil_heap* heap, size_t index) {
	size_t heap_size = cutil_vector_size(heap->vector);
	char* data = cutil_vector_data(heap->vector);
	cutil_trait* trait = cutil_vector_trait(heap->vector);

	size_t child1_index = 2 * index + 1;
	size_t child2_index = child1_index + 1;

	char* child1, *child2;

	if (child1_index >= heap_size) {
		return INVALID_CHILD_INDEX;
	}
	else if (child2_index >= heap_size) {
		return child1_index;
	}

	child1 = data + child1_index * trait->size;
	child2 = data + child2_index * trait->size;

	if (trait->compare_func(child1, child2, trait->user_data) < 0)
		return child1_index;
	else
		return child2_index;
}

int cutil_heap_pop(cutil_heap* heap) {
	char* data = cutil_vector_data(heap->vector);
	cutil_trait* trait = cutil_vector_trait(heap->vector);
	char* swap_space;
	size_t current_index = 0;

	if (cutil_vector_size(heap->vector) == 0) {
		return 0;
	}

	if (heap->vector->_trait->destroy_func != NULL) {
	    heap->vector->_trait->destroy_func(heap->vector->_data, heap->vector->_trait->user_data);
	}

	heap->vector->_size -= 1;

	if (heap->vector->_size > 0) {
		swap_space = malloc(trait->size);

		/* move the last item to the top of the heap and trickle down */
		/* note we do not use pop_back due to the fact it will trigger a destructor if one is defined */
		memcpy(data, data + (heap->vector->_size) * trait->size, trait->size);


		while (1) {
			/* determine the child that the item will potentially trickle down to */
			size_t child_index = _cutil_heap_smallest_child_index(heap, current_index);

			if (child_index != INVALID_CHILD_INDEX) {
				char* current_item = data + current_index * trait->size;
				char* child = data + child_index * trait->size;

				/* if the current item is greater than the smallest child swap them otherwise stop*/
				if (trait->compare_func(current_item, child, trait->user_data) >= 1) {
					memcpy(swap_space, current_item, trait->size);
					memcpy(current_item, child, trait->size);
					memcpy(child, swap_space, trait->size);
				}
				else {
					break;
				}
			}
			else {
				break;
			}

			current_index = child_index;
		}

		free(swap_space);
	}

	return 1;
}

void cutil_heap_destroy(cutil_heap* heap) {
	cutil_vector_destroy(heap->vector);
	free(heap);
}

size_t cutil_heap_size(cutil_heap* heap) {
	return cutil_vector_size(heap->vector);
}

void cutil_heap_insert(cutil_heap* heap, void* data) {
	size_t index = cutil_vector_size(heap->vector);

	cutil_vector_push_back(heap->vector, data);

	if (index > 0) {
		cutil_trait* trait = cutil_vector_trait(heap->vector);
		char* buffer = cutil_vector_data(heap->vector);

		char* swap_space = malloc(trait->size);

		while (index > 0) {
			size_t parent = _cutil_heap_get_parent_index(index);
			char* parent_item = buffer + parent * trait->size;
			char* current_item = buffer + index * trait->size;

			/* if the current item is less than it's parent swap the two */
			if (trait->compare_func(current_item, parent_item, trait->user_data) < 0) {
				memcpy(swap_space, current_item, trait->size);
				memcpy(current_item, parent_item, trait->size);
				memcpy(parent_item, swap_space, trait->size);
			}
			else {
				break;
			}

			index = parent;
		}

		free(swap_space);
	}
}

cutil_trait* cutil_heap_trait(cutil_heap* heap) {
	return cutil_vector_trait(heap->vector);
}

int cutil_heap_peek(cutil_heap* heap, void* data) {
    return cutil_vector_get(heap->vector, 0, data);
}

void cutil_heap_reset(cutil_heap* heap) {
	cutil_vector_reset(heap->vector);
}

void cutil_heap_clear(cutil_heap* heap) {
	cutil_vector_clear(heap->vector);
}
