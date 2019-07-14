#ifndef CUTIL_BTREE_H
#define CUTIL_BTREE_H

/** \file btree.h */

#include "trait.h"

typedef struct cutil_btree cutil_btree;
typedef struct cutil_btree_itr cutil_btree_itr;

/** @name Btree Functions
*/
/**@{*/

/**
Creates a new btree configured with the specified order and key / value traits.
\param order The order to use for this btree.  Note that this value must be >= 3.
\param key_trait trait object describing the keys of the container.  Note that this trait must define a comparison function.
\param value_trait trait object describing the keys of the container.
\returns pointer to newly created btree.  If creation failed then this function will return NULL.
*/
cutil_btree* cutil_btree_create(int order, cutil_trait* key_trait, cutil_trait* value_trait);

/**
Destroys a btree, freeing all resources used by it.
*/
void cutil_btree_destroy(cutil_btree* btree);

/**
Gets the order set at btree creation.
*/
unsigned int cutil_btree_get_order(cutil_btree* btree);

/**
Gets the key trait set at creation.
*/
cutil_trait* cutil_btree_get_key_trait(cutil_btree* btree);

/**
Gets the value trait set at creation
*/
cutil_trait* cutil_btree_get_value_trait(cutil_btree* btree);

/**
Compares a btree for equality with another btree.
The two btrees being compared must have the same key and value traits, otherwise this function will immediately return 0.
\returns non zero value if the btrees compare equal, otherwise zero.
*/
int cutil_btree_equals(cutil_btree *a, cutil_btree *b);

/**
Inserts a new element into the btree.  If there is an existing item present for the supplied key, it will be replaced with the new value.
\param key pointer of type T* where T is the type described by the btree's key trait.
\param value pointer of type T* where T is the type described by the btree's value trait.
*/
void cutil_btree_insert(cutil_btree* btree, void* key, void* value);

/**
Gets a reference to the value stored in the continer for the given key.
Note that the pointer placed in the out parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param key pointer of type T* where T is the type described by the btree's key trait.
\param out pointer of type T* where T is the type described by the btree's value trait.
\returns non zero value if a value for the supplied key was found in the container, otherwise zero.
*/
int cutil_btree_get(cutil_btree* btree, void* key, void* out);

/**
Checks if the supplied key is present in the btree
\param key pointer of type T* where T is the type described by the btree's key trait.
\returns non zero value if the btree contains the key, otherwise zero.
*/
int cutil_btree_contains(cutil_btree* btree, void* key);

/**
Removes all items in the btree.
*/
void cutil_btree_clear(cutil_btree* btree);

/**
Gets the number of items in the btree.
*/
size_t cutil_btree_size(cutil_btree* btree);

/**
Removes an item from the btree with the supplied key.
\param key pointer of type T* where T is the type described by the btree's key trait.
\returns non zero value if the supplied key was present and removed from the tree, otherwise zero
*/
int cutil_btree_erase(cutil_btree* btree, void* key);

/**@}*/

/** @name Btree Iterator Functions
*/
/**@{*/

/**
Creates a new forward iterator positioned before the first item in the supplied btree.
\param btree the btree to iteratoe over.
*/
cutil_btree_itr* cutil_btree_itr_create(cutil_btree* btree);

/**
Destroys a btree iterator, freeing all resources used by it.
*/
void cutil_btree_itr_destroy(cutil_btree_itr* itr);

/**
Advances the iterator position and gets the data at the next element in the iterated btree.  If this method is called when the iterator is pasted the end of the btree, the out parameter will be untouched.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_btree_itr_next(cutil_btree_itr* itr);

/**
Gets a reference to the iterator's current key.  If this method is called when the iterator is passed the last item of the btree, the key parameter will be untouched.
Note that the pointer placed in the key parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param key pointer of type T* where T is the type described by the iterator's btree key_trait.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_btree_itr_get_key(cutil_btree_itr* itr, void* key);

/**
Gets a reference to the iterator's current value.  If this method is called when the iterator is passed the last item of the btree, the value parameter will be untouched.
Note that the pointer placed in the value parameter is owned by the container and should be copied if it needs to be persisted beyond its lifetime.
\param key pointer of type T* where T is the type described by the iterator's btree key_trait.
\returns non zero value if data was written to the out pointer otherwise zero.
*/
int cutil_btree_itr_get_value(cutil_btree_itr* itr, void* value);

/**@}*/

#endif
