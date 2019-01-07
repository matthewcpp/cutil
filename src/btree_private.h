#ifndef CUTIL_BTREE_PRIVATE_H
#define CUTIL_BTREE_PRIVATE_H

/*
This header contains private functions for use by the btree class and its associated test harness.
*/

typedef struct _btree_node {
	struct _btree_node* parent;
	int item_count;
	int position;
	void* keys;
	void* values;
	struct _btree_node** branches;
} _btree_node;

/*
Creates a new btree node.  Initializes the keys, values, and branches arrays to be the correct size based on the btree order and traits.
*/
_btree_node* _node_create(cutil_btree* btree);

/*
Destroys an individual btree node and frees all memory held
*/
void _node_destroy(_btree_node* node);

/*
Searches through existing keys in the node and returns the index the supplied key should be inserted at.
returns ITEM_ALREADY_INSERTED if they key already exists in the node.
*/
unsigned int _node_get_insertion_position(cutil_btree* btree, _btree_node*  node, void* key);

/*
Searches through existing keys in the node and returns the index of the existing key comparing equal to the supplied key.
Returns ITEM_NOT_PRESENT if no key in the node compares equal.
*/
unsigned int _node_key_position(cutil_btree* btree, _btree_node*  node, void* key);

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

/*
Copies an item from one node to another.  
The dest_node and src_node parameter may point to the same node
*/
void _node_copy_item(cutil_btree* btree, _btree_node* dest_node, size_t dest_index, _btree_node* src_node, size_t src);

struct cutil_btree {
	_btree_node* root;
	size_t size;
	int order;
	cutil_trait* key_trait;
	cutil_trait* value_trait;
};

struct cutil_btree_itr {
	_btree_node* node;
	cutil_btree* btree;
	int node_pos;
};

#endif
