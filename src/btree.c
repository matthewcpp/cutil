#include "btree.h"
#include "btree_private.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <math.h>
#include <stdio.h>
#include <limits.h>

#define ITEM_ALREADY_INSERTED INT_MAX
#define ITEM_NOT_PRESENT INT_MAX
#define ITR_POS_UNINIT INT_MAX

void _node_clear_empty_branch_ptrs(cutil_btree* btree, _btree_node* node);

void _btree_node_recursive_delete(cutil_btree* btree, _btree_node* node);
_btree_node* _btree_find_node_for_key(cutil_btree* btree, _btree_node* node, void* key);

_btree_node* _node_right_sibling(_btree_node* node);
_btree_node* _node_left_sibling(_btree_node* node);

void _set_node_child(_btree_node* parent, _btree_node* child, int index);
void _push_up_one_level(cutil_btree* btree, _btree_node* parent, _btree_node* left_node, _btree_node* right_node, void* key, void* value);

void _rebalance_node(cutil_btree* btree, _btree_node* node);

unsigned int _get_pivot_index(cutil_btree* btree) {
	return (btree->order - 1) / 2 + ((btree->order - 1) % 2 != 0);
}

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
Copies an item from one node to another.
The dest_node and src_node parameter may point to the same node
*/
void _node_copy_item(cutil_btree* btree, _btree_node* dest_node, size_t dest_index, _btree_node* src_node, size_t src);


unsigned int cutil_btree_get_order(cutil_btree* btree) {
	return btree->order;
}

_btree_node* _node_create(cutil_btree* btree) {
	_btree_node* node = malloc(sizeof(_btree_node));

	node->parent = NULL;
	node->position = 0;
	node->item_count = 0;

	node->keys = calloc(btree->order - 1, btree->key_trait->size);
	node->values = calloc(btree->order - 1, btree->value_trait->size);

	node->branches = calloc(btree->order, sizeof(_btree_node* ));

	return node;
}

void _node_destroy(_btree_node* node) {
	free(node->values);
	free(node->keys);
	free(node->branches);
	free(node);
}

cutil_btree* cutil_btree_create(int order, cutil_trait* key_trait, cutil_trait* value_trait) {
	cutil_btree* btree = NULL;

	if (order >= 3 && key_trait != NULL && value_trait != NULL) {
		btree = malloc(sizeof(cutil_btree));

		btree->order = order;
		btree->size = 0;
		btree->key_trait = key_trait;
		btree->value_trait = value_trait;
		btree->root = _node_create(btree);
	}

	return btree;
}

void cutil_btree_destroy(cutil_btree* btree) {
	_btree_node_recursive_delete(btree, btree->root);
	free(btree);
}

void _btree_node_recursive_delete(cutil_btree* btree, _btree_node* node) {
	cutil_trait* trait = btree->key_trait;
	int i;

	if (trait->pre_destroy_func) {
		for (i = 0; i < node->item_count; i++) {
			trait->pre_destroy_func(_node_get_key(node, trait, i), trait->user_data);
		}
	}

	trait = btree->value_trait;
	if (trait->pre_destroy_func) {
		for (i = 0; i < node->item_count; i++) {
			trait->pre_destroy_func(_node_get_value(node, trait, i), trait->user_data);
		}
	}

	if (!_node_is_leaf(node)) {
		for (i = 0; i <= node->item_count; i++) {
			_btree_node_recursive_delete(btree, node->branches[i]);
		}
	}

	_node_destroy(node);
}

