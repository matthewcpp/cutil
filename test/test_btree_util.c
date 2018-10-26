#include "test_btree_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#define Q(x) #x
#define QUOTE(x) Q(x)

extern _btree_node* _btree_node_create();

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

	fprintf(file, "%i\n", btree->_order);
	dump_btree_node(btree->_root, 0, file);

	fclose(file);
}

bool read_btree_from_file(cutil_btree* btree, const char* test_data_name) {
	const char* test_data_dir = QUOTE(BTREE_TEST_DATA_DIR);
	int path_size = snprintf(NULL, 0, "%s/%s.txt", test_data_dir, test_data_name);
	char*  path_str = malloc(path_size + 1);
	sprintf(path_str, "%s/%s.txt", test_data_dir, test_data_name);

	FILE*  file = fopen(path_str, "r");
	free(path_str);

	if (file) {
		long file_size = 0;
		fseek(file, 0, SEEK_END);
		file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		char* file_data = malloc(file_size + 1);
		fread(file_data, 1, file_size, file);
		file_data[file_size] = 0;
		fclose(file);

		read_btree(btree, file_data);

		free(file_data);
		return true;
	}
	else {
		return false;
	}
}

_btree_node*read_btree_node(cutil_btree* btree, _btree_node* parent, int* node_counter, const char* data, int* string_pos) {
	int bytes_read = 0;
	int item_count = 0;
	char node_type[2];

	sscanf(data + *string_pos, "%2s%n", node_type, &bytes_read);
	*string_pos += bytes_read;

	sscanf(data + *string_pos, "%i%n", &item_count, &bytes_read);
	*string_pos += bytes_read;

	//process node if it is not NULL and has items
	if (strcmp(node_type, "N") != 0 && item_count > 0) {
		_btree_node* node = _btree_node_create(btree->_order);
		*node_counter += 1;
		node->parent = parent;
		node->item_count = item_count;

		// read the key values for this node from the input
		int val = 0;
		for (unsigned int i = 0; i < node->item_count; ++i) {
			sscanf(data + *string_pos, "%i%n", &val, &bytes_read);
			*string_pos += bytes_read;
			node->keys[i] = val;
		}

		//if this node is not a leaf, then read its children recursively
		if (strcmp(node_type, "L") != 0) {
			for (unsigned int i = 0; i < node->item_count + 1; ++i) {
				node->branches[i] = read_btree_node(btree, node, node_counter, data, string_pos);
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
	int node_counter = 0;
	sscanf(data, "%i%n", &order, &string_pos);

	_btree_node* new_root = read_btree_node(btree, NULL, &node_counter, data, &string_pos);
	if (new_root) {
		cutil_btree_uninit(btree);

		new_root->position = 0;
		btree->_order = order;
		btree->_root = new_root;
		btree->_size = node_counter;

#ifdef CUTIL_DEBUGGING
		btree->_debug_generation = 0;
#endif
	}
}


void insert_char_sequence(cutil_btree* btree, const char* sequence) {
	int len = strlen(sequence);

	for (int i = 0; i < len; i++) {
		cutil_btree_insert(btree, (int)sequence[i]);
	}
}

bool confirm_forward_iteration_char_sequence(cutil_btree* btree, const char* expected_sequence) {
	cutil_btree_itr*  itr = cutil_btree_itr_create(btree);
	int val = 0;
	int count = 0;
	bool ok = true;

	while (cutil_btree_itr_next(itr, &val)) {
		if (expected_sequence[count++] != val) {
			ok = false;
			break;
		}
	}

	cutil_btree_itr_destroy(itr);
	return ok;
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

bool compare_btrees(cutil_btree* a, cutil_btree* b) {
	if (a->_order != b->_order) {
		return false;
	}
	else {
		return _compare_btree_nodes(a->_root, b->_root);
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
		unsigned int ceil_order_div_2 = (unsigned int)ceil((double)btree->_order / 2.0);

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
	return _validate_btree_node(btree, btree->_root, INT_MIN, INT_MAX);
}
