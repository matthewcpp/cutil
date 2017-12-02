#pragma once

#include "btree.h"

#include <stdbool.h>

void dump_btree(cutil_btree* btree, const char* path);
bool read_btree_from_file(cutil_btree* btree, const char* test_data_name);
void read_btree(cutil_btree* btree, const char* data);
void insert_char_sequence(cutil_btree *btree, const char* sequence);
bool confirm_forward_iteration_char_sequence(cutil_btree* btree, const char* expected_sequence);
bool compare_btrees(cutil_btree *a, cutil_btree *b);
bool validate_btree(cutil_btree *btree);