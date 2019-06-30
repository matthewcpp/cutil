#ifndef CUTIL_TRAIT_H
#define CUTIL_TRAIT_H

/** \file trait.h */

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
Returns a trait describing a 32 bit signed integer
Subsequent calls to this function will return the same trait.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_int();

/**
Returns a trait describing a generic pointer.
Pointed to data will not be copied when inseted into a container.
The caller should take care to free resources consumed by pointed to data before the pointer goes out of scope.
Subsequent calls to this function will return the same trait object.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_ptr();

/**
Returns a trait describing a basic cstring.
cstring data will be copied when a a container using this trait receives a new cstring.  
The pointed to string will be freed when the pointer goes out of scope.
Subsequent calls to this function will return the same trait object.
\see cutil_trait_destroy()
*/
cutil_trait* cutil_trait_cstring();


/**
Releases all resources used by the built in default trait functions (cutil_trait_xxx).
User created traits will not be destroyed by this function.
After calling this method all traits that were created using the cutil_trait_xxx functions will be invalid.
*/
void cutil_trait_destroy();

#endif
