#pragma once

#include "btree.h"

#include <stdbool.h>

void dump_btree(cutil_btree* btree, const char* path);
void read_btree(cutil_btree* btree, const char* path);
void insert_char_sequence(cutil_btree *btree, const char* sequence);
bool confirm_forward_iteration_sequence(cutil_btree* btree, const char* expected_sequence);