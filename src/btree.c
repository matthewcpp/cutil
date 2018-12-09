#include "btree.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>

#define ITEM_ALREADY_INSERTED INT_MAX
#define ITEM_NOT_PRESENT INT_MAX
#define ITR_POS_UNINIT INT_MAX

typedef struct _btree_node {
	struct _btree_node* parent;
	unsigned int item_count;
	unsigned int position;
	int* keys;
	struct _btree_node* *branches;
} _btree_node;

struct cutil_btree {
	_btree_node* root;
	unsigned int size;
	unsigned int order;
};

struct cutil_btree_itr {
	_btree_node* node;
	cutil_btree* btree;
	unsigned int node_pos;
};

unsigned int _btree_node_get_insertion_position(_btree_node*  node, int key);
unsigned int _btree_node_key_position(_btree_node*  node, int key);

bool _node_full(cutil_btree* btree, _btree_node*  node);
bool _node_is_root(_btree_node*  node);
bool _node_is_leaf(_btree_node*  node);
bool _node_is_interior(_btree_node*  node);
unsigned int _btree_node_min_item_count(cutil_btree* btree);

void _btree_node_recursive_delete(_btree_node*  node);
_btree_node* _btree_find_key(_btree_node*  node, int key);

_btree_node* _node_right_sibling(_btree_node*  node);
_btree_node* _node_left_sibling(_btree_node*  node);

void _set_node_child(_btree_node* parent, _btree_node* child, int index);
void _push_up_one_level(cutil_btree* btree, _btree_node* parent, _btree_node* left_node, _btree_node* right_node, int key);

void _rebalance_node(cutil_btree* btree, _btree_node* node);

unsigned int _get_pivot_index(cutil_btree* btree) {
	return (btree->order - 1) / 2 + ((btree->order - 1) % 2 != 0);
}

unsigned int cutil_btree_get_order(cutil_btree* btree) {
	return btree->order;
}

_btree_node* _btree_node_create(cutil_btree* btree) {
	_btree_node* node = malloc(sizeof(_btree_node));

	node->parent = NULL;
	node->position = 0;
	node->item_count = 0;
	node->keys = calloc(btree->order - 1, sizeof(int));
	node->branches = calloc(btree->order, sizeof(_btree_node* ));

	return node;
}

void _btree_node_destroy(_btree_node* node) {
	free(node->keys);
	free(node->branches);
	free(node);
}

cutil_btree* cutil_btree_create(unsigned int order) {
	cutil_btree* btree = malloc(sizeof(cutil_btree));
	btree->order = order;
	btree->size = 0;

	btree->root = _btree_node_create(btree);

	return btree;
}

void cutil_btree_destroy(cutil_btree* btree) {
	_btree_node_recursive_delete(btree->root);
	free(btree);
}

void _btree_node_recursive_delete(_btree_node*  node) {
	for (unsigned int i = 0; i <= node->item_count; i++) {
		if (node->branches[i]) {
			_btree_node_recursive_delete(node->branches[i]);
		}
		else {
			break;
		}
	}

	_btree_node_destroy(node);
}

void _split_node_right(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, int key, int insert_position) {
	int pivot_index = _get_pivot_index(btree);
	int split_node_start = pivot_index + 1;
	int split_node_key_index = insert_position - split_node_start;

	//copy items before the key position
	int item_count = insert_position - split_node_start;
	if (item_count > 0) {
		for (int i = 0; i < item_count; ++i) {
			split_node->keys[i] = interior_node->keys[split_node_start + i];
		}

		for (int i = 0; i < item_count; ++i) {
			_set_node_child(split_node, interior_node->branches[split_node_start + i], i);
		}
	}

	//copy items after the key position
	item_count = interior_node->item_count - insert_position;
	if (item_count > 0) {
		for (int i = 0; i < item_count; ++i) {
			split_node->keys[split_node_key_index + 1 + i] = interior_node->keys[insert_position + i];

		}

		for (int i = 0; i < item_count + 1; ++i) {
			_set_node_child(split_node, interior_node->branches[insert_position + i], split_node_key_index + 1 + i);
		}
	}

	//insert the new key at the correct position
	split_node->keys[split_node_key_index] = key;

	//clear no longer used items from the node
	item_count = interior_node->item_count - pivot_index;
	memset(interior_node->keys + pivot_index, 0, item_count * sizeof(int));
	memset(interior_node->branches + pivot_index + 1, 0, item_count * sizeof(_btree_node*));

	split_node->item_count = interior_node->item_count - pivot_index;
	interior_node->item_count = pivot_index;
	
}

