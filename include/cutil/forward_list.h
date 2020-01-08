#ifndef CUTIL_FORWARD_LIST_H
#define CUTIL_FORWARD_LIST_H

/** \file forward_list.h */

#include "trait.h"

#include <stddef.h>

typedef struct cutil_forward_list cutil_forward_list;
typedef struct cutil_forward_list_itr cutil_forward_list_itr;

/** @name List Functions
*/
/**@{*/

/**
Creates a new forward list configured to store items as described by the passed in trait.
\param trait trait object describing the items that will be stored by the list.
\returns pointer to newly created list.  If creation failed then this function will return NULL.
*/
cutil_forward_list* cutil_forward_list_create(cutil_trait* trait);

/**
Destroys a list, freeing all resources used by it.
*/
void cutil_forward_list_destroy(cutil_forward_list* list);

/**
Removes all items in the list.
*/
void cutil_forward_list_clear(cutil_forward_list* list);

/**
Gets whether the list contains any items.
\returns non zero value if the list has items in it
*/
int cutil_forward_list_empty(cutil_forward_list* list);

/**
Gets a reference to the element at the front of the list.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param out pointer of type T* where T is the type described by the list's trait.
\returns non zero value if the list has a front element, otherwise zero.
*/
int cutil_forward_list_front(cutil_forward_list* list, void* out);

/**
Removes the item at the front of the list.
\returns non zero value if the list has a front element, otherwise zero.
*/
int cutil_forward_list_pop_front(cutil_forward_list* list);

/**
Pushes data to a new element at the front of the list.
\param data pointer to data of Type T* where T is the type described by the list's trait.
*/
void cutil_forward_list_push_front(cutil_forward_list* list, void* data);

/**@}*/

/** @name List Iterator Functions
*/
/**@{*/

/**
Creates a new forward iterator positioned before the front of the supplied list.
\param list the list to iterate over.
*/
cutil_forward_list_itr* cutil_forward_list_itr_create(cutil_forward_list* list);

/**
Destroys a list iterator, freeing all resources used by it.
\param itr the iterator to destroy.
*/
void cutil_forward_list_itr_destroy(cutil_forward_list_itr* itr);

/**
Returns a non zero value if the iterator is not at the end of the list.
*/
int cutil_forward_list_itr_has_next(cutil_forward_list_itr* itr);

/**
Advances the iterator position and gets the data at the next element in the iterated list.  If this method is called when the iterator is passed the end of the list, the out parameter will be untouched.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param out pointer of type T* where T is the type described by the iterator's list trait.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_forward_list_itr_next(cutil_forward_list_itr* itr, void* out);
/**@}*/

#endif
