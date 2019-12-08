#ifndef CUTIL_HEAP_H
#define CUTIL_HEAP_H

/** \file heap.h */

#include "trait.h"

#include <stddef.h>

typedef struct cutil_heap cutil_heap;

/**
Creates a new binary heap configured to store items as described by the passed in trait.
\param trait trait object describing the items that will be stored by the list.  This trait must define a comparison function.
\returns pointer to newly created list.  If creation failed then this function will return NULL.
*/
cutil_heap* cutil_heap_create(cutil_trait* trait);

/**
Destroys a heap, freeing all resources used by it.
*/
void cutil_heap_destroy(cutil_heap* heap);

/**
Inserts a new item into the heap.
\param data pointer to data of Type T* where T is the type described by the heap's trait.
*/
void cutil_heap_insert(cutil_heap* heap, void* data);

/**
Returns the number of items in the heap.
*/
size_t cutil_heap_size(cutil_heap* heap);

/**
Returns the heap's trait object.
*/
cutil_trait* cutil_heap_trait(cutil_heap* heap);

/**
Returns the top item of the heap.
\param data pointer to data of Type T* where T is the type described by the heap's trait.
\returns non zero value if the heap was not empty otherwise zero
*/
int cutil_heap_peek(cutil_heap* heap, void* data);


/**
Pops the top item from the heap
\return non-zero value if an item was removed from the heap, or zero if the heap was empty.
*/
int cutil_heap_pop(cutil_heap* heap);

/**
Resets the heap, leaving it in a fresh state.
This function will also deallocate the memory used by the internal buffer.
*/
void cutil_heap_reset(cutil_heap* heap);

/**
Clears all items in the vector, setting its size to zero.
This function will not deallocate any memory or adjust the vector's capacity.
*/
void cutil_heap_clear(cutil_heap* heap);


#endif