void _split_interior_right(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, _btree_node* left_node, _btree_node* right_node, int key, int insert_position) {
	int pivot_index = _get_pivot_index(btree);
	int split_node_start = pivot_index + 1;
	int split_node_key_index = insert_position - split_node_start;

	_split_node_right(btree, interior_node, split_node, key, insert_position);

	_set_node_child(split_node, left_node, split_node_key_index);
	_set_node_child(split_node, right_node, split_node_key_index + 1);
}

//splitting a node in the middle has two distinct cases depending on whether it is a leaf or internal node.

void _split_node_middle(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, int insertion_position) {
	(void)btree;

	// copy the keys after the insertion point to the new now
	int insertion_pos = 0;

	for (unsigned int i = insertion_position; i < interior_node->item_count; i++) {
		split_node->keys[insertion_pos++] = interior_node->keys[i];
		split_node->item_count += 1;
	}

	//clear no longer used items from the source node
	memset(interior_node->keys + insertion_position, 0, insertion_pos*  sizeof(int));
	interior_node->item_count = insertion_position;
}

void _split_interior_middle(_btree_node* interior_node, _btree_node* split_node, _btree_node* left_node, _btree_node* right_node, int insert_position) {
	//copy the items from the right part of the interior node into the split node
	int item_count = interior_node->item_count - insert_position;
	for (int i = 0; i < item_count; ++i) {
		split_node->keys[i] = interior_node->keys[insert_position + i];
		_set_node_child(split_node, interior_node->branches[insert_position + 1 + i], i + 1);
		split_node->item_count += 1;
	}

	//set the left and right nodes
	_set_node_child(interior_node, left_node, insert_position);
	_set_node_child(split_node, right_node, 0);

	interior_node->item_count -= split_node->item_count;

	//clear no longer used items from interior node
	memset(interior_node->keys + insert_position, 0, item_count*  sizeof(int));
	memset(interior_node->branches + insert_position + 1, 0, item_count*  sizeof(_btree_node* ));

}

void _split_node_left(cutil_btree* btree, _btree_node* interior_node, _btree_node* split_node, int key, int insert_position) {
	unsigned int pivot_index = _get_pivot_index(btree);

	//copy items after the pivot position to the split node
	int item_count = interior_node->item_count - pivot_index;
	for (int i = 0; i < item_count; ++i) {
		split_node->keys[i] = interior_node->keys[pivot_index + i];
		split_node->item_count += 1;
	}

	for (int i = 1; i <= item_count; ++i) {
		_set_node_child(split_node, interior_node->branches[pivot_index + i], i);
	}

	_set_node_child(split_node, interior_node->branches[pivot_index], 0);

	//make room in the interior node for the new key
	for (int i = pivot_index - 1; i >= insert_position; --i) {
		interior_node->keys[i + 1] = interior_node->keys[i];
	}

	for (int i = pivot_index; i > insert_position + 1; --i) {
		_set_node_child(interior_node, interior_node->branches[i - 1], i);
	}

	//add in the new key and its children to the interior node
	interior_node->keys[insert_position] = key;

    //clear no longer used items from the node.  After this split only branches 0 -> pivot_index + 1 are valid.
    memset(interior_node->keys + pivot_index, 0, item_count*  sizeof(int));
    memset(interior_node->branches + pivot_index + 1, 0, (btree->order - (pivot_index + 1))*  sizeof(_btree_node* ));

	interior_node->item_count = pivot_index;
}

void _split_interior_left(cutil_btree* btree,_btree_node* interior_node, _btree_node* split_node, _btree_node* left_node, _btree_node* right_node, int key, int insert_position) {
	_split_node_left(btree, interior_node, split_node, key, insert_position);

	_set_node_child(interior_node, left_node, insert_position);
	_set_node_child(interior_node, right_node, insert_position + 1);
}

