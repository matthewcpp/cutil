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
*/
void cutil_list_destroy(cutil_list* list);

/**
Returns the number of elements in the list
*/
unsigned int cutil_list_size(cutil_list* list);

/**
Removes all items in the list.
*/
void cutil_list_clear(cutil_list* list);

/**
Removes the item at the front of the list.
\returns non zero value if the list has a front element, otherwise zero.
*/
int cutil_list_pop_front(cutil_list* list);

/**
Removes the item at the back of the list.
\returns non zero value if the list has a back element, otherwise zero.
*/
int cutil_list_pop_back(cutil_list* list);

/**
Gets a reference to the element at the front of the list.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the list has a front element, otherwise zero.
*/
int cutil_list_front(cutil_list* list, void* out);

/**
Gets a reference to the element at the back of the list.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the list has a back element, otherwise zero.
*/
int cutil_list_back(cutil_list* list, void* out);

/**
Gets a reference to the element at the given position in the list.  This method is of linear complexity.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param index the index of the item to get.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the index was valid or zero if the index was invalid.
*/
int cutil_list_at(cutil_list* list, size_t index, void* out);

/**
Pushes data to a new element at the front of the list.
\param data pointer to data of Type T* where T is the type described by the list's trait.
*/
void cutil_list_push_front(cutil_list* list, void* data);

/**
Pushes data to a new element at the back of the list.
\param data pointer to data of Type T* where T is the type described by the list's trait.
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
Destroys a list iterator, freeing all resources used by it.
\param itr the iterator to destroy.
*/
void cutil_list_itr_destroy(cutil_list_itr* itr);

/**
Returns a non zero value if the iterator is not at the end of the list.
*/
int cutil_list_itr_has_next(cutil_list_itr* itr);

/**
Advances the iterator position and gets the data at the next element in the iterated list.  If this method is called when the iterator is passed the end of the list, the out parameter will be untouched.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param out pointer of type T* where T is the type described by the iterator's list trait.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_list_itr_next(cutil_list_itr* itr, void* out);

/**
Returns a non zero value if the iterator is not at the beginning of the list.
*/
int cutil_list_itr_has_prev(cutil_list_itr* itr);

/**
Decrements the iterator position and gets the data at the previous element in the iterated list.  If this method is called when the iterator is at the beginning of the list, the out parameter will be untouched.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param out pointer of type T* where T is the type described by the iterator's list trait.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_list_itr_prev(cutil_list_itr* itr, void* out);

/**@}*/

#endif
