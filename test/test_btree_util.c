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

bool read_btree_from_file(cutil_btree* btree, const char* test_data_name) {
	if (!btree) {
		return false;
	}

	const char* test_data_dir = cutil_test_get_data_directory();
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

void read_btree(cutil_btree* btree, const char* data) {
	int order = 0;
	int string_pos = 0;
	int item_counter = 0;
	sscanf(data, "%i%n", &order, &string_pos);

	_btree_node* new_root = read_btree_node(btree, NULL, &item_counter, data, &string_pos);
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

	//process node if it is not NULL and has items
	if (strcmp(node_type, "N") != 0 && item_count > 0) {
		_btree_node* node = _node_create(btree);
		node->parent = parent;
		node->item_count = item_count;

		// read the key values for this node from the input
		int key_value = 0;
		for (int i = 0; i < node->item_count; ++i) {
			sscanf(data + *string_pos, "%i%n", &key_value, &bytes_read);
			*string_pos += bytes_read;
			void* key_ptr = _node_get_key(node, btree->key_trait, i);
			memcpy(key_ptr, &key_value, sizeof(int));
			*item_counter += 1;
		}

		//if this node is not a leaf, then read its children recursively
		if (strcmp(node_type, "L") != 0) {
			for (int i = 0; i < node->item_count + 1; ++i) {
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

//TODO: Implement me
bool validate_btree(cutil_btree* btree) {
	if (btree) {
		return true;
	}
	else {
		return false;
	}
}

void insert_char_sequence(cutil_btree* btree, const char* sequence) {
	size_t len = strlen(sequence);

	for (size_t i = 0; i < len; i++) {
		int key = (int)sequence[i];
		cutil_btree_insert(btree, &key, NULL);
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

void indent(int depth, FILE* file) {
	for (int i = 0; i < depth; ++i) {
		fputc('\t', file);
	}
}

void dump_btree_node(cutil_btree* btree, _btree_node* node, int depth, FILE* file) {
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

	if (btree->key_trait == cutil_trait_cstring()) {
		for (int i = 0; i < node->item_count; ++i) {
			fprintf(file, "'%s' ", ((char**)node->keys)[i]);
		}
	}
	else if (btree->key_trait == cutil_trait_int()) {
		for (int i = 0; i < node->item_count; ++i) {
			fprintf(file, "%i ", ((int*)node->keys)[i]);
		}
	}

	fputc('\n', file);

	for (int i = 0; i < node->item_count + 1; ++i) {
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