/*
The leaf node must be split, and the position of the new item is greater than the pivot index of the node.
This means the new item will be located in the new split node.
*/
void _split_node_right(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, void* key, void* value, int insert_position) {
	int pivot_index = _get_pivot_index(btree);
	int split_node_start = pivot_index + 1;
	int split_node_key_index = insert_position - split_node_start;
	int i;

	/* copy items from source node > pivot index and < insertion position */
	int item_count = insert_position - split_node_start;
	if (item_count > 0) {
		for (i = 0; i < item_count; ++i) {
			_node_copy_item(btree, split_node, i, interior_node, split_node_start + i);
		}

		for (i = 0; i < item_count; ++i) {
			_set_node_child(split_node, interior_node->branches[split_node_start + i], i);
		}
	}

	/* insert the new item in the new split node */
	memcpy(_node_get_key(split_node, btree->key_trait, split_node_key_index), key, btree->key_trait->size);
	memcpy(_node_get_value(split_node, btree->value_trait, split_node_key_index), value, btree->value_trait->size);

	/* copy items after the new item's position */
	item_count = (int)interior_node->item_count - insert_position;
	if (item_count > 0) {
		for (i = 0; i < item_count; ++i) {
			_node_copy_item(btree, split_node, split_node_key_index + 1 + i, interior_node, insert_position + i);
		}

		for (i = 0; i < item_count + 1; ++i) {
			_set_node_child(split_node, interior_node->branches[insert_position + i], split_node_key_index + 1 + i);
		}
	}

	split_node->item_count = interior_node->item_count - pivot_index;
	interior_node->item_count = pivot_index;
	_node_clear_empty_branch_ptrs(btree, interior_node);
}

void _split_interior_right(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, _btree_node* left_node, _btree_node* right_node, void* key, void* value, int insert_position) {
	int pivot_index = _get_pivot_index(btree);
	int split_node_start = pivot_index + 1;
	int split_node_key_index = insert_position - split_node_start;

	_split_node_right(btree, interior_node, split_node, key, value, insert_position);

	_set_node_child(split_node, left_node, split_node_key_index);
	_set_node_child(split_node, right_node, split_node_key_index + 1);
}

/*
This method splits a leaf node at the insertion position by simply copying the items after that position to the new split node.
*/
void _split_node_middle(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, int insertion_position) {
	int insertion_pos = 0;
	int i;
	(void)btree;

	for (i = insertion_position; i < interior_node->item_count; i++) {
		_node_copy_item(btree, split_node, insertion_pos++, interior_node, i);
		split_node->item_count += 1;
	}

	interior_node->item_count = insertion_position;
}

void _split_interior_middle(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, _btree_node* left_node, _btree_node* right_node, int insert_position) {
	int item_count = interior_node->item_count - insert_position;
	int i;

	/* copy the items from the right part of the interior node into the split node */
	for (i = 0; i < item_count; ++i) {
		_node_copy_item(btree, split_node, i, interior_node, insert_position + i);
		_set_node_child(split_node, interior_node->branches[insert_position + 1 + i], i + 1);
		split_node->item_count += 1;
	}

	/* set the left and right nodes */
	_set_node_child(interior_node, left_node, insert_position);
	_set_node_child(split_node, right_node, 0);

	interior_node->item_count -= split_node->item_count;

	/* clear no longer used branches from interior node */
	_node_clear_empty_branch_ptrs(btree, interior_node);
}

void _split_node_left(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, void* key, void* value, int insert_position) {
	unsigned int pivot_index = _get_pivot_index(btree);
	int i;

	/* copy items after the pivot position to the split node */
	int item_count = interior_node->item_count - pivot_index;
	for (i = 0; i < item_count; ++i) {
		_node_copy_item(btree, split_node, i, interior_node, pivot_index + i);
		split_node->item_count += 1;
	}

	for (i = 1; i <= item_count; ++i) {
		_set_node_child(split_node, interior_node->branches[pivot_index + i], i);
	}

	_set_node_child(split_node, interior_node->branches[pivot_index], 0);

	/* make room in the interior node for the new key */
	for (i = pivot_index - 1; i >= insert_position; --i) {
		_node_copy_item(btree, interior_node, i + 1, interior_node, i);
	}

	for (i = pivot_index; i > insert_position + 1; --i) {
		_set_node_child(interior_node, interior_node->branches[i - 1], i);
	}

	/* add in the new key and its children to the interior node*/
	memcpy(_node_get_key(interior_node, btree->key_trait, insert_position), key, btree->key_trait->size);
	memcpy(_node_get_value(interior_node, btree->value_trait, insert_position), value, btree->value_trait->size);

	interior_node->item_count = pivot_index;
	_node_clear_empty_branch_ptrs(btree, interior_node);
}

