#ifndef CUTIL_BTREE_H
#define CUTIL_BTREE_H

/** \file btree.h */

#include "trait.h"

typedef struct cutil_btree cutil_btree;
typedef struct cutil_btree_itr cutil_btree_itr;

cutil_btree* cutil_btree_create(int order, cutil_trait* key_trait, cutil_trait* value_trait);

void cutil_btree_destroy(cutil_btree* btree);

unsigned int cutil_btree_get_order(cutil_btree* btree);
cutil_trait* cutil_btree_get_key_trait(cutil_btree* btree);
cutil_trait* cutil_btree_get_value_trait(cutil_btree* btree);

int cutil_btree_equals(cutil_btree *a, cutil_btree *b);

int cutil_btree_insert(cutil_btree* btree, void* key, void* value);
int cutil_btree_get(cutil_btree* btree, void* key, void* value);
int cutil_btree_contains(cutil_btree* btree, void* key);
void cutil_btree_clear(cutil_btree* btree);
size_t cutil_btree_size(cutil_btree* btree);

int cutil_btree_erase(cutil_btree* btree, void* key);


cutil_btree_itr* cutil_btree_itr_create(cutil_btree* btree);
void cutil_btree_itr_destroy(cutil_btree_itr* itr);

int cutil_btree_itr_next(cutil_btree_itr* itr);
int cutil_btree_itr_get_key(cutil_btree_itr* itr, void* key);
int cutil_btree_itr_get_value(cutil_btree_itr* itr, void* value);

#endif