void _split_interior_node(cutil_btree* btree, _btree_node* interior_node, _btree_node* left_node, _btree_node* right_node, int key) {
	unsigned int insert_position = _btree_node_get_insertion_position(interior_node, key);
	unsigned int pivot_index = _get_pivot_index(btree);
	int pivot_key;

	_btree_node* split_node = _btree_node_create(btree);

	//the newly inserted key appears in the new split node.
	if (insert_position > pivot_index) {
		pivot_key = interior_node->keys[pivot_index];
		_split_interior_right(btree, interior_node, split_node, left_node, right_node, key, insert_position);
	}
	else if (insert_position < pivot_index) {
		pivot_key = interior_node->keys[pivot_index - 1];
		_split_interior_left(btree, interior_node, split_node, left_node, right_node, key, insert_position);
	}
	else {
		pivot_key = key;
		_split_interior_middle(interior_node, split_node, left_node, right_node, insert_position);
	}

	// We have split the top most level, create a new root node
	if (_node_is_root(interior_node)) {
		_btree_node* new_root_node = _btree_node_create(btree);
		btree->root = new_root_node;
		new_root_node->item_count = 1;
		new_root_node->keys[0] = pivot_key;

		_set_node_child(new_root_node, interior_node, 0);
		_set_node_child(new_root_node, split_node, 1);
	}
    else{
        _push_up_one_level(btree, interior_node->parent, interior_node, split_node, pivot_key);
    }
}

void _set_node_child(_btree_node* parent, _btree_node* child, int index) {
	parent->branches[index] = child;

	if (child) {
		child->position = index;
		child->parent = parent;
	}
}

void _push_up_one_level(cutil_btree* btree, _btree_node* parent, _btree_node* left_node, _btree_node* right_node, int key) {
	unsigned int insertion_point = _btree_node_get_insertion_position(parent, key);

	if (_node_full(btree, parent)) {
		_split_interior_node(btree, parent, left_node, right_node, key);
	}
	else { //adjust keys and branches to make room for new items
		for (unsigned int i = parent->item_count; i > insertion_point; i--) {
			parent->keys[i] = parent->keys[i - 1];
		}

		for (unsigned int i = parent->item_count + 1; i > insertion_point; i--) {
			_set_node_child(parent, parent->branches[i - 1], i);
		}

		parent->keys[insertion_point] = key;
		_set_node_child(parent, left_node, insertion_point);
		_set_node_child(parent, right_node, insertion_point + 1);

		parent->item_count += 1;
	}
}

void _split_leaf_node(cutil_btree* btree, _btree_node*  node, int key, unsigned int insert_position) {
	//get key that will be pushed up (ceil)
	unsigned int pivot_index = _get_pivot_index(btree);
	_btree_node*  new_right_node = _btree_node_create(btree);
	int pivot_key;

	if (insert_position > pivot_index ) {
		pivot_key = node->keys[pivot_index];
		_split_node_right(btree, node, new_right_node, key, insert_position);
	}
	else if (insert_position < pivot_index) {
		pivot_key = node->keys[pivot_index - 1];
		_split_node_left(btree, node, new_right_node, key, insert_position);
	}
	else {
		_split_node_middle(btree, node, new_right_node, insert_position);
		pivot_key = key;
	}

	// create a new root node and attach these children
	if (_node_is_root(node)) {
		_btree_node*  new_root = _btree_node_create(btree);
		new_root->keys[0] = pivot_key;
		new_root->item_count = 1;

		_set_node_child(new_root, node, 0);
		_set_node_child(new_root, new_right_node, 1);

		btree->root = new_root;
	}
	else {
		_push_up_one_level(btree, node->parent, node, new_right_node, pivot_key);
	}

}

bool cutil_btree_insert(cutil_btree* btree, int key) {
	_btree_node*  node = _btree_find_key(btree->root, key);
	unsigned int insert_position = _btree_node_get_insertion_position(node, key);

	if (insert_position == ITEM_ALREADY_INSERTED) {
		return false;
	}
	if (_node_full(btree, node)) {
		_split_leaf_node(btree, node, key, insert_position);
	}
	else {
		// make room for new key in leaf node
		for (unsigned int i = node->item_count; i > insert_position; i--) {
			node->keys[i] = node->keys[i - 1];
		}

		node->keys[insert_position] = key;

		node->item_count += 1;
	}

	btree->size += 1;
	return true;
}