void _split_interior_left(cutil_btree* btree,_btree_node* interior_node, _btree_node* split_node, _btree_node* left_node, _btree_node* right_node, void* key, void* value, int insert_position) {
	_split_node_left(btree, interior_node, split_node, key, value, insert_position);

	_set_node_child(interior_node, left_node, insert_position);
	_set_node_child(interior_node, right_node, insert_position + 1);
}

void _split_interior_node(cutil_btree* btree, _btree_node* interior_node, _btree_node* left_node, _btree_node* right_node, void* key, void* value) {
	unsigned int insert_position = _node_get_insertion_position(btree, interior_node, key);
	unsigned int pivot_index = _get_pivot_index(btree);

	void* pivot_key = malloc(btree->key_trait->size);
	void* pivot_value = malloc(btree->value_trait->size);

	_btree_node* split_node = _node_create(btree);

	/* the newly inserted key appears in the new split node. */
	if (insert_position > pivot_index) {
		memcpy(pivot_key, _node_get_key(interior_node, btree->key_trait, pivot_index), btree->key_trait->size);
		memcpy(pivot_value, _node_get_value(interior_node, btree->value_trait, pivot_index), btree->value_trait->size);

		_split_interior_right(btree, interior_node, split_node, left_node, right_node, key, value, insert_position);
	}
	else if (insert_position < pivot_index) {
		memcpy(pivot_key, _node_get_key(interior_node, btree->key_trait, pivot_index - 1), btree->key_trait->size);
		memcpy(pivot_value, _node_get_value(interior_node, btree->value_trait, pivot_index - 1), btree->value_trait->size);

		_split_interior_left(btree, interior_node, split_node, left_node, right_node, key, value, insert_position);
	}
	else {
		memcpy(pivot_key, key, btree->key_trait->size);
		memcpy(pivot_value, value, btree->value_trait->size);

		_split_interior_middle(btree, interior_node, split_node, left_node, right_node, insert_position);
	}

	/* We have split the top most level, create a new root node */
	if (_node_is_root(interior_node)) {
		_btree_node* new_root_node = _node_create(btree);

		btree->root = new_root_node;
		new_root_node->item_count = 1;

		memcpy(new_root_node->keys, pivot_key, btree->key_trait->size);
		memcpy(new_root_node->values, pivot_value, btree->value_trait->size);

		_set_node_child(new_root_node, interior_node, 0);
		_set_node_child(new_root_node, split_node, 1);
	}
	else{
		_push_up_one_level(btree, interior_node->parent, interior_node, split_node, pivot_key, pivot_value);
	}

	free(pivot_key);
	free(pivot_value);
}

void _set_node_child(_btree_node* parent, _btree_node* child, int index) {
	parent->branches[index] = child;

	if (child) {
		child->position = index;
		child->parent = parent;
	}
}

void _push_up_one_level(cutil_btree* btree, _btree_node* parent, _btree_node* left_node, _btree_node* right_node, void* key, void* value) {
	unsigned int i, insertion_point = _node_get_insertion_position(btree, parent, key);

	if (_node_full(btree, parent)) {
		_split_interior_node(btree, parent, left_node, right_node, key, value);
	}
	else { /* adjust keys and branches to make room for new items */
		for (i = parent->item_count; i > insertion_point; i--) {
			_node_copy_item(btree, parent, i, parent, i - 1);
		}

		for (i = parent->item_count + 1; i > insertion_point; i--) {
			_set_node_child(parent, parent->branches[i - 1], i);
		}

		memcpy(_node_get_key(parent, btree->key_trait, insertion_point), key, btree->key_trait->size);
		memcpy(_node_get_value(parent, btree->value_trait, insertion_point), value, btree->value_trait->size);

		_set_node_child(parent, left_node, insertion_point);
		_set_node_child(parent, right_node, insertion_point + 1);

		parent->item_count += 1;
	}
}

