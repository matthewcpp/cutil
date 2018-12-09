#ifndef CUTIL_BTREE_H
#define CUTIL_BTREE_H

#include <stdbool.h>

typedef struct cutil_btree cutil_btree;
typedef struct cutil_btree_itr cutil_btree_itr;

cutil_btree* cutil_btree_create(unsigned int order);

void cutil_btree_destroy(cutil_btree* btree);

unsigned int cutil_btree_get_order(cutil_btree* btree);
bool cutil_btree_equals(cutil_btree *a, cutil_btree *b);

bool cutil_btree_insert(cutil_btree* btree, int key);
bool cutil_btree_find(cutil_btree* btree, int key);
void cutil_btree_clear(cutil_btree* btree);
unsigned int cutil_btree_size(cutil_btree* btree);

bool cutil_btree_delete(cutil_btree* btree, int key);


cutil_btree_itr* cutil_btree_itr_create(cutil_btree* btree);
void cutil_btree_itr_init(cutil_btree_itr* itr, cutil_btree* btree);

bool cutil_btree_itr_has_next(cutil_btree_itr* itr);
bool cutil_btree_itr_next(cutil_btree_itr* itr, int* key);

void cutil_btree_itr_destroy(cutil_btree_itr* itr);

#endif
