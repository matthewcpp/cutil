#ifndef CUTIL_TEST_BTREE_UTIL_H
#define CUTIL_TEST_BTREE_UTIL_H

#include "cutil/btree.h"

#include <stdio.h>

typedef int (*cutil_btree_value_xform_func)(int val);


cutil_btree* read_btree_from_file(const char* test_data_name);
int insert_char_sequence(cutil_btree *btree, const char* sequence, cutil_btree_value_xform_func xform_func);
void _btree_test_insert_test_strings(cutil_btree* btree, int item_count);
int forward_itr_char_sequence(cutil_btree* btree, const char* expected_sequence, int* error_index);
void dump_btree(cutil_btree* btree, const char* path);

int validate_btree(cutil_btree* btree);

#endif