_btree_node* _btree_find_key(_btree_node*  node, int key) {
	if (_node_is_leaf(node)) {
		return node;
	}
	else {
		// find the correct branch to traverse down
		for (unsigned int i = 0; i < node->item_count; i++) {
			if (key < node->keys[i]) {
				return _btree_find_key(node->branches[i], key);
			}
            else if (key == node->keys[i]){
                return node;
            }
		}

		// search key is greater than the largest item in this node
		return _btree_find_key(node->branches[node->item_count], key);
	}
}

unsigned int _btree_node_min_item_count(cutil_btree* btree) {
	return ((unsigned int)ceil((double)btree->order / 2.0)) - 1;
}

unsigned int _btree_node_key_position(_btree_node*  node, int key) {
	for (unsigned int i = 0; i < node->item_count; i++) {
		if (node->keys[i] == key) {
			return i;
		}
	}

	return ITEM_NOT_PRESENT;
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
		return false;
	}
}

// this method will merge the supplied node with its right sibling
_btree_node* _btree_merge_node_with_right_sibling(_btree_node* node) {
	_btree_node* parent = node->parent;
	_btree_node* right_sibling = _node_right_sibling(node);

	// first we will append the corresponding key from our parent into the new merged node to preserve key ordering integrity
	// any keys in our right sibling will have a greater value than this key
	node->keys[node->item_count++] = parent->keys[node->position];
	parent->item_count -= 1;

	unsigned int insert_pos = node->item_count;

	// append the keys and branches from the right sibling onto the target node
	for (unsigned int i = 0; i < right_sibling->item_count; i++) {
		node->keys[insert_pos + i] = right_sibling->keys[i];
		node->item_count += 1;
	}

	for (unsigned int i = 0; i <= right_sibling->item_count; i++) {
		_set_node_child(node, right_sibling->branches[i], insert_pos + i);
	}

	// remove the right sibling from the parent and adjust its existing branches
	for (unsigned int i = right_sibling->position; i < parent->item_count; i++) {
		parent->branches[i] = parent->branches[i] + 1;
		parent->keys[i - 1] = parent->keys[i];
	}

	_btree_node_destroy(right_sibling);

	return node;
}

/*	In the case that a node that is being rebalanced does not have enough keys but its left sibling does,
the greatest value key from the sibling will be borrowed so that a value from the nodes parent can be used to ensure the target has enough keys*/
void _btree_borrow_from_left_sibling(_btree_node* node, _btree_node* left_sibling) {
	// first slide the existing keys and branches in the target node over to make room for the incoming key from the parent
	for (unsigned int i = node->item_count; i >= 1; i--) {
		node->keys[i] = node->keys[i - 1];
		node->branches[i + 1] = node->branches[i];
	}

	// move the corresponding key from the parent to the first item of the node's key array
	node->keys[0] = node->parent->keys[node->position - 1];
	node->item_count += 1;

	// move the greatest key from the left sibling up to the parent
	node->parent->keys[left_sibling->position] = left_sibling->keys[left_sibling->item_count - 1];

	//move the associated branch from the borrowed key to our target node
	_set_node_child(node, left_sibling->branches[left_sibling->item_count], 0);
	left_sibling->branches[left_sibling->item_count] = NULL;
	left_sibling->item_count -= 1;
}

/*	In the case that a node that is being rebalanced does not have enough keys but its right sibling does, 
	the lowest value key from the sibling will be borrowed so that a value from the nodes parent can be used to ensure the target has enough keys*/
void _btree_borrow_from_right_sibling(_btree_node* node, _btree_node* right_sibling) {
	// first take our corresponding key from our parent and add it to the end of our key list and increment our item count
	node->keys[node->item_count++] = node->parent->keys[node->position];

	// grab the first node from our right sibling and move it up to the parent to replace the key we just borrowed
	node->parent->keys[node->position] = right_sibling->keys[0];
	
	// the branch corresponding to the borrowed key is moved over to our target node
	_set_node_child(node, right_sibling->branches[0], node->item_count);

	// adjust the remaining keys and branches for the right sibling
	for (unsigned int i = 1; i <= right_sibling->item_count; i++) {
		right_sibling->keys[i - 1] = right_sibling->keys[i];
		_set_node_child(right_sibling, right_sibling->branches[i], i - 1);
	}

	right_sibling->item_count -= 1;
}

