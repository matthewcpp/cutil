#ifndef CUTIL_TEST_BTREE_UTIL_H
#define CUTIL_TEST_BTREE_UTIL_H

#include "btree.h"

#include <stdbool.h>

bool read_btree_from_file(cutil_btree* btree, const char* test_data_name);
void insert_char_sequence(cutil_btree *btree, const char* sequence);
bool confirm_forward_iteration_char_sequence(cutil_btree* btree, const char* expected_sequence);

#endif
