#include "btree.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


int _btree_node_get_item_position(_btree_node * node, char key);
bool _node_full(_btree_node * node);
bool _node_is_root(_btree_node * node);

void _btree_node_recursive_delete(_btree_node * node);
_btree_node *_btree_find_key(_btree_node * node, char key);
bool _btree_node_is_leaf(_btree_node * node);

_btree_node *_btree_node_create() {
	_btree_node *node = malloc(sizeof(_btree_node));

	node->parent = NULL;
	node->item_count = 0;
	memset(&node->keys, 0, sizeof(char) * BTREE_NODE_KEY_COUNT);
	memset(&node->branches, 0, sizeof(_btree_node *) * BTREE_NODE_BRANCH_COUNT);

	return node;
}

cutil_btree *cutil_btree_create() {
	cutil_btree *btree = malloc(sizeof(cutil_btree));
	cutil_btree_init(btree);
	return btree;
}

void cutil_btree_init(cutil_btree *btree) {
	btree->_root = _btree_node_create();
	btree->_size = 0;
}

void cutil_btree_uninit(cutil_btree *btree) {
	_btree_node_recursive_delete(btree->_root);
}

void cutil_btree_destroy(cutil_btree *btree) {
	cutil_btree_uninit(btree);

	free(btree);
}

void _btree_node_recursive_delete(_btree_node * node) {
	for (int i = 0; i < BTREE_NODE_BRANCH_COUNT; i++) {
		if (node->branches[i]) {
			_btree_node_recursive_delete(node->branches[i]);
		}
		else {
			break;
		}
	}

	free(node);
}

void _split_key_on_right(_btree_node *interior_node, _btree_node *split_node, _btree_node *left_node, _btree_node *right_node, char key, int insert_position) {
	int pivot_index = BTREE_NODE_KEY_COUNT / 2 + (BTREE_NODE_KEY_COUNT % 2 != 0);
	int split_node_start = pivot_index + 1;
	int split_node_key_index = insert_position - split_node_start;

	//copy items before the key position
	int item_count = insert_position - split_node_start;
	if (item_count > 0) {
		memcpy(split_node->keys, interior_node->keys + split_node_start, item_count * sizeof(char));
		memcpy(split_node->branches, interior_node->branches + split_node_start, item_count * sizeof(_btree_node *));
	}

	//copy items after the key position
	item_count = BTREE_NODE_KEY_COUNT - insert_position;
	if (item_count > 0) {
		memcpy(split_node->keys + split_node_key_index + 1, interior_node->keys + insert_position, item_count * sizeof(char));
		memcpy(split_node->branches + split_node_key_index + 1, interior_node->branches + insert_position, (item_count + 1) * sizeof(_btree_node *));
	}

	//insert the new key at the correct position
	split_node->item_count = pivot_index;
	split_node->keys[split_node_key_index] = key;

	split_node->branches[split_node_key_index] = left_node;
	left_node->parent = split_node;

	split_node->branches[split_node_key_index + 1] = right_node;
	right_node->parent = split_node;

	//clear no longer used items from interior node
	item_count = BTREE_NODE_KEY_COUNT - pivot_index;
	memset(interior_node->keys + pivot_index, 0, item_count * sizeof(char));
	memset(interior_node->branches + pivot_index + 1, 0, item_count * sizeof(_btree_node *));

	interior_node->item_count = pivot_index;
}

void _split_interior_node(cutil_btree *btree, _btree_node *interior_node, _btree_node *left_node, _btree_node *right_node, char key) {
	int insert_position = _btree_node_get_item_position(interior_node, key);
	int pivot_index = BTREE_NODE_KEY_COUNT / 2 + (BTREE_NODE_KEY_COUNT % 2 != 0);
	int pivot_key = interior_node->keys[pivot_index];

	_btree_node *split_node = _btree_node_create();

	//the newly inserted key appears in the new split node.
	if (insert_position > pivot_index) {
		_split_key_on_right(interior_node, split_node, left_node, right_node, key, insert_position);
	}

	// We have split the top most level, create a new root node
	if (_node_is_root(interior_node)) {
		_btree_node *new_root_node = _btree_node_create();
		btree->_root = new_root_node;
		new_root_node->item_count = 1;
		new_root_node->keys[0] = pivot_key;

		new_root_node->branches[0] = interior_node;
		interior_node->parent = new_root_node;

		new_root_node->branches[1] = split_node;
		split_node->parent = new_root_node;
	}
}