// if we are rebalancing the root and it has no keys, then we need to promote its child to the new root.
// if the rebalanced node is short on keys then we will need to either borrow or steal one from our neighbor
// first we check if we can borrow from the left or right, if not, then we will merge
void _rebalance_node(cutil_btree* btree, _btree_node* node) {
	unsigned int min_item_count = _btree_node_min_item_count(btree);
	if (node->item_count >= min_item_count) {
		return;
	}

	if (node == btree->root) {
		
		if (node->item_count == 0) {
			_btree_node* old_root = btree->root;
			btree->root = node->branches[0];
			btree->root->parent = NULL;

			_btree_node_destroy(old_root);
		}
	}
	else {
		_btree_node* right_sibling = _node_right_sibling(node);
		_btree_node* left_sibling = _node_left_sibling(node);

		if (right_sibling && right_sibling->item_count > min_item_count) {
			_btree_borrow_from_right_sibling(node, right_sibling);
		}
		else if (left_sibling && left_sibling->item_count > min_item_count) {
			_btree_borrow_from_left_sibling(node, left_sibling);
		}
		else if (node->position == 0) {
			_btree_node* next_node = _btree_merge_node_with_right_sibling(node);
			_rebalance_node(btree, next_node->parent);
		}
		else {
			_btree_node* next_node = _btree_merge_node_with_right_sibling(_node_left_sibling(node));
			_rebalance_node(btree, next_node->parent);
		}
	}
}

// When deleting from a leaf node, we just need to slide the keys over and repair
void _btree_delete_from_leaf(cutil_btree* btree, _btree_node* node, unsigned int item_pos) {
	for (unsigned int i = item_pos + 1; i < node->item_count; i++) {
		node->keys[i - 1] = node->keys[i];
	}

	node->item_count -= 1;
	_rebalance_node(btree, node);
}

void _btree_delete_from_interior(cutil_btree* btree, _btree_node* node, unsigned int item_pos) {
	// first step is to find the maximum leaf node containing the value we will move up to replace this item
	_btree_node* max_leaf = node->branches[item_pos];
	while (!_node_is_leaf(max_leaf)) {
		max_leaf = max_leaf->branches[max_leaf->item_count];
	}

	// place the greatest value of maxnode in the deleted item's position
	node->keys[item_pos] = max_leaf->keys[max_leaf->item_count - 1];
	max_leaf->item_count -= 1;

	_rebalance_node(btree, max_leaf);
}

bool cutil_btree_delete(cutil_btree* btree, int key) {
	_btree_node*  node = _btree_find_key(btree->root, key);
	unsigned int item_pos = _btree_node_key_position(node, key);

	if (item_pos != ITEM_NOT_PRESENT) {

		if (_node_is_leaf(node)) {
			_btree_delete_from_leaf(btree, node, item_pos);
		}
		else {
			_btree_delete_from_interior(btree, node, item_pos);
		}

		btree->size -= 1;

		return true;
	}
	else {
		return false;
	}
}

bool cutil_btree_find(cutil_btree* btree, int key) {
	_btree_node*  node = _btree_find_key(btree->root, key);
	unsigned int insert_position = _btree_node_get_insertion_position(node, key);

	return insert_position == ITEM_ALREADY_INSERTED;
}

void cutil_btree_clear(cutil_btree* btree) {
	_btree_node_recursive_delete(btree->root);

	btree->root = _btree_node_create(btree);
	btree->size = 0;
}

unsigned int cutil_btree_size(cutil_btree* btree) {
	return btree->size;
}

bool _node_is_leaf(_btree_node*  node) {
	return node->branches[0] == NULL;
}

bool _node_is_interior(_btree_node*  node) {
	return !_node_is_root(node) && !_node_is_leaf(node);
}

