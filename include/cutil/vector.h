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
If the vector's trait includes a deletion function, it will be called for every item in the vector.
\param vector vector that will be destroyed.
*/
void cutil_vector_destroy(cutil_vector* vector);

/**
Returns the number of items currently in the vector.
*/
size_t cutil_vector_size(cutil_vector* vector);

/**
Clears the vector, resetting it to an empty state.
If the vector's trait includes a deletion function, it will be called for every item in the vector.
This function will also deallocate the memory used by the vector internally and reset its capacity.
*/
void cutil_vector_clear(cutil_vector* vector);

/**
Pushes an item to the back of the vector.
For the type T that this vector's trait defines, the data parameter should be of type T*.
If the vector's trait includes a copy function, it will be called on the data passed into this function.
\param data pointer to data of Type T* where T is the type described by the vector's trait.
*/
void cutil_vector_push(cutil_vector* vector, void* data);

/**
Pops an item from the back of the vector.
If the vector's trait includes a deletion function, it will be called on the last item in the vector.
This function has no effect on an empty vector.
*/
void cutil_vector_pop(cutil_vector* vector);

/**
Gets the data at a specific index.
For the type T that this vector's trait defines, the out parameter should be of type T*.
If the vector's trait includes a copy function it will be called on the item being accessed.
\param index the index of the item to get.
\param out pointer of tpye T* where T is the type described by the vector's trait.
\returns non zero value if the index was valid or zero if the index was invalid.
*/
int cutil_vector_get(cutil_vector* vector, unsigned int index, void* out);

/**
Gets the trait assocaited with this vector.  It should not be modified.
*/
cutil_trait* cutil_vector_trait(cutil_vector* vector);

/**
Gets the current capactiy of the vector.
*/
size_t cutil_vector_capacity(cutil_vector* vector);

#endif
