#include "trait.h"

#include <stdlib.h>
#include <string.h>

void cutil_trait_default_copy(void* dest, void* src, void* user_data) {
	memcpy(dest, src, ((cutil_trait*)user_data)->size);
}

void cutil_trait_default_pre_destroy(void* item, void* user_data) {
	(void)item;
	(void)user_data;
}

void cutil_trait_default_delete_pre_destroy(void* item, void* user_data) {
	(void)user_data;
	free(*(void**)item);
}

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
	trait->pre_destroy_func = cutil_trait_default_pre_destroy;
	trait->copy_func = cutil_trait_default_copy;
	trait->user_data = trait;
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
	trait->pre_destroy_func = cutil_trait_default_pre_destroy;
	trait->copy_func = cutil_trait_default_copy;
	trait->user_data = trait;
	trait->size = sizeof(void*);
	return trait;
}

int cutil_trait_cstring_compare(void* a, void* b, void* user_data) {
	(void)user_data;

	return strcmp(*(char**)a, *(char**)b);
}

void cutil_trait_cstring_copy(void* dest, void* src, void* user_data) {
	(void)user_data;

	char** dest_ptr = (char**)dest;
	char* src_ptr = *(char**)src;

	size_t buffer_size = strlen(src_ptr) + 1;
	*dest_ptr = malloc(buffer_size);
	strncpy(*dest_ptr, src_ptr, buffer_size);
}

cutil_trait* cutil_trait_create_cstring() {
	cutil_trait* trait = (cutil_trait*)malloc(sizeof(cutil_trait));
	trait->compare_func = cutil_trait_cstring_compare;
	trait->pre_destroy_func = cutil_trait_default_delete_pre_destroy;
	trait->copy_func = cutil_trait_cstring_copy;
	trait->user_data = trait;
	trait->size = sizeof(char*);
	return trait;
}