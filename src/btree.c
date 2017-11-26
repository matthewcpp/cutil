#include "btree.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

int _btree_node_get_item_position(_btree_node * node, int key);
bool _node_full(_btree_node * node);

bool _node_is_root(_btree_node * node);
bool _btree_node_is_leaf(_btree_node * node);
bool _node_is_interior(_btree_node * node);

void _btree_node_recursive_delete(_btree_node * node);
_btree_node *_btree_find_key(_btree_node * node, int key);

void _set_node_child(_btree_node *parent, _btree_node *child, int index);

_btree_node *_btree_node_create() {
	_btree_node *node = malloc(sizeof(_btree_node));

	node->parent = NULL;
	node->position = 0;
	node->item_count = 0;
	memset(&node->keys, 0, sizeof(int) * BTREE_NODE_KEY_COUNT);
	memset(&node->branches, 0, sizeof(_btree_node *) * BTREE_NODE_BRANCH_COUNT);

	return node;
}

cutil_btree *cutil_btree_create() {
	cutil_btree *btree = malloc(sizeof(cutil_btree));
	cutil_btree_init(btree);

#ifdef CUTIL_DEBUGGING
	btree->_debug_malloc = true;
#endif

	return btree;
}

void cutil_btree_init(cutil_btree *btree) {
	btree->_root = _btree_node_create();
	btree->_size = 0;

#ifdef CUTIL_DEBUGGING
	btree->_debug_generation = 0;
	btree->_debug_malloc = false;
#endif
}

void cutil_btree_uninit(cutil_btree *btree) {
	_btree_node_recursive_delete(btree->_root);
}

void cutil_btree_destroy(cutil_btree *btree) {
#ifdef CUTIL_DEBUGGING
	assert(btree->_debug_malloc);
#endif
	cutil_btree_uninit(btree);

	free(btree);
}

void _btree_node_recursive_delete(_btree_node * node) {
	for (int i = 0; i < node->item_count; i++) {
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
		for (int i = 0; i < item_count; ++i) {
			split_node->keys[i] = interior_node->keys[split_node_start + i];
			_set_node_child(split_node, interior_node->branches[split_node_start + i], i);
		}
	}

	//copy items after the key position
	item_count = BTREE_NODE_KEY_COUNT - insert_position;
	if (item_count > 0) {
		for (int i = 0; i < item_count; ++i) {
			split_node->keys[split_node_key_index + 1 + i] = interior_node->keys[insert_position + i];
			
		}

		for (int i = 0; i < item_count + 1; ++i) {
			_set_node_child(split_node, interior_node->branches[insert_position + i], split_node_key_index + 1 + i);
		}
	}

	//insert the new key at the correct position
	split_node->item_count = pivot_index;
	split_node->keys[split_node_key_index] = key;

	_set_node_child(split_node, left_node, split_node_key_index);
	_set_node_child(split_node, right_node, split_node_key_index + 1);

	//clear no longer used items from interior node
	item_count = BTREE_NODE_KEY_COUNT - pivot_index;
	memset(interior_node->keys + pivot_index, 0, item_count * sizeof(int));
	memset(interior_node->branches + pivot_index + 1, 0, item_count * sizeof(_btree_node *));

	interior_node->item_count = pivot_index;
}

void _split_key_middle(_btree_node *interior_node, _btree_node *split_node, _btree_node *left_node, _btree_node *right_node, int insert_position) {
	//copy the items from the right part of the interior node into the split node
	for (int i = 0; i < insert_position; ++i) {
		split_node->keys[i] = interior_node->keys[insert_position + i];
		_set_node_child(split_node, interior_node->branches[insert_position + 1 + i], i + 1);
	}

	//set the left and right nodes
	_set_node_child(interior_node, left_node, insert_position);
	_set_node_child(split_node, right_node, 0);

	// TODO: Verify
	interior_node->item_count = insert_position;
	split_node->item_count = insert_position;

	//clear no longer used items from interior node
	memset(interior_node->keys + insert_position, 0, insert_position * sizeof(int));
	memset(interior_node->branches + insert_position + 1, 0, insert_position * sizeof(_btree_node *));

}

