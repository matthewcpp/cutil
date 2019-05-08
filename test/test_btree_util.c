#include "test_btree_util.h"

#include "test_settings.h"
#include "btree_private.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

_btree_node* read_btree_node(cutil_btree* btree, _btree_node* parent, int* item_counter, const char* data, int* string_pos);
void read_btree(cutil_btree* btree, const char* data);

int read_btree_from_file(cutil_btree* btree, const char* test_data_name) {
	const char* test_data_dir = NULL;
	char* path_str = NULL;
	FILE* file = NULL;
	int path_size;

	if (!btree) {
		return 0;
	}

	test_data_dir = cutil_test_get_data_directory();
	path_size = cutil_snprintf_func(NULL, 0, "%s/%s.txt", test_data_dir, test_data_name);
	path_str = malloc(path_size + 1);
	sprintf(path_str, "%s/%s.txt", test_data_dir, test_data_name);

	file = fopen(path_str, "r");
	free(path_str);

	if (file) {
		long file_size = 0;
		char* file_data = NULL;

		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		file_data = malloc(file_size + 1);
		fread(file_data, 1, file_size, file);
		file_data[file_size] = 0;
		fclose(file);

		read_btree(btree, file_data);

		free(file_data);
		return 1;
	}
	else {
		return 0;
	}
}

void read_btree(cutil_btree* btree, const char* data) {
	int order = 0;
	int string_pos = 0;
	int item_counter = 0;
	_btree_node* new_root = NULL;

	sscanf(data, "%i%n", &order, &string_pos);

	new_root = read_btree_node(btree, NULL, &item_counter, data, &string_pos);
	if (new_root) {
		_node_destroy(btree->root);

		new_root->position = 0;
		btree->order = order;
		btree->root = new_root;
		btree->size = item_counter;
	}
}

