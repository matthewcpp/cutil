#include "trait_tracker.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
	cutil_trait* tracked_trait;
	size_t copy_func_count;
	size_t destroy_func_count;
	size_t compare_func_count;
} cutil_test_trait_tracker;

int cutil_test_trait_tracker_compare(void* a, void* b, void* user_data) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)user_data;
	trait_tracker->compare_func_count += 1;

	return trait_tracker->tracked_trait->compare_func(a, b, trait_tracker->tracked_trait->user_data);
}

void cutil_test_trait_tracker_copy(void* dest, void* src, void* user_data) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)user_data;
	trait_tracker->copy_func_count += 1;

	trait_tracker->tracked_trait->copy_func(dest, src, trait_tracker->tracked_trait->user_data);
}

void cutil_test_trait_tracker_destroy(void* item, void* user_data) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)user_data;
	trait_tracker->destroy_func_count += 1;

	trait_tracker->tracked_trait->destroy_func(item, trait_tracker->tracked_trait->user_data);
}

cutil_trait* cutil_test_create_trait_tracker(cutil_trait* tracked_trait) {
	cutil_trait* trait = (cutil_trait*) calloc(1, sizeof(cutil_trait));
	cutil_test_trait_tracker* trait_tracker = calloc(1, sizeof(cutil_test_trait_tracker));

	trait_tracker->tracked_trait = tracked_trait;

	if (tracked_trait->compare_func) {
		trait->compare_func = cutil_test_trait_tracker_compare;
	}
	
	if (tracked_trait->copy_func) {
		trait->copy_func = cutil_test_trait_tracker_copy;
	}
	
	if (tracked_trait->destroy_func) {
		trait->destroy_func = cutil_test_trait_tracker_destroy;
	}

	trait->size = tracked_trait->size;
	trait->user_data = trait_tracker;
	
	return trait;
}

void cutil_test_destroy_trait_tracker(cutil_trait* trait) {
	free(trait->user_data);
	free(trait);
}

size_t cutil_test_trait_tracker_copy_count(cutil_trait* trait) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)trait->user_data;

	return trait_tracker->copy_func_count;
}

size_t cutil_test_trait_tracker_compare_count(cutil_trait* trait) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)trait->user_data;

	return trait_tracker->compare_func_count;
}

size_t cutil_test_trait_tracker_destroy_count(cutil_trait* trait) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)trait->user_data;

	return trait_tracker->destroy_func_count;
}

void cutil_test_trait_tracker_reset_counts(cutil_trait* trait) {
	cutil_test_trait_tracker* trait_tracker = (cutil_test_trait_tracker*)trait->user_data;

	trait_tracker->compare_func_count = 0;
	trait_tracker->copy_func_count = 0;
	trait_tracker->destroy_func_count = 0;
}
