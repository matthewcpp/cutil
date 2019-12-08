#ifndef CUTIL_ALLOCATOR_H
#define CUTIL_ALLOCATOR_H

/** \file allocator.h */

#include <stddef.h>

/**
Allocator Function for requesting new memory.
\param count the number of bytes that are requested.
\param user_data user specified data attached to the allocator object.
\returns pointer to the start of the allocated memory block or NULL if the memory could not be allocated.
*/
typedef void* (*cutil_allocator_malloc_func)(size_t count, void* user_data);

/**
Allocator Function for requesting new memory that is zero initialized.
\param count the number of items that are requested.
\param size the size of the item that is being requested.
\param user_data user specified data attached to the allocator object.
\returns pointer to the start of the allocated memory block or NULL if the memory could not be allocated.
*/
typedef void* (*cutil_allocator_calloc_func)(size_t count, size_t size, void* user_data);

/**
Allocator Function for reallocating memory.
\param ptr pointer to the start of a memory block to reallocate.
\param size new size of the allocated memory.
\param user_data user specified data attached to the allocator object.
\returns pointer to the start of the reallocated memory block or NULL if the memory could not be reallocated.  If reallocation fails, the initial memory remains valid and untouched.
*/
typedef void* (*cutil_allocator_realloc_func)(void* ptr, size_t size, void* user_data);

/**
Allocator Function for freeing memory.
\param ptr pointer to the start of a memory block to free.
\param user_data user specified data attached to the allocator object.
*/
typedef void (*cutil_allocator_free_func)(void* ptr, void* user_data);

typedef struct  {
    /** Function used for allocating new memory. */
	cutil_allocator_malloc_func malloc;

	/** Function used for allocating new zero-initialized memory. */
	cutil_allocator_calloc_func calloc;

	/** Function used for reallocating a block of memory. */
	cutil_allocator_realloc_func realloc;

	/** Function used for freeing a block of memory. */
	cutil_allocator_free_func free;
	void* user_data;
} cutil_allocator;

/**
Returns a default allocator.  Subsequent calls to this function will return the same object.
The Default allocator simply wraps the corresponding functions from the standard library.
\returns The default allocator.
*/
cutil_allocator* cutil_default_allocator();

/**
Frees resources used by the default allocator.
After this function is called, any pointers to the default allocator will become invalid.
*/
void cutil_default_allocator_destroy();

/**
Gets the current allocator that will be used for allocations by the cutil library.
Note that this function will return the default allocator if one has not been explicitly set.
\returns the current allocator used by the cutil library.
*/
cutil_allocator* cutil_current_allocator();

/**
Set the allocator that will be used for allocations by the cutil library.
It is the caller's responsibility to free the allocator object passed to this function.
*/
void cutil_set_current_allocator(cutil_allocator* allocator);

#endif
