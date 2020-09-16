#ifndef CUTIL_VECTOR_H
#define CUTIL_VECTOR_H

/** \file vector.h */

#include "trait.h"

/**
The vector is an array that can grow dynamically in size as items are added to it.
Items in the vecotr are stored contiguously in memory.
*/
typedef struct cutil_vector cutil_vector;

/**
Creates a new vector configured to store items as described by the passed in trait.
\param trait trait object describing the items that will be stored by the vector.
\returns pointer to newly created vector object.  If creation failed then this function will return NULL.
*/
cutil_vector* cutil_vector_create(cutil_trait* trait);

/**
Destroys a vector, freeing all resources used by it.
If the vector's trait includes a destroy function, it will be called for every item in the vector.
\param vector vector that will be destroyed.
*/
void cutil_vector_destroy(cutil_vector* vector);

/**
Returns the number of items currently in the vector.
*/
size_t cutil_vector_size(cutil_vector* vector);

/**
Returns a non-zero value if the vector is currently empty or 0 if the vector has items in it.
 */
int cutil_vector_empty(cutil_vector* vector);

/**
Clears all items in the vector, setting its size to zero.
If the vector's trait includes a destroy function, it will be called for every item in the vector.
This function will not deallocate any memory or adjust the vector's capacity.
\see cutil_vector_reset()
*/
void cutil_vector_clear(cutil_vector* vector);

/**
Resets the vector, leaving it in a fresh state.
This function will also deallocate the memory used by the vector internally and reset its size and capacity.
\see cutil_vector_clear()
*/
void cutil_vector_reset(cutil_vector* vector);

/**
Pushes an item to the back of the vector.
For the type T that this vector's trait defines, the data parameter should be of type T*.
\param data pointer to data of Type T* where T is the type described by the vector's trait.
*/
void cutil_vector_push_back(cutil_vector* vector, void* data);

/**
Pops an item from the back of the vector.
If the vector's trait includes a deletion function, it will be called on the last item in the vector.
This function has no effect on an empty vector.
*/
void cutil_vector_pop_back(cutil_vector* vector);

/**
Gets a reference to the element at a specific index.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param index the index of the item to get.
\param out pointer of type T* where T is the type described by the vector's trait.
\returns non zero value if the index was valid or zero if the index was invalid.
*/
int cutil_vector_get(cutil_vector* vector, size_t index, void* out);

/**
Sets the data at a specific index in the vector.
If the vector's trait includes a destroy function, it will be called for the existing item at the specified index.
If the vector's trait includes a copy function, it will be called on the new item being placed into the vector.
\param index the index of the item to set.
\param data pointer to data of Type T* where T is the type described by the vector's trait.
\returns non zero value if the index was valid or zero if the index was invalid.
*/
int cutil_vector_set(cutil_vector* vector, size_t index, void* data);

/**
Inserts an item at the specified index in the vector.  Existing items will be shifted if necessary.
\param index index item will be inserted at.
\param data pointer to data of Type T* where T is the type described by the vector's trait.
\return non zero value if the index was valid or zero if the index was invalid.
 */
int cutil_vector_insert(cutil_vector* vector, size_t index, void* data);

/**
Removes an item at the specified index in the vector.  Existing items will be shifted if necessary.
\param index index item will be inserted at.
\return non zero value if the index was valid or zero if the index was invalid.
 */
int cutil_vector_remove(cutil_vector* vector, size_t index);

/**
Returns pointer to the underlying buffer used by the vector.
Note that this pointer may be null if the vector's size is zero and no items have ever been inserted in the vector.
*/
void* cutil_vector_data(cutil_vector* vector);

/**
Gets the trait assocaited with this vector.  It should not be modified.
*/
cutil_trait* cutil_vector_trait(cutil_vector* vector);

/**
Gets the current capactiy of the vector.
*/
size_t cutil_vector_capacity(cutil_vector* vector);

/**
Compares vectors for equality.
Both vectos must have the same trait object to be considered for equality.
If the vector's trait defines a comarison function it will be called on each pair of elements.
If no comparison function is defined the underlying data buffers will be compared using memcmp
*/
int cutil_vector_equals(cutil_vector* a, cutil_vector* b);

#endif
