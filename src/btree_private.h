#ifndef CUTIL_BTREE_PRIVATE_H
#define CUTIL_BTREE_PRIVATE_H

#include "cutil/allocator.h"

/*
This header contains private functions for use by the btree class and its associated test harness.
*/

typedef struct _btree_node {
    struct _btree_node* parent;
    unsigned int item_count;
    unsigned int position;
    void* keys;
    void* values;
    struct _btree_node** branches;
} _btree_node;

unsigned int _btree_node_min_item_count(cutil_btree* btree);
int _node_full(cutil_btree* btree, _btree_node* node);
int _node_is_root(_btree_node* node);
int _node_is_leaf(_btree_node* node);
int _node_is_interior(_btree_node* node);

/*
Creates a new btree node.  Initializes the keys, values, and branches arrays to be the correct size based on the btree order and traits.
*/
_btree_node* _node_create(cutil_btree* btree);

/*
Destroys an individual btree node and frees all memory held
*/
void _node_destroy(cutil_btree* btree, _btree_node* node);

/*
Gets a pointer to the key in the node with the supplied index.
Precondition: index < node->item_count
*/
void* _node_get_key(_btree_node* node, cutil_trait* trait, size_t index);

/*
Gets a pointer to the value in the node with the supplied index.
Precondition: index < node->item_count
*/
void* _node_get_value(_btree_node* node, cutil_trait* trait, size_t index);

struct cutil_btree {
    _btree_node* root;
    size_t size;
    unsigned int order;
    cutil_trait* key_trait;
    cutil_trait* value_trait;
};

struct cutil_btree_itr {
    _btree_node* node;
    cutil_btree* btree;
    unsigned int node_pos;
};

#endif