/*
Called when the target node for insertion cannot accommodate a new new item.
The node can be split in three ways, depending on the relationship between the insertion position and pivot index of the node.
*/
void _split_leaf_node(cutil_btree* btree, _btree_node* node, void* key, void* value, unsigned int insert_position) {
	/* get key that will be pushed up (ceil) */
	unsigned int pivot_index = _get_pivot_index(btree);
	_btree_node* new_right_node = _node_create(btree);

	void* pivot_key = malloc(btree->key_trait->size);
	void* pivot_value = malloc(btree->value_trait->size);

	if (insert_position > pivot_index ) {
		memcpy(pivot_key, _node_get_key(node, btree->key_trait, pivot_index), btree->key_trait->size);
		memcpy(pivot_value, _node_get_value(node, btree->value_trait, pivot_index), btree->value_trait->size);
		_split_node_right(btree, node, new_right_node, key, value, insert_position);
	}
	else if (insert_position < pivot_index) {
		memcpy(pivot_key, _node_get_key(node, btree->key_trait, pivot_index - 1), btree->key_trait->size);
		memcpy(pivot_value, _node_get_value(node, btree->value_trait, pivot_index - 1), btree->value_trait->size);
		_split_node_left(btree, node, new_right_node, key, value, insert_position);
	}
	else {
		memcpy(pivot_key, key, btree->key_trait->size);
		memcpy(pivot_value, value, btree->value_trait->size);
		_split_node_middle(btree, node, new_right_node, insert_position);
	}

	/* create a new root node and attach these children */
	if (_node_is_root(node)) {
		_btree_node*  new_root = _node_create(btree);

		memcpy(new_root->keys, pivot_key, btree->key_trait->size);
		memcpy(new_root->values, pivot_value, btree->value_trait->size);

		new_root->item_count = 1;

		_set_node_child(new_root, node, 0);
		_set_node_child(new_root, new_right_node, 1);

		btree->root = new_root;
	}
	else {
		_push_up_one_level(btree, node->parent, node, new_right_node, pivot_key, pivot_value);
	}

	free(pivot_key);
	free(pivot_value);
}


void _node_copy_item(cutil_btree* btree, _btree_node* dest_node, size_t dest_index, _btree_node* src_node, size_t src_index) {
	void* dest_ptr = _node_get_key(dest_node, btree->key_trait, dest_index);
	void* src_ptr = _node_get_key(src_node, btree->key_trait, src_index);

	memcpy(dest_ptr, src_ptr, btree->key_trait->size);

	dest_ptr = _node_get_value(dest_node, btree->value_trait, dest_index);
	src_ptr = _node_get_value(src_node, btree->value_trait, src_index);

	memcpy(dest_ptr, src_ptr, btree->value_trait->size);
}

/*
Copies the src value to dest according to the supplied trait
Uses the copy function if present, otherwise does a simple memcpy
Assumes dest is a pointer to The trait type such as int* for int or char** for string
*/
void _copy_with_trait(void* dest, void* src, cutil_trait* trait) {
	if (trait->copy_func) {
		trait->copy_func(dest, src, trait->user_data);
	}
	else {
		memcpy(dest, src, trait->size);
	}
}


int cutil_btree_insert(cutil_btree* btree, void* key, void* value) {
	_btree_node*  node = _btree_find_node_for_key(btree, btree->root, key);
	unsigned int i, insert_position = _node_get_insertion_position(btree, node, key);

	if (insert_position == ITEM_ALREADY_INSERTED) {
		return 0;
	}
	if (_node_full(btree, node)) {
		void* copied_key = malloc(sizeof(btree->key_trait->size));
		void* copied_value = malloc(sizeof(btree->value_trait->size));

		_copy_with_trait(copied_key, key, btree->key_trait);
		_copy_with_trait(copied_value, value, btree->value_trait);

		_split_leaf_node(btree, node, copied_key, copied_value, insert_position);

		free(copied_key);
		free(copied_value);
	}
	else {
		void* new_key, *new_value;

		/* make room for new key in leaf node */
		for (i = node->item_count; i > insert_position; i--) {
			_node_copy_item(btree, node, i, node, i - 1);
		}

		new_key = _node_get_key(node, btree->key_trait, insert_position);
		new_value = _node_get_value(node, btree->value_trait, insert_position);

		_copy_with_trait(new_key, key, btree->key_trait);
		_copy_with_trait(new_value, value, btree->value_trait);

		node->item_count += 1;
	}

	btree->size += 1;
	return 1;
}

