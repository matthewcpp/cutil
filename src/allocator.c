#include "cutil/allocator.h"

#include <stdlib.h>

cutil_allocator* default_allocator = NULL;
cutil_allocator* current_allocator = NULL;

cutil_allocator* cutil_current_allocator() {
	if (!current_allocator){
		current_allocator = cutil_default_allocator();
	}

	return current_allocator;
}

void cutil_set_current_allocator(cutil_allocator* allocator) {
	current_allocator = allocator;
}

cutil_allocator* cutil_get_defult_allocator() {
	return default_allocator;
}

cutil_allocator* cutil_get_current_allocator() {
	return current_allocator;
}

void* default_allocator_malloc(size_t count, void* user_data) {
	(void)user_data;

	return malloc(count);
}

void* default_allocator_calloc(size_t count, size_t size, void* user_data) {
	(void)user_data;

	return calloc(count, size);
}

void* default_allocator_realloc(void* ptr, size_t size, void* user_data) {
	(void)user_data;

	return realloc(ptr, size);
}

void default_allocator_free(void* ptr, void* user_data) {
	(void)user_data;

	free(ptr);
}

cutil_allocator* cutil_default_allocator() {
	if (default_allocator == NULL) {
		default_allocator = malloc(sizeof(cutil_allocator));

		default_allocator->malloc = default_allocator_malloc;
		default_allocator->calloc = default_allocator_calloc;
		default_allocator->realloc = default_allocator_realloc;
		default_allocator->free = default_allocator_free;

		default_allocator->user_data = NULL;
	}

	return default_allocator;
}

void cutil_default_allocator_destroy() {
	if (default_allocator) {
		if (current_allocator == default_allocator) {
			current_allocator = NULL;
		}

		free(default_allocator);
		default_allocator = NULL;
	}
}
