#pragma once

#include <stdbool.h>

#define BTREE_NODE_KEY_COUNT 4
#define BTREE_NODE_BRANCH_COUNT 5

typedef struct _btree_node {
	struct _btree_node *parent;
	int item_count;
	int position;
	int keys[BTREE_NODE_KEY_COUNT];
	struct _btree_node *branches[BTREE_NODE_BRANCH_COUNT];
} _btree_node;

typedef struct cutil_btree{
	_btree_node *_root;
	unsigned int _size;
#ifdef CUTIL_DEBUGGING
	bool _debug_malloc;
	int _debug_generation;
#endif
} cutil_btree;

cutil_btree *cutil_btree_create();
void cutil_btree_init(cutil_btree *btree);

void cutil_btree_uninit(cutil_btree *btree);
void cutil_btree_destroy(cutil_btree *btree);

bool cutil_btree_insert(cutil_btree *btree, int key);
bool cutil_btree_find(cutil_btree *btree, int key);
void cutil_btree_clear(cutil_btree *btree);
unsigned int cutil_btree_size(cutil_btree *btree);


typedef struct cutil_btree_itr {
	_btree_node* _node;
	cutil_btree* _betree;
	int _node_pos;

#ifdef CUTIL_DEBUGGING
	bool _debug_malloc;
	unsigned int _debug_generation;
#endif
} cutil_btree_itr;

cutil_btree_itr *cutil_btree_itr_create(cutil_btree *btree);
void cutil_btree_itr_init(cutil_btree_itr *itr, cutil_btree *btree);

bool cutil_btree_has_next(cutil_btree_itr *itr);
bool cutil_btree_itr_next(cutil_btree_itr *itr, int* key);

void cutil_btree_itr_uninit(cutil_btree_itr *itr);
void cutil_btree_itr_destroy(cutil_btree_itr *itr);