int cutil_btree_get(cutil_btree* btree, void* key, void* value) {
	_btree_node*  node = _btree_find_node_for_key(btree, btree->root, key);
	int position = _node_key_position(btree, node, key);

	if (position == ITEM_NOT_PRESENT) {
		return 0;
	}
	else {
		void* value_ptr = _node_get_value(node, btree->value_trait, position);
		memcpy(value, value_ptr, btree->value_trait->size);

		return 1;
	}
}

_btree_node* _btree_find_node_for_key(cutil_btree* btree, _btree_node* node, void* key) {
	if (_node_is_leaf(node)) {
		return node;
	}
	else {
		/* find the correct branch to traverse down */
		int i;
		for (i = 0; i < node->item_count; i++) {
			void* item_key = _node_get_key(node, btree->key_trait, i);
			int key_comp = btree->key_trait->compare_func(key, item_key, btree->key_trait->user_data);

			if (key_comp < 0) {
				return _btree_find_node_for_key(btree, node->branches[i], key);
			}
			else if (key_comp == 0){
				return node;
			}
		}

		/* search key is greater than the largest item in this node */
		return _btree_find_node_for_key(btree, node->branches[node->item_count], key);
	}
}

/* TODO: use integer ceiling */
int _btree_node_min_item_count(cutil_btree* btree) {
	return ((unsigned int)ceil((double)btree->order / 2.0)) - 1;
}

void* _node_get_key(_btree_node* node, cutil_trait* trait, size_t index) {
	return ((char*)node->keys) + (trait->size * index);
}

void* _node_get_value(_btree_node* node, cutil_trait* trait, size_t index) {
	return ((char*)node->values) + (trait->size * index);
}

_btree_node* _node_right_sibling(_btree_node*  node) {
	if (node->parent && node->position < node->parent->item_count) {
		return node->parent->branches[node->position + 1];
	}
	else {
		return NULL;
	}
}

_btree_node* _node_left_sibling(_btree_node*  node) {
	if (node->parent && node->position > 0) {
		return node->parent->branches[node->position - 1];
	}
	else {
		return NULL;
	}
}

/* this method will merge the supplied node with its right sibling */
_btree_node* _btree_merge_node_with_right_sibling(cutil_btree* btree, _btree_node* node) {
	_btree_node* parent = node->parent;
	_btree_node* right_sibling = _node_right_sibling(node);
	unsigned int insert_pos;
	int i;

	/*	first we will append the corresponding key from our parent into the new merged node to preserve key ordering integrity
		any keys in our right sibling will have a greater value than this key */
	_node_copy_item(btree, node, node->item_count++, parent, node->position);

	insert_pos = node->item_count;

	/* append the keys and branches from the right sibling onto the target node*/
	for (i = 0; i < right_sibling->item_count; i++) {
		_node_copy_item(btree, node, insert_pos + i, right_sibling, i);
		node->item_count += 1;
	}

	for (i = 0; i <= right_sibling->item_count; i++) {
		_set_node_child(node, right_sibling->branches[i], insert_pos + i);
	}

	/* remove the right sibling from the parent and adjust its existing branches */
	for (i = right_sibling->position; i < parent->item_count; i++) {
		_node_copy_item(btree, parent, i - 1, parent, i);
		_set_node_child(parent, parent->branches[i + 1], i);
	}

	/* This update the count for the item that was moved out of parent to `node` above */
	parent->item_count -= 1;

	_node_destroy(right_sibling);

	return node;
}

