#ifndef CUTIL_LIST_H
#define CUTIL_LIST_H

/** \file list.h */

#include "trait.h"

#include <stddef.h>


typedef struct cutil_list cutil_list;

/** @name List Functions
*/
/**@{*/

/**
Creates a new list configured to store items as described by the passed in trait.
\param trait trait object describing the items that will be stored by the list.
\returns pointer to newly created list.  If creation failed then this function will return NULL.
*/
cutil_list* cutil_list_create(cutil_trait* trait);

/**
Destroys a list, freeing all resources used by it.
If the list's trait includes a destroy function, it will be called on each element in the list.
*/
void cutil_list_destroy(cutil_list* list);

/**
Returns the number of elements in the list
*/
unsigned int cutil_list_size(cutil_list* list);

/**
Removes all items in the list.
If the list's trait includes a destroy function, it will be called on each element in the list.
*/
void cutil_list_clear(cutil_list* list);

/**
Removes the item at the front of the list.
If the list's trait includes a destroy function, it will be called on the front element.
\returns non zero value if the list has a front element, otherwise zero.
*/
int cutil_list_pop_front(cutil_list* list);

/**
Removes the item at the back of the list.
If the list's trait includes a destroy function, it will be called on the back element.
\returns non zero value if the list has a back element, otherwise zero.
*/
int cutil_list_pop_back(cutil_list* list);

/**
Gets data stored at the front of the list.
For the type T that this vector's trait defines, the out parameter should be of type T*.
If the vector's trait includes a copy function it will be called on the item being accessed.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the list has a front element, otherwise zero.
*/
int cutil_list_front(cutil_list* list, void* out);

/**
Gets data stored at the back of the list.
For the type T that this vector's trait defines, the out parameter should be of type T*.
If the vector's trait includes a copy function it will be called on the item being accessed.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the list has a back element, otherwise zero.
*/
int cutil_list_back(cutil_list* list, void* out);

/**
Gets data stored at the element of a given position in the list.  This method is of linear complexity.
For the type T that this vector's trait defines, the out parameter should be of type T*.
If the vector's trait includes a copy function it will be called on the item being accessed.
\param index the index of the item to get.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the index was valid or zero if the index was invalid.
*/
int cutil_list_at(cutil_list* list, size_t index, void* out);

/**
Pushes data to a new element at the front of the list.
For the type T that this vector's trait defines, the out parameter should be of type T*.
If the vector's trait includes a copy function it will be called on the item being inserted.
\param data pointer to data of Type T* where T is the type described by the vector's trait.
*/
void cutil_list_push_front(cutil_list* list, void* data);

/**
Pushes data to a new element at the back of the list.
For the type T that this vector's trait defines, the out parameter should be of type T*.
If the vector's trait includes a copy function it will be called on the item being inserted.
\param data pointer to data of Type T* where T is the type described by the vector's trait.
*/
void cutil_list_push_back(cutil_list* list, void* data);

/**@}*/


typedef struct cutil_list_itr cutil_list_itr;

/** @name List Iterator Functions
*/
/**@{*/

/**
Creates a new forward iterator positioned before the front of the supplied list.
\param list the list to iterate over.
*/
cutil_list_itr* cutil_list_itr_create(cutil_list* list);

/**
Destroys an iterator, freeing all resources used by it.
\param itr the iterator to destroy.
*/
void cutil_list_itr_destroy(cutil_list_itr* itr);

/**
Returns a non zero value if the iterator is not at the end of the list.
*/
int cutil_list_itr_has_next(cutil_list_itr* itr);

/**
Advances the iterator position and gets the data at the next element in the iterated list.  If this method is called when the iterator is at the end of the list, the out parameter will be untouched.
For the type T that this iterator's list defines, the data parameter should be of type T*.
If the iterator's list trait includes a copy function, it will be called on the data passed into this function.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_list_itr_next(cutil_list_itr* itr, void* out);

/**
Returns a non zero value if the iterator is not at the beginning of the list.
*/
int cutil_list_itr_has_prev(cutil_list_itr* itr);

/**
Decrements the iterator position and gets the data at the previous element in the iterated list.  If this method is called when the iterator is at the beginning of the list, the out parameter will be untouched.
For the type T that this iterator's list defines, the data parameter should be of type T*.
If the iterator's list trait includes a copy function, it will be called on the data passed into this function.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_list_itr_prev(cutil_list_itr* itr, void* data);

/**@}*/

#endif
