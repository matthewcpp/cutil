#pragma once

#include <stdbool.h>

#define BTREE_NODE_KEY_COUNT 4
#define BTREE_NODE_BRANCH_COUNT 5

typedef struct _btree_node {
	struct _btree_node *parent;
	int item_count;
	char keys[BTREE_NODE_KEY_COUNT];
	struct _btree_node *branches[BTREE_NODE_BRANCH_COUNT];
} _btree_node;

typedef struct cutil_btree{
	_btree_node *_root;
	unsigned int _size;
} cutil_btree;

cutil_btree *cutil_btree_create();
void cutil_btree_init(cutil_btree *btree);

void cutil_btree_uninit(cutil_btree *btree);
void cutil_btree_destroy(cutil_btree *btree);

bool cutil_btree_insert(cutil_btree *btree, char key);
bool cutil_btree_find(cutil_btree *btree, char key);
void cutil_btree_clear(cutil_btree *btree);
unsigned int cutil_btree_size(cutil_btree *btree);