/*	In the case that a node that is being rebalanced does not have enough keys but its left sibling does,
the greatest value key from the sibling will be borrowed so that a value from the nodes parent can be used to ensure the target has enough keys*/
void _btree_borrow_from_left_sibling(cutil_btree* btree, _btree_node* node, _btree_node* left_sibling) {
	/* first slide the existing keys and branches in the target node over to make room for the incoming key from the parent */
	unsigned int i;
	for (i = node->item_count; i >= 1; i--) {
		_node_copy_item(btree, node, i, node, i - 1);
		node->branches[i + 1] = node->branches[i];
	}

	/* move the corresponding key from the parent to the first item of the node's key array */
	_node_copy_item(btree, node, 0, node->parent, node->position - 1);
	node->item_count += 1;

	/* move the greatest key from the left sibling up to the parent */
	_node_copy_item(btree, node->parent, left_sibling->position, left_sibling, left_sibling->item_count - 1);

	/* move the associated branch from the borrowed key to our target node */
	_set_node_child(node, left_sibling->branches[left_sibling->item_count], 0);
	left_sibling->branches[left_sibling->item_count] = NULL;
	left_sibling->item_count -= 1;
}

/*	In the case that a node that is being rebalanced does not have enough keys but its right sibling does,
	the lowest value key from the sibling will be borrowed so that a value from the nodes parent can be used to ensure the target has enough keys*/
void _btree_borrow_from_right_sibling(cutil_btree* btree, _btree_node* node, _btree_node* right_sibling) {
	int i;

	/* first take our corresponding key from our parent and add it to the end of our key list and increment our item count */
	_node_copy_item(btree, node, node->item_count++, node->parent, node->position);

	/* grab the first node from our right sibling and move it up to the parent to replace the key we just borrowed */
	_node_copy_item(btree, node->parent, node->position, right_sibling, 0);

	/* the branch corresponding to the borrowed key is moved over to our target node */
	_set_node_child(node, right_sibling->branches[0], node->item_count);

	/* adjust the remaining keys and branches for the right sibling */
	for (i = 1; i <= right_sibling->item_count; i++) {
		_node_copy_item(btree, right_sibling, i - 1, right_sibling, i);
		_set_node_child(right_sibling, right_sibling->branches[i], i - 1);
	}

	right_sibling->item_count -= 1;
}

/*
if we are rebalancing the root and it has no keys, then we need to promote its child to the new root.
if the rebalanced node is short on keys then we will need to either borrow or steal one from our neighbor
first we check if we can borrow from the left or right, if not, then we will merge */
void _rebalance_node(cutil_btree* btree, _btree_node* node) {
	int min_item_count = _btree_node_min_item_count(btree);
	if (node->item_count >= min_item_count) {
		return;
	}

	if (node == btree->root) {
		if (node->item_count == 0 && !_node_is_leaf(node)) {
			_btree_node* old_root = btree->root;
			btree->root = node->branches[0];
			btree->root->parent = NULL;

			_node_destroy(old_root);
		}
	}
	else {
		_btree_node* right_sibling = _node_right_sibling(node);
		_btree_node* left_sibling = _node_left_sibling(node);

		if (right_sibling && right_sibling->item_count > min_item_count) {
			_btree_borrow_from_right_sibling(btree, node, right_sibling);
		}
		else if (left_sibling && left_sibling->item_count > min_item_count) {
			_btree_borrow_from_left_sibling(btree, node, left_sibling);
		}
		else if (node->position == 0) {
			_btree_node* next_node = _btree_merge_node_with_right_sibling(btree, node);
			_rebalance_node(btree, next_node->parent);
		}
		else {
			_btree_node* next_node = _btree_merge_node_with_right_sibling(btree, _node_left_sibling(node));
			_rebalance_node(btree, next_node->parent);
		}
	}
}

/* When deleting from a leaf node, we just need to slide the keys over and repair */
void _btree_delete_from_leaf(cutil_btree* btree, _btree_node* node, unsigned int item_pos) {
	int i;
	for (i = item_pos + 1; i < node->item_count; i++) {
		_node_copy_item(btree, node, i - 1, node, i);
	}

	node->item_count -= 1;
	_rebalance_node(btree, node);
}

