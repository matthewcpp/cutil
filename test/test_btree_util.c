#include "test_btree_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Q(x) #x
#define QUOTE(x) Q(x)

extern _btree_node *_btree_node_create();

void indent(int depth, FILE* file) {
	for (int i = 0; i < depth; ++i) {
		fputc('\t', file);
	}
}

void dump_btree_node(_btree_node* node, int depth, FILE* file) {
	indent(depth, file);

	fprintf(file, "%i ", node->item_count);

	for (int i = 0; i < node->item_count; ++i) {
		fprintf(file, "%i ", node->keys[i]);
	}
	fputc('\n', file);

	for (int i = 0; i < node->item_count + 1; ++i) {
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
	FILE * file = fopen(path, "w");

	fprintf(file, "%i\n", BTREE_NODE_BRANCH_COUNT);
	dump_btree_node(btree->_root, 0, file);

	fclose(file);
}

bool read_btree_from_file(cutil_btree* btree, const char* test_data_name) {
	const char* test_data_dir = QUOTE(BTREE_TEST_DATA_DIR);
	int path_size = snprintf(NULL, 0, "%s/%s.txt", test_data_dir, test_data_name);
	char * path_str = malloc(path_size + 1);
	sprintf(path_str, "%s/%s.txt", test_data_dir, test_data_name);

	FILE * file = fopen(path_str, "r");
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

_btree_node*read_btree_node(_btree_node* parent, const char* data, int* string_pos) {
	int bytes_read = 0;
	int item_count = 0;

	sscanf(data + *string_pos, "%i%n", &item_count, &bytes_read);
	*string_pos += bytes_read;

	if (item_count > 0) {
		_btree_node* node = _btree_node_create();
		node->parent = parent;
		node->item_count = item_count;

		int val = 0;
		for (int i = 0; i < node->item_count; ++i) {
			sscanf(data + *string_pos, "%i%n", &val, &bytes_read);
			*string_pos += bytes_read;
			node->keys[i] = (char)val;
		}

		for (int i = 0; i < node->item_count + 1; ++i) {
			node->branches[i] = read_btree_node(node, data, string_pos);
			if (node->branches[i]) {
				node->branches[i]->position = i;
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
	sscanf(data, "%i%n", &order, &string_pos);

	btree->_root = read_btree_node(NULL, data, &string_pos);
	if (btree->_root) {
		btree->_root->position = 0;
	}
}


void insert_char_sequence(cutil_btree *btree, const char* sequence) {
	int len = strlen(sequence);

	for (int i = 0; i < len; i++) {
		cutil_btree_insert(btree, (int)sequence[i]);
	}
}

bool confirm_forward_iteration_char_sequence(cutil_btree* btree, const char* expected_sequence) {
	cutil_btree_itr * itr = cutil_btree_itr_create(btree);
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