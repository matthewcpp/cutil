#ifndef CUTIL_TEST_BTREE_FIXTURES_H
#define CUTIL_TEST_BTREE_FIXTURES_H

#include "cutil/btree.h"

#include <cutil/trait.h>

typedef struct {
    cutil_btree* btree;
} btree_test;

void btree_test_setup(btree_test* test);
void btree_test_teardown(btree_test* test);

typedef struct {
    cutil_btree* actual_btree;
    cutil_btree* expected_btree;
} btree_expect_test;

void btree_expect_test_setup(btree_expect_test* test);
void btree_expect_test_teardown(btree_expect_test* test);

typedef struct {
    cutil_btree* btree;

    int** int_keys;
    int** int_values;

    int count;
} btree_get_test;

void btree_get_test_setup(btree_get_test* test);
void btree_get_test_teardown(btree_get_test* test);

typedef struct {
    cutil_btree* btree;
    cutil_trait* key_trait_tracker;
    cutil_trait* val_trait_tracker;
} btree_trait_func_test;

void btree_trait_func_test_setup(btree_trait_func_test* test);
void btree_trait_func_test_teardown(btree_trait_func_test* test);

#endif