void _btree_delete_from_interior(cutil_btree* btree, _btree_node* node, unsigned int item_pos) {
	/* first step is to find the maximum leaf node containing the value we will move up to replace this item */
	_btree_node* max_leaf = node->branches[item_pos];
	while (!_node_is_leaf(max_leaf)) {
		max_leaf = max_leaf->branches[max_leaf->item_count];
	}

	/* place the greatest value of maxnode in the deleted item's position */
	_node_copy_item(btree, node, item_pos, max_leaf, max_leaf->item_count - 1);
	max_leaf->item_count -= 1;

	_rebalance_node(btree, max_leaf);
}

int cutil_btree_delete(cutil_btree* btree, void* key) {
	_btree_node*  node = _btree_find_node_for_key(btree, btree->root, key);
	unsigned int item_pos = _node_key_position(btree, node, key);

	if (btree->key_trait->pre_destroy_func) {
		void* item_key = _node_get_key(node, btree->key_trait, item_pos);
		btree->key_trait->pre_destroy_func(item_key, btree->key_trait->user_data);
	}

	if (btree->value_trait->pre_destroy_func) {
		void* item_key = _node_get_value(node, btree->value_trait, item_pos);
		btree->value_trait->pre_destroy_func(item_key, btree->value_trait->user_data);
	}

	if (item_pos != ITEM_NOT_PRESENT) {
		if (_node_is_leaf(node)) {
			_btree_delete_from_leaf(btree, node, item_pos);
		}
		else {
			_btree_delete_from_interior(btree, node, item_pos);
		}

		btree->size -= 1;

		return 1;
	}
	else {
		return 0;
	}
}

int cutil_btree_contains(cutil_btree* btree, void* key) {
	_btree_node* node = _btree_find_node_for_key(btree, btree->root, key);
	unsigned int insert_position = _node_get_insertion_position(btree, node, key);

	return insert_position == ITEM_ALREADY_INSERTED;
}

void cutil_btree_clear(cutil_btree* btree) {
	_btree_node_recursive_delete(btree, btree->root);

	btree->root = _node_create(btree);
	btree->size = 0;
}

size_t cutil_btree_size(cutil_btree* btree) {
	return btree->size;
}

int _node_is_leaf(_btree_node*  node) {
	return node->branches[0] == NULL;
}

int _node_is_interior(_btree_node*  node) {
	return !_node_is_root(node) && !_node_is_leaf(node);
}

unsigned int _node_get_insertion_position(cutil_btree* btree, _btree_node* node, void* key) {
	int i, insert_pos = 0;

	for (i = 0; i < node->item_count; i++) {
		void* item_key = _node_get_key(node, btree->key_trait, i);
		int key_comp = btree->key_trait->compare_func(key, item_key, btree->key_trait->user_data);

		if (key_comp == 0) {
			insert_pos = ITEM_ALREADY_INSERTED;
			break;
		}
		else if (key_comp < 0) {
			insert_pos = i;
			break;
		}
		else {
			insert_pos = i + 1;
		}
	}

	return insert_pos;
}

unsigned int _node_key_position(cutil_btree* btree, _btree_node* node, void* key) {
	int i;
	for (i = 0; i < node->item_count; i++) {
		void* item_key = _node_get_key(node, btree->key_trait, i);
		int key_comp = btree->key_trait->compare_func(key, item_key, btree->key_trait->user_data);

		if (key_comp == 0) {
			return i;
		}
	}

	return ITEM_NOT_PRESENT;
}

int _node_full(cutil_btree* btree, _btree_node*  node) {
	return node->item_count == btree->order-1;
}

int _node_is_root(_btree_node*  node) {
	return node->parent == NULL;
}

_btree_node*  _itr_find_next_leaf_node(_btree_node* node);
void _itr_set_next_parent_node(cutil_btree_itr* itr);


cutil_btree_itr* cutil_btree_itr_create(cutil_btree* btree) {
	cutil_btree_itr* itr = malloc(sizeof(cutil_btree_itr));
	cutil_btree_itr_init(itr, btree);

	return itr;
}

void cutil_btree_itr_init(cutil_btree_itr* itr, cutil_btree* btree) {
	itr->node = NULL;
	itr->btree = btree;
	itr->node_pos = ITR_POS_UNINIT;
}

