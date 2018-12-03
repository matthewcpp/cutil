#ifndef CUTIL_TRAIT_H
#define CUTIL_TRAIT_H

#include <stddef.h>

typedef int (*cutil_trait_compare_func)(void* a, void* b, void* user_data);
typedef void (*cutil_trait_pre_destroy_func)(void* item, void* user_data);
typedef void (*cutil_trait_copy_func)(void* dest, void* src, void* user_data);

typedef struct {
	cutil_trait_compare_func compare_func;
	cutil_trait_pre_destroy_func pre_destroy_func;
	cutil_trait_copy_func copy_func;
	size_t size;

	void* user_data;
} cutil_trait;

/** 
Creates a new trait describing a signed integer for the current system.
The object returned by this method should be freed by the caller;
*/
cutil_trait* cutil_trait_create_int();

/**
Creates a new trait describing a generic for the current system.
The data pointed to the pointer is not freed when the pointer is about to be destroyed.
The object returned by this method should be freed by the caller;
*/
cutil_trait* cutil_trait_create_ptr();

#endif
