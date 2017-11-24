#include "test_btree_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

_btree_node*read_btree_node(_btree_node* parent, FILE* file) {
	int item_count = 0;
	fscanf(file, "%i", &item_count);

	if (item_count > 0) {
		_btree_node* node = _btree_node_create();
		node->parent = parent;
		node->item_count = item_count;

		int val = 0;
		for (int i = 0; i < node->item_count; ++i) {
			fscanf(file, "%i", &val);
			node->keys[i] = (char)val;
		}

		for (int i = 0; i < node->item_count + 1; ++i) {
			node->branches[i] = read_btree_node(node, file);
		}

		return node;
	}
	else {
		return NULL;
	}

}

void read_btree(cutil_btree* btree, const char* path) {
	FILE * file = fopen(path, "r");
	int order = 0;
	fscanf(file, "%i", &order);

	btree->_root = read_btree_node(NULL, file);

	fclose(file);
}

void insert_char_sequence(cutil_btree *btree, const char* sequence) {
	int len = strlen(sequence);

	for (int i = 0; i < len; i++) {
		cutil_btree_insert(btree, (int)sequence[i]);
	}
}

bool confirm_forward_iteration_sequence(cutil_btree* btree, const char* expected_sequence) {
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