void _split_key_on_left(_btree_node *interior_node, _btree_node *split_node, _btree_node *left_node, _btree_node *right_node, char key, int insert_position) {
	int pivot_index = BTREE_NODE_KEY_COUNT / 2 + (BTREE_NODE_KEY_COUNT % 2 != 0);

	//copy items after the pivot position to the split node
	int item_count = BTREE_NODE_KEY_COUNT - pivot_index;
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

	for (int i = pivot_index; i > insert_position  + 1; --i) {
		_set_node_child(interior_node, interior_node->branches[i - 1], i);
	}

	//add in the new key and its children to the interior node
	interior_node->keys[insert_position] = key;
	_set_node_child(interior_node, left_node, insert_position);
	_set_node_child(interior_node, right_node, insert_position + 1);

	interior_node->item_count = pivot_index;
}

void _split_interior_node(cutil_btree *btree, _btree_node *interior_node, _btree_node *left_node, _btree_node *right_node, int key) {
	int insert_position = _btree_node_get_item_position(interior_node, key);
	int pivot_index = BTREE_NODE_KEY_COUNT / 2 + (BTREE_NODE_KEY_COUNT % 2 != 0);
	int pivot_key;

	_btree_node *split_node = _btree_node_create();

	//the newly inserted key appears in the new split node.
	if (insert_position > pivot_index) {
		pivot_key = interior_node->keys[pivot_index];
		_split_key_on_right(interior_node, split_node, left_node, right_node, key, insert_position);
	}
	else if (insert_position < pivot_index) {
		pivot_key = pivot_key = interior_node->keys[pivot_index - 1];
		_split_key_on_left(interior_node, split_node, left_node, right_node, key, insert_position);
	}
	else {
		pivot_key = key;
		_split_key_middle(interior_node, split_node, left_node, right_node, insert_position);
	}

	// We have split the top most level, create a new root node
	if (_node_is_root(interior_node)) {
		_btree_node *new_root_node = _btree_node_create();
		btree->_root = new_root_node;
		new_root_node->item_count = 1;
		new_root_node->keys[0] = pivot_key;

		_set_node_child(new_root_node, interior_node, 0);
		_set_node_child(new_root_node, split_node, 1);
	}
}

void _set_node_child(_btree_node *parent, _btree_node *child, int index) {
	parent->branches[index] = child;

	if (child) {
		child->position = index;
		child->parent = parent;
	}
}

void _push_up_one_level(cutil_btree *btree, _btree_node *parent, _btree_node *left_node, _btree_node *right_node, int key) {
	int insertion_point = _btree_node_get_item_position(parent, key);

	if (_node_full(parent)) {
		_split_interior_node(btree, parent, left_node, right_node, key);
	}
	else { //adjust keys and branches to make room for new items
		for (int i = parent->item_count; i > insertion_point; i--) {
			parent->keys[i] = parent->keys[i - 1];
		}

		for (int i = parent->item_count + 1; i > insertion_point; i--) {
			_set_node_child(parent, parent->branches[i - 1], i);
		}

		parent->keys[insertion_point] = key;
		_set_node_child(parent, left_node, insertion_point);
		_set_node_child(parent, right_node, insertion_point + 1);

		parent->item_count += 1;
	}
}

void _split_leaf_node(cutil_btree *btree, _btree_node * node, int key, int insert_position) {
	//insert new item into a temp array
	int* temp_arr = malloc((BTREE_NODE_KEY_COUNT + 1) * sizeof(int));
	memcpy(temp_arr, node->keys, sizeof(int) * BTREE_NODE_KEY_COUNT);

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

		_set_node_child(new_root, node, 0);
		_set_node_child(new_root, new_right_node, 1);

		btree->_root = new_root;
	}
	else {
		_push_up_one_level(btree, node->parent, node, new_right_node, temp_arr[pivot_key]);
	}

	free(temp_arr);
}

bool cutil_btree_insert(cutil_btree *btree, int key) {
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
#ifdef CUTIL_DEBUGGING
			btree->_debug_generation += 1;
#endif
		}

		btree->_size += 1;
		return true;
	}
	else {
		return false;
	}
}