_btree_node* read_btree_node(cutil_btree* btree, _btree_node* parent, int* item_counter, const char* data, int* string_pos) {
	int bytes_read = 0;
	int item_count = 0;
	char node_type[2];

	sscanf(data + *string_pos, "%2s%n", node_type, &bytes_read);
	*string_pos += bytes_read;

	sscanf(data + *string_pos, "%i%n", &item_count, &bytes_read);
	*string_pos += bytes_read;

	/* process node if it is not NULL and has items */
	if (strcmp(node_type, "N") != 0 && item_count > 0) {
		int i, key_value = 0;

		_btree_node* node = _node_create(btree);
		node->parent = parent;
		node->item_count = item_count;

		/* read the key values for this node from the input */
		for (i = 0; i < node->item_count; ++i) {
			void* key_ptr = _node_get_key(node, btree->key_trait, i);

			sscanf(data + *string_pos, "%i%n", &key_value, &bytes_read);
			*string_pos += bytes_read;

			memcpy(key_ptr, &key_value, sizeof(int));
			*item_counter += 1;
		}

		/* if this node is not a leaf, then read its children recursively */
		if (strcmp(node_type, "L") != 0) {
			for (i = 0; i < node->item_count + 1; ++i) {
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

void* get_max_key_from_subtree(cutil_btree* btree, _btree_node* node) {
    if (_node_is_leaf(node)) {
        return _node_get_key(node, btree->key_trait, node->item_count - 1);
    }
    else {
        return get_max_key_from_subtree(btree, node->branches[node->item_count]);
    }
}


void* get_min_key_from_subtree(cutil_btree* btree, _btree_node* node) {
    if (_node_is_leaf(node)) {
        return _node_get_key(node, btree->key_trait, 0);
    }
    else {
        return get_max_key_from_subtree(btree, node->branches[0]);
    }
}

/* Gets the minimum key a leaf node may have.
 * This is usually the key in the parent node corresponding to the previous position of this node
 * In the case that this is the minimal leaf node then there is no min value.
 * */
void* get_min_value_for_leaf(cutil_btree* btree, _btree_node* node) {
    if (_node_is_root(node)) {
        return NULL;
    }
    else if (node->position == 0) {
        return get_min_value_for_leaf(btree, node->parent);
    }
    else {
        return _node_get_key(node->parent, btree->key_trait, node->position - 1);
    }
}

/* Gets the max key a leaf node may have.  This is usually the key in the parent corresponding to this node's position.
 * If this is the maximal leaf node then there is no max value.
 */
void* get_max_value_for_leaf(cutil_btree* btree, _btree_node* node) {
    if (_node_is_root(node)) {
        return NULL;
    }
    else if (node->position == node->item_count) {
        return get_max_value_for_leaf(btree, node->parent);
    }
    else {
        return _node_get_key(node, btree->key_trait, node->position);
    }
}

/* Ensures that the keys of a node are within the appropriate ranges, and stored in ascending order */
int validate_btree_node_keys(cutil_btree* btree, _btree_node* node) {
    int i;

    void* min_key, *max_key;

    if (_node_is_leaf(node)) {
        min_key = get_min_value_for_leaf(btree, node);
        max_key = get_max_value_for_leaf(btree, node);
    }
    else {
        min_key = get_max_key_from_subtree(btree, node->branches[0]);
        max_key = get_min_key_from_subtree(btree, node->branches[node->item_count]);
    }


	if (min_key) {
        for (i = 0; i < node->item_count; i++) {
            void* key = _node_get_key(node, btree->key_trait, i);

            if (btree->key_trait->compare_func(key, min_key, btree->key_trait->user_data) < 1) {
                return 0;
            }
        }
	}

    if (max_key) {
        for (i = 0; i < node->item_count; i++) {
            void* key = _node_get_key(node, btree->key_trait, i);

            if (btree->key_trait->compare_func(key, max_key, btree->key_trait->user_data) > 1) {
                return 0;
            }
        }
    }

	if (!_node_is_leaf(node)) {
        for(i = 0; i < node->item_count; i++) {
            if (i) {
                void* prev_key = _node_get_key(node, btree->key_trait, i - 1);
                void* current_key = _node_get_key(node, btree->key_trait, i);

                if (btree->key_trait->compare_func(prev_key, current_key, btree->key_trait->user_data) != -1){
                    return 0;
                }
            }

            if (!validate_btree_node_keys(btree, node->branches[i])) {
                return 0;
            }
        }
	}

	return 1;
}


int validate_btree_node_structure(cutil_btree* btree, _btree_node* node) {
	/* Every node has at least m children */
    if (node->item_count >= btree->order) {
        return 0;
    }

    /* every interior node has at least ceil(m/2) child nodes */
    if (_node_is_interior(node)) {
		if (node->item_count < _btree_node_min_item_count(btree)) {
			return 0;
		}
    }

    /* The root has at least 2 children if it is not a leaf node */
    if (_node_is_root(node) && !_node_is_leaf(node)) {
    	if (node->branches[0] == NULL || node->branches[1] == NULL) {
    		return 0;
    	}
    }

    return 1;
}

/* This function finds the depth of the minimum leaf node.  All leaf nodes in the tree should have this depth. */
int determine_leaf_depth(_btree_node* node, int depth) {
	if (_node_is_leaf(node)) {
		return depth;
	}
	else {
		return determine_leaf_depth(node->branches[0], depth + 1);
	}
}

/*  All Leaf nodes in the btree should be at the same depth.  This function walks the tree and ensures that
 *  Each Leaf node's depth is equal to the target.
 */
int validate_leaf_depths(_btree_node* node, int current_depth, int target_depth) {
	if (_node_is_leaf(node)) {
		return current_depth == target_depth;
	}
	else {
		int i;
		for (i = 0; i <= node->item_count; i++) {
			if (!validate_leaf_depths(node->branches[i], current_depth + 1, target_depth)) {
			    return 0;
			}
		}

		return 1;
	}
}

int validate_btree(cutil_btree* btree) {
	if (btree) {
		int leaf_depth = determine_leaf_depth(btree->root, 0);
		int ok = validate_btree_node_keys(btree, btree->root);
		ok &= validate_btree_node_structure(btree, btree->root);
		ok &= validate_leaf_depths(btree->root, 0, leaf_depth);

		return ok;
	}
	else {
		return 0;
	}
}

int insert_char_sequence(cutil_btree* btree, const char* sequence, cutil_btree_value_xform_func xform_func) {
	size_t i, len;

	if (cutil_btree_get_key_trait(btree) != cutil_trait_int() || cutil_btree_get_value_trait(btree) != cutil_trait_int()) {
	    return 0;
	}

    len = strlen(sequence);

	for (i = 0; i < len; i++) {
		int key = (int)sequence[i];
		int value = key;

		if (xform_func) {
		    value = xform_func(value);
		}

		cutil_btree_insert(btree, &key, &value);
	}

	return 1;
}

int forward_itr_char_sequence(cutil_btree* btree, const char* expected_sequence, int* error_index) {
    size_t sequence_length = strlen(expected_sequence);
	cutil_btree_itr*  itr = cutil_btree_itr_create(btree);
	int key = 0;
	int ok = 1;
	int i = 0;

	if (sequence_length != cutil_btree_size(btree)) {
	    *error_index = -1;
	    return 0;
	}

	while (cutil_btree_itr_next(itr)) {
	    cutil_btree_itr_get_key(itr, &key);

		if ((int)expected_sequence[i] != key) {
		    *error_index = i;
			ok = 0;
			break;
		}

		i += 1;
	}

	cutil_btree_itr_destroy(itr);

	return ok;
}

void indent(int depth, FILE* file) {
	int i;
	for (i = 0; i < depth; ++i) {
		fputc('\t', file);
	}
}

void dump_btree_node(cutil_btree* btree, _btree_node* node, int depth, FILE* file) {
	char node_type;
	int i;

	indent(depth, file);

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

	if (btree->key_trait == cutil_trait_cstring()) {
		for (i = 0; i < node->item_count; ++i) {
			fprintf(file, "'%s' ", ((char**)node->keys)[i]);
		}
	}
	else if (btree->key_trait == cutil_trait_int()) {
		for (i = 0; i < node->item_count; ++i) {
			fprintf(file, "%i ", ((int*)node->keys)[i]);
		}
	}

	fputc('\n', file);

	for (i = 0; i < node->item_count + 1; ++i) {
		if (node->branches[i]) {
			dump_btree_node(btree, node->branches[i], depth + 1, file);
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
	dump_btree_node(btree, btree->root, 0, file);

	fclose(file);
}
