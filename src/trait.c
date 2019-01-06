#include "trait.h"

#include <stdlib.h>
#include <string.h>

int cutil_trait_int_compare(void* a, void* b, void* user_data) {
	(void)user_data;
	int int_a = *(int*)a;
	int int_b = *(int*)b;

	if (int_a > int_b) {
		return 1;
	}
	else if (int_a < int_b) {
		return -1;
	}
	else {
		return 0;
	}
}

cutil_trait* cutil_trait_create_int() {
	cutil_trait* trait = (cutil_trait*)malloc(sizeof(cutil_trait));
	trait->compare_func = cutil_trait_int_compare;
	trait->pre_destroy_func = NULL;
	trait->copy_func = NULL;
	trait->user_data = NULL;
	trait->size = sizeof(int);

	return trait;
}

int cutil_trait_ptr_compare(void* a, void* b, void* user_data) {
	(void)user_data;

	void* a_ptr = *(void**)a;
	void* b_ptr = *(void**)b;

	if (a_ptr > b_ptr) {
		return 1;
	}
	else if (a_ptr < b_ptr) {
		return -1;
	}
	else {
		return 0;
	}
}

cutil_trait* cutil_trait_create_ptr() {
	cutil_trait* trait = (cutil_trait*)malloc(sizeof(cutil_trait));
	trait->compare_func = cutil_trait_ptr_compare;
	trait->pre_destroy_func = NULL;
	trait->copy_func = NULL;
	trait->user_data = NULL;
	trait->size = sizeof(void*);
	return trait;
}

int cutil_trait_cstring_compare(void* a, void* b, void* user_data) {
	(void)user_data;

	char* a_ptr = *((char**)a);
	char* b_ptr = *((char**)b);

	return strcmp(a_ptr, b_ptr);
}

void cutil_trait_cstring_copy(void* dest, void* src, void* user_data) {
	(void)user_data;

	char* src_ptr = *((char**)src);

	size_t buffer_size = strlen(src_ptr) + 1;
	char* str_copy = malloc(buffer_size);
	strncpy(str_copy, src_ptr, buffer_size);

	memcpy(dest, &str_copy, sizeof(char*));
}

void cutil_trait_cstring_pre_destroy(void* item, void* user_data) {
	(void)user_data;

	char* item_ptr = *((char**)item);
	free(item_ptr);
}

cutil_trait* cutil_trait_create_cstring() {
	cutil_trait* trait = (cutil_trait*)malloc(sizeof(cutil_trait));
	trait->compare_func = cutil_trait_cstring_compare;
	trait->pre_destroy_func = cutil_trait_cstring_pre_destroy;
	trait->copy_func = cutil_trait_cstring_copy;
	trait->user_data = NULL;
	trait->size = sizeof(char*);
	return trait;
}

static cutil_trait* defailt_int_trait = NULL;
static cutil_trait* default_ptr_trait = NULL;
static cutil_trait* default_ctring_trait = NULL;

cutil_trait* cutil_trait_int() {
	if (!defailt_int_trait) {
		defailt_int_trait = cutil_trait_create_int();
	}

	return defailt_int_trait;
}

cutil_trait* cutil_trait_ptr() {
	if (!default_ptr_trait) {
		default_ptr_trait = cutil_trait_create_ptr();
	}

	return default_ptr_trait;
}

cutil_trait* cutil_trait_cstring() {
	if (!default_ctring_trait) {
		default_ctring_trait = cutil_trait_create_cstring();
	}

	return default_ctring_trait;
}

void cutil_trait_destroy() {
	if (defailt_int_trait) {
		free(defailt_int_trait);
		defailt_int_trait = NULL;
	}

	if (default_ptr_trait) {
		free(default_ptr_trait);
		default_ptr_trait = NULL;
	}

	if (default_ctring_trait) {
		free(default_ctring_trait);
		default_ctring_trait = NULL;
	}
}