_btree_node *_btree_find_key(_btree_node * node, int key) {
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


bool cutil_btree_find(cutil_btree *btree, int key) {
	_btree_node * node = _btree_find_key(btree->_root, key);
	int insert_position = _btree_node_get_item_position(node, key);

	return insert_position == -1;
}

void cutil_btree_clear(cutil_btree *btree) {
	_btree_node_recursive_delete(btree->_root);

	btree->_root = _btree_node_create();
	btree->_size = 0;

#ifdef CUTIL_DEBUGGING
	btree->_debug_generation += 1;
#endif
}

unsigned int cutil_btree_size(cutil_btree *btree) {
	return btree->_size;
}

bool _btree_node_is_leaf(_btree_node * node) {
	return node->branches[0] == NULL;
}

bool _node_is_interior(_btree_node * node) {
	return !_node_is_root(node) && !_btree_node_is_leaf(node);
}

/*	Gets the position for this item in the node key array.
	If the return value is negative it is the index of them key.
*/
int _btree_node_get_item_position(_btree_node * node, int key) {
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


///-------------------------
_btree_node * _itr_find_next_leaf_node(_btree_node *node);
void _itr_set_next_parent_node(cutil_btree_itr *itr);


cutil_btree_itr *cutil_btree_itr_create(cutil_btree *btree) {
	cutil_btree_itr *itr = malloc(sizeof(cutil_btree_itr));
	cutil_btree_itr_init(itr, btree);

#ifdef CUTIL_DEBUGGING
	itr->_debug_malloc = true;
#endif

	return itr;
}

void cutil_btree_itr_init(cutil_btree_itr *itr, cutil_btree *btree) {
	itr->_node = NULL;
	itr->_betree = btree;
	itr->_node_pos = -1; //this signals that the iterator is uninitialized.

#ifdef CUTIL_DEBUGGING
	
	itr->_debug_generation = btree->_debug_generation;
	itr->_debug_malloc = false;
#endif
}

void cutil_btree_itr_uninit(cutil_btree_itr *itr) {

}

void cutil_btree_itr_destroy(cutil_btree_itr *itr) {
#ifdef CUTIL_DEBUGGING
	assert(itr->_debug_malloc);
#endif
	cutil_btree_itr_uninit(itr);
	free(itr);
}

void _find_starting_node_pos(cutil_btree_itr *itr) {
	if (cutil_btree_size(itr->_betree) > 0) {
		itr->_node = _itr_find_next_leaf_node(itr->_betree->_root);
	}

	itr->_node_pos = 0;
}

_btree_node * _itr_find_next_leaf_node(_btree_node *node) {
	_btree_node *leaf = node;

	while (leaf->branches[0]) {
		leaf = leaf->branches[0];
	}

	return leaf;
}

bool cutil_btree_itr_has_next(cutil_btree_itr *itr) {
	if (itr->_node_pos == -1) {
		_find_starting_node_pos(itr);
	}

	return itr->_node != NULL;
}

void _itr_set_next_parent_node(cutil_btree_itr *itr) {
	do {
		itr->_node_pos = itr->_node->position;
		itr->_node = itr->_node->parent;
	} while ((itr->_node != NULL) && (itr->_node_pos >= itr->_node->item_count));
}

bool cutil_btree_itr_next(cutil_btree_itr *itr, int* key) {
	if (itr->_node_pos == -1) {
		_find_starting_node_pos(itr);
	}

	if (_btree_node_is_leaf(itr->_node)) {
		// all items in leaf node explored, return to parent
		if (itr->_node_pos >= itr->_node->item_count) {
			_itr_set_next_parent_node(itr);
		}
	}
	else {
		//explore the next branch in this tree
		if (itr->_node->branches[itr->_node_pos]) {
			itr->_node = _itr_find_next_leaf_node(itr->_node->branches[itr->_node_pos]);
			itr->_node_pos = 0;
		}
		else { //no branches left, head up
			_itr_set_next_parent_node(itr);
		}
	}

	if (itr->_node) {
		if (key) {
			*key = itr->_node->keys[itr->_node_pos];
		}

		itr->_node_pos += 1;

		return true;
	}
	else {
		return false;
	}
}