void cutil_btree_itr_destroy(cutil_btree_itr* itr) {
	free(itr);
}

void _find_starting_node_pos(cutil_btree_itr* itr) {
	if (cutil_btree_size(itr->btree) > 0) {
		itr->node = _itr_find_next_leaf_node(itr->btree->root);
	}

	itr->node_pos = 0;
}

_btree_node*  _itr_find_next_leaf_node(_btree_node* node) {
	_btree_node* leaf = node;

	while (leaf->branches[0]) {
		leaf = leaf->branches[0];
	}

	return leaf;
}

int cutil_btree_itr_has_next(cutil_btree_itr* itr) {
	if (itr->node_pos == ITR_POS_UNINIT) {
		_find_starting_node_pos(itr);
	}

	return itr->node != NULL;
}

void _itr_set_next_parent_node(cutil_btree_itr* itr) {
	do {
		itr->node_pos = itr->node->position;
		itr->node = itr->node->parent;
	} while ((itr->node != NULL) && (itr->node_pos >= itr->node->item_count));
}

int cutil_btree_itr_next(cutil_btree_itr* itr, int* key) {
	if (itr->node_pos == ITR_POS_UNINIT) {
		_find_starting_node_pos(itr);
	}

	if (_node_is_leaf(itr->node)) {
		/* all items in leaf node explored, return to parent */
		if (itr->node_pos >= (int)itr->node->item_count) {
			_itr_set_next_parent_node(itr);
		}
	}
	else {
		/* explore the next branch in this tree */
		if (itr->node->branches[itr->node_pos]) {
			itr->node = _itr_find_next_leaf_node(itr->node->branches[itr->node_pos]);
			itr->node_pos = 0;
		}
		else { /* no branches left, head up */
			_itr_set_next_parent_node(itr);
		}
	}

	if (itr->node) {
		if (key) {
			/* TODO: evaluate me */
		}

		itr->node_pos += 1;

		return 1;
	}
	else {
		return 0;
	}
}

int _compare_btree_nodes(cutil_btree* tree_a, _btree_node* a, cutil_btree* tree_b, _btree_node* b) {
	if (a->item_count != b->item_count) {
		return 0;
	}
	else {
		_btree_node* branch_a, *branch_b;
		int i, ok = 1;
		for (i = 0; i < a->item_count; i++) {
			void* key_a = _node_get_key(a, tree_a->key_trait, i);
			void* key_b = _node_get_key(b, tree_b->key_trait, i);

			if (tree_a->key_trait->compare_func(key_a, key_b, tree_a->key_trait->user_data) != 0) {
				return 0;
			}
		}

		for (i = 0; i < a->item_count + 1; i++) {
			branch_a = a->branches[i];
			branch_b = b->branches[i];

			if (branch_a == NULL && branch_b == NULL) {
				continue;
			}
			else if (branch_a == NULL && branch_b != NULL) {
				return 0;
			}
			else if (branch_a != NULL && branch_b == NULL) {
				return 0;
			}
			else {
				ok = _compare_btree_nodes(tree_a, branch_a, tree_b, branch_b);
				if (!ok) {
					break;
				}
			}
		}

		return ok;
	}
}

int cutil_btree_equals(cutil_btree* a, cutil_btree* b) {
	if (a->order != b->order) {
		return 0;
	}

	if (a->key_trait != b->key_trait) {
		return 0;
	}

	if (a->value_trait != b->value_trait) {
		return 0;
	}

	if (a->size != b->size) {
		return 0;
	}

	return _compare_btree_nodes(a, a->root, b, b->root);
}

void _node_clear_empty_branch_ptrs(cutil_btree* btree, _btree_node* node) {
	if (_node_is_leaf(node)) {
		memset(node->branches, 0, sizeof(_btree_node*) * btree->order);
	}
	else if (!_node_full(btree, node)){
		int first_empty_branch_index = node->item_count + 1;
		int empty_branch_count = btree->order - first_empty_branch_index;
		memset(node->branches + first_empty_branch_index, 0, empty_branch_count * sizeof(_btree_node*));
	}
}