void _push_up_one_level(cutil_btree *btree, _btree_node *parent, _btree_node *left_node, _btree_node *right_node, char key) {
	int insertion_point = _btree_node_get_item_position(parent, key);

	if (_node_full(parent)) {
		_split_interior_node(btree, parent, left_node, right_node, key);
	}
	else { //adjust keys and branches to make room for new items
		for (int i = parent->item_count; i > insertion_point; i--) {
			parent->keys[i] = parent->keys[i - 1];
		}

		for (int i = parent->item_count + 1; i > insertion_point; i--) {
			parent->branches[i] = parent->branches[i-1];
		}

		left_node->parent = parent;
		right_node->parent = parent;

		parent->keys[insertion_point] = key;
		parent->branches[insertion_point] = left_node;
		parent->branches[insertion_point + 1] = right_node;

		parent->item_count += 1;
	}
}

void _split_leaf_node(cutil_btree *btree, _btree_node * node, char key, int insert_position) {
	//insert new item into a temp array
	char* temp_arr = malloc(BTREE_NODE_KEY_COUNT + 1 * sizeof(char));
	memcpy(temp_arr, node->keys, sizeof(char) * BTREE_NODE_KEY_COUNT);

	for (int i = BTREE_NODE_KEY_COUNT; i > insert_position; i--) {
		temp_arr[i] = temp_arr[i - 1];
	}
	temp_arr[insert_position] = key;

	//get key that will be pushed up (ceil)
	int pivot_key = BTREE_NODE_KEY_COUNT / 2 + (BTREE_NODE_KEY_COUNT % 2 != 0);

	//create new left node and copy items to it
	_btree_node * new_right_node = _btree_node_create();
	new_right_node->item_count = BTREE_NODE_KEY_COUNT - pivot_key;
	for (int i = 0; i < new_right_node->item_count; i++) {
		new_right_node->keys[i] = temp_arr[pivot_key + 1 + i];
	}

	node->item_count = pivot_key;

	// create a new root node and attach these children
	if (_node_is_root(node)) {
		_btree_node * new_root = _btree_node_create();
		new_root->keys[0] = node->keys[pivot_key];
		new_root->item_count = 1;

		new_root->branches[0] = node;
		node->parent = new_root;

		new_root->branches[1] = new_right_node;
		new_right_node->parent = new_root;

		btree->_root = new_root;
	}
	else {
		_push_up_one_level(btree, node->parent, node, new_right_node, temp_arr[pivot_key]);
	}

	free(temp_arr);
}

bool cutil_btree_insert(cutil_btree *btree, char key) {
	_btree_node * node = _btree_find_key(btree->_root, key);
	int insert_position = _btree_node_get_item_position(node, key);

	if (insert_position > -1) {
		if (_node_full(node)) {
			_split_leaf_node(btree, node, key, insert_position);
		}
		else if (insert_position >= 0) {
			// make room for new key in leaf node
			for (int i = node->item_count; i > insert_position; i--) {
				node->keys[i] = node->keys[i - 1];
			}

			node->keys[insert_position] = key;

			node->item_count += 1;
		}

		return true;
	}
	else {
		return false;
	}
}

_btree_node *_btree_find_key(_btree_node * node, char key) {
	if (_btree_node_is_leaf(node)) {
		return node;
	}
	else {
		// find the correct branch to traverse down
		for (int i = 0; i < node->item_count; i++) {
			if (key < node->keys[i]) {
				return _btree_find_key(node->branches[i], key);
			}
		}

		return _btree_find_key(node->branches[node->item_count], key);
	}

	return NULL;
}


bool cutil_btree_find(cutil_btree *btree, char key) {
	_btree_node * node = _btree_find_key(btree->_root, key);
	int insert_position = _btree_node_get_item_position(node, key);

	return insert_position == -1;
}

void cutil_btree_clear(cutil_btree *btree) {
	_btree_node_recursive_delete(btree->_root);

	btree->_root = _btree_node_create();
	btree->_size = 0;
}

unsigned int cutil_btree_size(cutil_btree *btree) {
	return btree->_size;
}

bool _btree_node_is_leaf(_btree_node * node) {
	return node->branches[0] == NULL;
}

/*	Gets the position for this item in the node key array.
	If the return value is negative it is the index of them key.
*/
int _btree_node_get_item_position(_btree_node * node, char key) {
	int insert_pos = 0;

	for (int i = 0; i < node->item_count; i++) {
		if (node->keys[i] == key) {
			insert_pos = -i;
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

bool _node_full(_btree_node * node) {
	return node->item_count == BTREE_NODE_KEY_COUNT;
}

bool _node_is_root(_btree_node * node) {
	return node->parent == NULL;
}