// Gets the position for this item in the node key array. Returns ITEM_ALREADY_INSERTED if already present
unsigned int _btree_node_get_insertion_position(_btree_node*  node, int key) {
	unsigned int insert_pos = 0;

	for (unsigned int i = 0; i < (int)node->item_count; i++) {
		if (node->keys[i] == key) {
			insert_pos = ITEM_ALREADY_INSERTED;
			break;
		}
		else if (node->keys[i] > key) {
			insert_pos = i;
			break;
		}
		else {
			insert_pos = i + 1;
		}
	}

	return insert_pos;
}

bool _node_full(cutil_btree* btree, _btree_node*  node) {
	return node->item_count == btree->order-1;
}

bool _node_is_root(_btree_node*  node) {
	return node->parent == NULL;
}

///-------------------------
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

bool cutil_btree_itr_has_next(cutil_btree_itr* itr) {
	if (itr->node_pos == ITR_POS_UNINIT) {
		_find_starting_node_pos(itr);
	}

	return itr->node != NULL;
}

void _itr_set_next_parent_node(cutil_btree_itr* itr) {
	do {
		itr->node_pos = itr->node->position;
		itr->node = itr->node->parent;
	} while ((itr->node != NULL) && (itr->node_pos >= (int)itr->node->item_count));
}

bool cutil_btree_itr_next(cutil_btree_itr* itr, int* key) {
	if (itr->node_pos == ITR_POS_UNINIT) {
		_find_starting_node_pos(itr);
	}

	if (_node_is_leaf(itr->node)) {
		// all items in leaf node explored, return to parent
		if (itr->node_pos >= (int)itr->node->item_count) {
			_itr_set_next_parent_node(itr);
		}
	}
	else {
		//explore the next branch in this tree
		if (itr->node->branches[itr->node_pos]) {
			itr->node = _itr_find_next_leaf_node(itr->node->branches[itr->node_pos]);
			itr->node_pos = 0;
		}
		else { //no branches left, head up
			_itr_set_next_parent_node(itr);
		}
	}

	if (itr->node) {
		if (key) {
			*key = itr->node->keys[itr->node_pos];
		}

		itr->node_pos += 1;

		return true;
	}
	else {
		return false;
	}
}

bool _compare_btree_nodes(_btree_node* a, _btree_node* b) {
	if (a->item_count != b->item_count) {
		return false;
	}
	else {
		for (unsigned int i = 0; i < a->item_count; i++) {
			if (a->keys[i] != b->keys[i]) {
				return false;
			}
		}

		_btree_node* branch_a;
		_btree_node* branch_b;
		bool ok = true;

		for (unsigned int i = 0; i < a->item_count + 1; i++) {
			branch_a = a->branches[i];
			branch_b = b->branches[i];

			if (branch_a == NULL && branch_b == NULL) {
				continue;
			}
			else if (branch_a == NULL && branch_b != NULL) {
				return false;
			}
			else if (branch_a != NULL && branch_b == NULL) {
				return false;
			}
			else {
				ok = _compare_btree_nodes(branch_a, branch_b);
				if (!ok) {
					break;
				}
			}
		}

		return ok;
	}
}

bool cutil_btree_equals(cutil_btree* a, cutil_btree* b) {
	if (a->order != b->order) {
		return false;
	}
	else {
		return _compare_btree_nodes(a->root, b->root);
	}
}

void indent(int depth, FILE* file) {
	for (int i = 0; i < depth; ++i) {
		fputc('\t', file);
	}
}

void dump_btree_node(_btree_node* node, int depth, FILE* file) {
	indent(depth, file);

	char node_type;
	if (node->parent == NULL) {
		node_type = 'R';
	}
	else if (node->branches[0] == NULL) {
		node_type = 'L';
	}
	else {
		node_type = 'I';
	}

	fprintf(file, "%c ", node_type);
	fprintf(file, "%i ", node->item_count);

	for (unsigned int i = 0; i < node->item_count; ++i) {
		fprintf(file, "%i ", node->keys[i]);
	}
	fputc('\n', file);

	for (unsigned int i = 0; i < node->item_count + 1; ++i) {
		if (node->branches[i]) {
			dump_btree_node(node->branches[i], depth + 1, file);
		}
		else {
			indent(depth + 1, file);
			fprintf(file, "%i\n", 0);
		}
	}

	fputc('\n', file);
}

void dump_btree(cutil_btree* btree, const char* path) {
	FILE*  file = fopen(path, "w");

	fprintf(file, "%i\n", btree->order);
	dump_btree_node(btree->root, 0, file);

	fclose(file);
}

_btree_node* read_btree_node(cutil_btree* btree, _btree_node* parent, int* item_counter, const char* data, int* string_pos) {
	int bytes_read = 0;
	int item_count = 0;
	char node_type[2];

	sscanf(data + *string_pos, "%2s%n", node_type, &bytes_read);
	*string_pos += bytes_read;

	sscanf(data + *string_pos, "%i%n", &item_count, &bytes_read);
	*string_pos += bytes_read;

	//process node if it is not NULL and has items
	if (strcmp(node_type, "N") != 0 && item_count > 0) {
		_btree_node* node = _btree_node_create(btree);
		node->parent = parent;
		node->item_count = item_count;

		// read the key values for this node from the input
		int val = 0;
		for (unsigned int i = 0; i < node->item_count; ++i) {
			sscanf(data + *string_pos, "%i%n", &val, &bytes_read);
			*string_pos += bytes_read;
			node->keys[i] = val;
			*item_counter += 1;
		}

		//if this node is not a leaf, then read its children recursively
		if (strcmp(node_type, "L") != 0) {
			for (unsigned int i = 0; i < node->item_count + 1; ++i) {
				node->branches[i] = read_btree_node(btree, node, item_counter, data, string_pos);
				if (node->branches[i]) {
					node->branches[i]->position = i;
				}
			}
		}

		return node;
	}
	else {
		return NULL;
	}

}

void read_btree(cutil_btree* btree, const char* data) {
	int order = 0;
	int string_pos = 0;
	int item_counter = 0;
	sscanf(data, "%i%n", &order, &string_pos);

	_btree_node* new_root = read_btree_node(btree, NULL, &item_counter, data, &string_pos);
	if (new_root) {
		_btree_node_recursive_delete(btree->root);

		new_root->position = 0;
		btree->order = order;
		btree->root = new_root;
		btree->size = item_counter;
	}
}

int _get_max_branch_val(_btree_node* node) {
	while (node->parent && node->position >= node->parent->item_count) {
		node = node->parent;
	}

	if (node->parent) {
		return node->parent->keys[node->position];
	}
	else {
		return INT_MAX;
	}
}

bool _validate_btree_node(cutil_btree* btree, _btree_node* node, int parent_min_val, int parent_max_val) {
	if (node->parent) {
		unsigned int ceil_order_div_2 = (unsigned int)ceil((double)btree->order / 2.0);

		if (node->branches[0]) { //interior node
								 // All internal nodes (except the root node) have at least ceil(order / 2) nonempty children.
			if (node->branches[ceil_order_div_2 - 1] == NULL) {
				return false;
			}
		}
		else { //leaf node
			   // Each leaf node (other than the root node if it is a leaf) must contain at least ceil(m / 2) - 1 keys
			if (node->item_count < ceil_order_div_2 - 1) {
				return false;
			}
		}
	}

	for (unsigned int i = 0; i < node->item_count; i++) {
		// all keys in the node must be less than the parent max and greater than the parent min
		if (node->keys[i] <= parent_min_val || node->keys[i] >= parent_max_val) {
			return false;
		}

		int subtree_min_value = parent_min_val;

		// keys should be in ascending order
		if (i) {
			if (node->keys[i] <= node->keys[i - 1]) {
				return false;
			}

			subtree_min_value = node->keys[i - 1];
		}

		// recursive validation of subtree
		if (node->branches[i] && !_validate_btree_node(btree, node->branches[i], subtree_min_value, node->keys[i])) {
			return false;
		}
	}

	if (node->branches[node->item_count]) {
		if (!_validate_btree_node(btree, node->branches[node->item_count], node->keys[node->item_count - 1], _get_max_branch_val(node))) {
			return false;
		}
	}

	return true;
}

bool validate_btree(cutil_btree* btree) {
	return _validate_btree_node(btree, btree->root, INT_MIN, INT_MAX);
}
