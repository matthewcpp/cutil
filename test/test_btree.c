#include "cutil/btree.h"

#include "ctest/ctest.h"
#include "test_suites.h"

#include "test_btree_util.h"
#include "test_btree_fixtures.h"
#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_ODD_BTREE_ORDER 5
#define DEFAULT_EVEN_BTREE_ORDER 4

typedef btree_test btree_trait_test;
typedef btree_test btree_create_test;
typedef btree_test btree_size_test;
typedef btree_test btree_clear_test;
typedef btree_test btree_contains_test;
typedef btree_expect_test btree_insert_test;
typedef btree_expect_test btree_delete_test;

CTEST_FIXTURE(btree_create, btree_create_test, btree_test_setup, btree_test_teardown)
CTEST_FIXTURE(btree_size, btree_size_test, btree_test_setup, btree_test_teardown)
CTEST_FIXTURE(btree_clear, btree_clear_test, btree_test_setup, btree_test_teardown)
CTEST_FIXTURE(btree_contains, btree_contains_test, btree_test_setup, btree_test_teardown)
CTEST_FIXTURE(btree_insert, btree_insert_test, btree_expect_test_setup, btree_expect_test_teardown)
CTEST_FIXTURE(btree_delete, btree_delete_test, btree_expect_test_setup, btree_expect_test_teardown)
CTEST_FIXTURE(btree_trait, btree_trait_test, btree_test_setup, btree_test_teardown)
CTEST_FIXTURE(btree_get, btree_get_test, btree_get_test_setup, btree_get_test_teardown)
CTEST_FIXTURE(btree_trait_func, btree_trait_func_test, btree_trait_func_test_setup, btree_trait_func_test_teardown)

void invalid_key_trait_no_compare_func(btree_create_test* test) {
    cutil_trait* bogus_trait = malloc(sizeof(cutil_trait));
	memcpy(bogus_trait, cutil_trait_int(), sizeof(cutil_trait));
	bogus_trait->compare_func = NULL;

	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, bogus_trait, cutil_trait_int());
	CTEST_EXPECT_PTR_NULL(test->btree);

	free(bogus_trait);
}

void invalid_parameters(btree_create_test* test) {
    test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, NULL, cutil_trait_int());
	CTEST_ASSERT_PTR_NULL(test->btree);

    test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), NULL);
	CTEST_ASSERT_PTR_NULL(test->btree);
}

void order_sets_correct_value(btree_create_test* test) {
	test->btree = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_INT_EQ(cutil_btree_get_order(test->btree), DEFAULT_EVEN_BTREE_ORDER);
}

void non_empty(btree_size_test* test) {
    int expected_tree_size = 5;
    int i = 0;

    test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

    for (i = 0; i < expected_tree_size; ++i) {
        cutil_btree_insert(test->btree, &i, &i);
        CTEST_ASSERT_INT_EQ(cutil_btree_size(test->btree), i + 1);
    }
}

void empty(btree_size_test* test) {
	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
    CTEST_ASSERT_INT_EQ(cutil_btree_size(test->btree), 0);
}

void returns_true_if_key_in_leaf_node(btree_contains_test* test){
    int expected_to_contain_value = 34;

    test->btree = read_btree_from_file("btree4_split_interior_left_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->btree);

    CTEST_ASSERT_TRUE(cutil_btree_contains(test->btree, &expected_to_contain_value));
}

void returns_true_if_key_in_interior_node(btree_contains_test* test){
	int expected_to_contain_value = 30;

    test->btree = read_btree_from_file("btree4_split_interior_left_result");

    CTEST_ASSERT_PTR_NOT_NULL(test->btree);

    CTEST_ASSERT_TRUE(cutil_btree_contains(test->btree, &expected_to_contain_value));
}

void returns_true_if_key_in_root_node(btree_contains_test* test){
    int expected_to_contain_value = 20;

    test->btree = read_btree_from_file("btree4_split_interior_left_result");

    CTEST_ASSERT_PTR_NOT_NULL(test->btree);
	CTEST_ASSERT_TRUE(cutil_btree_contains(test->btree, &expected_to_contain_value));
}

void returns_false_if_key_not_present(btree_contains_test* test){
    int i = 0;

    test->btree = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

    for (i = 0; i < 10; ++i){
		cutil_btree_insert(test->btree, &i, &i);
	}

	for (i = 100; i < 110; ++i){
		CTEST_ASSERT_FALSE(cutil_btree_contains(test->btree, &i));
	}
}

void removes_all_items(btree_clear_test* test){
    int item_count = 10, i = 0;

    test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

    for (i = 0; i < item_count; ++i){
		cutil_btree_insert(test->btree, &i, &i);
	}

	CTEST_ASSERT_INT_EQ(cutil_btree_size(test->btree), item_count);

	cutil_btree_clear(test->btree);

	CTEST_ASSERT_INT_EQ(cutil_btree_size(test->btree), 0);
}

void replaces_existing_key(btree_insert_test* test) {
    int key = 123;
    int expected_value1 = 1000, expected_value2 = 2000;
    int actual_value = 0;

    test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

    cutil_btree_insert(test->actual_btree, &key, &expected_value1);
    CTEST_ASSERT_TRUE(cutil_btree_get(test->actual_btree, &key, &actual_value));
    CTEST_ASSERT_INT_EQ(expected_value1, actual_value);

    cutil_btree_insert(test->actual_btree, &key, &expected_value2);
    CTEST_ASSERT_TRUE(cutil_btree_get(test->actual_btree, &key, &actual_value));
    CTEST_ASSERT_INT_EQ(expected_value2, actual_value);
}

void odd_split_interior_node_push_up(btree_insert_test* test){
	int key = 123;
	test->actual_btree = read_btree_from_file("btree4_split_interior_push_up");
	CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);
	cutil_btree_insert(test->actual_btree, &key, &key);

    test->expected_btree = read_btree_from_file("btree4_split_interior_push_up_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->actual_btree, test->expected_btree));
}

/*	inserts items into the tree  with odd number order that requires a leaf node to be split to the right of the node pivot */
void odd_split_leaf_node_right(btree_insert_test* test) {
	test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
    insert_char_sequence(test->actual_btree,"ABCDE", NULL);

	test->expected_btree = read_btree_from_file("btree5_split_leaf_new_root_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	inserts items into the tree with odd number order that requires a leaf node to be split to the left of the node pivot */
void odd_split_leaf_node_left(btree_insert_test* test) {
    test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
    insert_char_sequence(test->actual_btree, "ACDEB", NULL);

    test->expected_btree = read_btree_from_file("btree5_split_leaf_new_root_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	inserts items into the tree with odd number order that requires a leaf node to be split at the node pivot */
void odd_split_leaf_node_middle(btree_insert_test* test) {
    test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
    insert_char_sequence(test->actual_btree, "ABDEC", NULL);

    test->expected_btree = read_btree_from_file("btree5_split_leaf_new_root_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	loads a tree and inserts a value that will require a split of an interior node at the right of the pivot and
	creates a new root node */
void odd_split_right_new_root(btree_insert_test* test) {
	int key = 50;

	test->actual_btree = read_btree_from_file("btree5_split_interior");
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);

	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree5_split_interior_right_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);
	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	loads a tree and inserts a value that will require a split of an interior node at the pivot and
	creates a new root node */
void odd_split_middle_new_root(btree_insert_test* test) {
	int key = 25;
    test->actual_btree = read_btree_from_file("btree5_split_interior");
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);

	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree5_split_interior_middle_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	loads a tree and inserts a value that will require a split of an interior node to the left of a pivot and
	creates a new root node */
void odd_split_left_new_root(btree_insert_test* test) {
	int key = 1;

	test->actual_btree = read_btree_from_file("btree5_split_interior");
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);

	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree5_split_interior_left_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	inserts items into the tree with even number order that requires a leaf node to be split to the left of the node pivot */
void even_split_leaf_node_left(btree_insert_test* test) {
	int key = (int)'B';

	test->actual_btree = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ADF", NULL);
	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree4_split_leaf_new_root_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	inserts items into the tree with even number order that requires a leaf node to be split at the node pivot */
void even_split_leaf_node_middle(btree_insert_test* test) {
	int key = (int)'D';

    test->actual_btree = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ABF", NULL);
	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree4_split_leaf_new_root_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	inserts items into the tree with even number order that requires a leaf node to be split to the right of the node pivot */
void even_split_leaf_node_right(btree_insert_test* test) {
	int key = (int)'F';

    test->actual_btree = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ABD", NULL);
	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree4_split_leaf_new_root_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the left of the pivot and
creates a new root node */
void even_split_left_new_root(btree_insert_test* test) {
	int key = 1;

	test->actual_btree = read_btree_from_file("btree4_split_interior");
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);
	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree4_split_interior_left_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the pivot and
creates a new root node */
void even_split_middle_new_root(btree_insert_test* test) {
	int key = 21;

    test->actual_btree = read_btree_from_file("btree4_split_interior");
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);
	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

	test->expected_btree = read_btree_from_file("btree4_split_interior_middle_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);

	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the right of the pivot and
creates a new root node */
void even_split_right_new_root(btree_insert_test* test) {
	int key = 38;

	test->actual_btree = read_btree_from_file("btree4_split_interior");
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);
	cutil_btree_insert(test->actual_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));

    test->expected_btree = read_btree_from_file("btree4_split_interior_right_result");
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);
	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

void decrements_size_when_removing_item(btree_delete_test* test) {
	size_t starting_size = 0;
	size_t actual_size = 0;
	int value = (int)'A';

    test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ABF", NULL);
	starting_size = cutil_btree_size(test->actual_btree);

	cutil_btree_erase(test->actual_btree, &value);
	actual_size = cutil_btree_size(test->actual_btree);
	CTEST_ASSERT_INT_EQ(actual_size, starting_size - 1);
}

void does_not_decrement_size_when_removing_invalid_item(btree_delete_test* test) {
	size_t starting_size = 0;
	size_t actual_size = 0;
	int value = (int)'P';

    test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ABF", NULL);
	starting_size = cutil_btree_size(test->actual_btree);

	cutil_btree_erase(test->actual_btree, &value);
	actual_size = cutil_btree_size(test->actual_btree);
	CTEST_ASSERT_INT_EQ(actual_size, starting_size);
}

void returns_true_deleting_key_that_is_present(btree_delete_test* test) {
	int key = (int)'A';

    test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ABF", NULL);

	CTEST_ASSERT_TRUE(cutil_btree_erase(test->actual_btree, &key));
}

void returns_false_deleting_unknown_key(btree_delete_test* test) {
	int key = (int)'Z';

	test->actual_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	insert_char_sequence(test->actual_btree, "ABF", NULL);

	CTEST_ASSERT_FALSE(cutil_btree_erase(test->actual_btree, &key));
}

/** utility method for running a delete test.  Loads a tree, removes a key, and validates it against the expected result */
void do_btree_delete_test(btree_delete_test* test, const char* test_tree_data, const char* expected_tree_data, int* keys_to_delete, int key_count) {
	int i;

    test->actual_btree = read_btree_from_file(test_tree_data);
    CTEST_ASSERT_PTR_NOT_NULL(test->actual_btree);

	for (i = 0; i < key_count; i++) {
		CTEST_ASSERT_TRUE(cutil_btree_erase(test->actual_btree, keys_to_delete + i));
		CTEST_ASSERT_FALSE(cutil_btree_contains(test->actual_btree, keys_to_delete + i));
		CTEST_ASSERT_TRUE(validate_btree(test->actual_btree));
	}

    test->expected_btree = read_btree_from_file(expected_tree_data);
    CTEST_ASSERT_PTR_NOT_NULL(test->expected_btree);
	CTEST_ASSERT_TRUE(cutil_btree_equals(test->expected_btree, test->actual_btree));
}

/* deletes a key from a leaf node that has extra keys and does not require any rebalancing. */
void simple_delete_leaf_node(btree_delete_test* test) {
	int keys[] = { 5 };
	do_btree_delete_test(test, "btree3_delete_leaf_simple", "btree3_delete_leaf_simple_result", keys, 1);
}

/** deletes a leaf from the first branch of the root.  Requires merging the first branch with its right sibling and removal of an invalid root */
void first_leaf_node_merge_with_parent(btree_delete_test* test) {
	int keys[] = { 10 };
	do_btree_delete_test(test, "btree3_delete_simple_merge_and_reblance_root", "btree3_delete_first_leaf_node_merge_with_parent_result", keys, 1);
}

/** deletes a leaf node from the second branch of the root.  Requires merging the from the left node (first branch) and removal of an invalid root*/
void second_leaf_node_merge_with_parent(btree_delete_test* test) {
	int keys[] = { 200 };
	do_btree_delete_test(test, "btree3_delete_simple_merge_and_reblance_root", "btree3_delete_second_leaf_node_merge_with_parent_result", keys, 1);
}

void leaf_borrow_right(btree_delete_test* test) {
	int keys[] = { 20 };
	do_btree_delete_test(test, "btree3_leaf_delete_borrow_right", "btree3_leaf_delete_borrow_right_result", keys, 1);
}

void leaf_borrow_left(btree_delete_test* test) {
	int keys[] = { 50 };
	do_btree_delete_test(test, "btree3_leaf_delete_borrow_left", "btree3_leaf_delete_borrow_left_result", keys, 1);
}

void interior1(btree_delete_test* test) {
	int keys[] = { 36 };
	do_btree_delete_test(test, "btree4_delete_interior", "btree4_delete_interior_result_1", keys, 1);
}

void interior2(btree_delete_test* test) {
	int keys[] = { 36, 28, 22 };
	do_btree_delete_test(test, "btree4_delete_interior", "btree4_delete_interior_result_2", keys, 3);
}

void pod_methods(btree_trait_test* test) {
	int i, item_count = 15;

	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

	for (i = item_count - 1; i >= 0; i--) {
		cutil_btree_insert(test->btree, &i, &i);
		CTEST_EXPECT_TRUE(cutil_btree_contains(test->btree, &i));
	}

	for (i = item_count - 1; i >= 0; i--) {
		CTEST_ASSERT_TRUE(cutil_btree_erase(test->btree, &i));
	}

	CTEST_ASSERT_INT_EQ(0, cutil_btree_size(test->btree));
}

void cstring_methods(btree_trait_test* test) {
	int i, item_count = 15;
    test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_cstring(), cutil_trait_cstring());

	for (i = item_count - 1; i >= 0; i--) {
		int contains_result;
		char* insert_str = malloc(32);
		cutil_snprintf_func(insert_str, 32, "test string %i", i);

		cutil_btree_insert(test->btree, &insert_str, &insert_str);
		contains_result = cutil_btree_contains(test->btree, &insert_str);
		free(insert_str);

		CTEST_ASSERT_TRUE(contains_result);
	}

	CTEST_ASSERT_INT_EQ(item_count, cutil_btree_size(test->btree));

	for (i = item_count - 1; i >= 0; i--) {
		int delete_result;
		char* insert_str = malloc(32);
		cutil_snprintf_func(insert_str, 32, "test string %i", i);

		delete_result = cutil_btree_erase(test->btree, &insert_str);
		free(insert_str);

		CTEST_ASSERT_TRUE(delete_result);
	}

	CTEST_ASSERT_INT_EQ(0, cutil_btree_size(test->btree));
}

void ptr_methods(btree_trait_test* test) {
	int i, item_count = 15;
	int** test_ptrs = calloc(item_count, sizeof(int*));

	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_ptr(), cutil_trait_ptr());

	for (i = item_count -1; i >= 0; i--) {
		void* ptr_val = NULL;
		int* test_ptr = malloc(sizeof(int));
		*test_ptr = i;
		test_ptrs[i] = test_ptr;

		cutil_btree_insert(test->btree, &test_ptr, &ptr_val);
		CTEST_EXPECT_TRUE(cutil_btree_contains(test->btree, &test_ptr));
	}

	for (i = item_count - 1; i >= 0; i--) {
		int* test_ptr = test_ptrs[i];
		CTEST_EXPECT_TRUE(cutil_btree_erase(test->btree, &test_ptr));
	}

	/* test cleanup */
	for (i = item_count - 1; i >= 0; i--) {
		free(test_ptrs[i]);
	}

	CTEST_ASSERT_INT_EQ(0, cutil_btree_size(test->btree));

	free(test_ptrs);
}

void get_pod(btree_get_test* test) {
	int i, item_count = 10;

	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

	for (i = 1; i <= item_count; i++) {
		int expected_value = i * 10;
		cutil_btree_insert(test->btree, &i, &expected_value);
	}

	for (i = 1; i <= item_count; i++) {
		int expected_value = i * 10;
		int actual_value = 0;
		int result = cutil_btree_get(test->btree, &i, &actual_value);
		CTEST_ASSERT_TRUE(result);
		CTEST_ASSERT_INT_EQ(expected_value, actual_value);
	}
}


void get_ptr(btree_get_test* test) {
	int* expected_ptr = NULL;
	int i;

	test->count = 10;

	test->int_keys = calloc(test->count, sizeof(int*));
	test->int_values = calloc(test->count, sizeof(int*));

	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_ptr(), cutil_trait_ptr());

	for (i = 0; i < test->count; i++) {
		test->int_keys[i] = malloc(sizeof(int));
		test->int_values[i] = malloc(sizeof(int));

		cutil_btree_insert(test->btree, &test->int_keys[i], &test->int_values[i]);
	}

	for (i = 0; i < test->count; i++) {
		cutil_btree_get(test->btree, &test->int_keys[i], &expected_ptr);

		CTEST_EXPECT_PTR_EQ(test->int_values[i], expected_ptr);
	}
}

void get_cstring(btree_get_test* test) {
	int i, item_count = 10;

	char key_str[32];
	char value_str[32];
	char actual_str[32];

	char* key_ptr = &key_str[0];
	char* value_ptr = &value_str[0];
	char* actual_ptr = &actual_str[0];

	test->btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_cstring(), cutil_trait_cstring());

	for (i = 0; i < item_count; i++) {
		cutil_snprintf_func(key_str, 32, "key %i", i);
		cutil_snprintf_func(value_str, 32, "value %i", i);

		cutil_btree_insert(test->btree, &key_ptr, &value_ptr);
	}

	for (i = 0; i < item_count; i++) {
		cutil_snprintf_func(key_str, 32, "key %i", i);
		cutil_snprintf_func(value_str, 32, "value %i", i);

		cutil_btree_get(test->btree, &key_ptr, &actual_ptr);

		CTEST_ASSERT_INT_EQ(strcmp(value_ptr, actual_ptr), 0);
	}
}
void btree_equals_with_different_traits() {
	cutil_btree* btree1 = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	cutil_btree* btree2 = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_cstring(), cutil_trait_int());
    cutil_btree* btree3 = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_cstring());

    /* key trait */
	CTEST_EXPECT_FALSE(cutil_btree_equals(btree1, btree2));

	/* value trait */
	CTEST_EXPECT_FALSE(cutil_btree_equals(btree1, btree3));

    cutil_btree_destroy(btree1);
    cutil_btree_destroy(btree2);
    cutil_btree_destroy(btree3);
}

void btree_equals_empty() {
	cutil_btree* btree1 = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	cutil_btree* btree2 = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

	/* empty trees should be equal */
	CTEST_EXPECT_TRUE(cutil_btree_equals(btree1, btree2));

	cutil_btree_destroy(btree1);
	cutil_btree_destroy(btree2);
}

void key_copy_on_insert(btree_trait_func_test* test) {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(test->btree, &key, &value);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(test->key_trait_tracker), 1);
}

void key_destroy_on_erase(btree_trait_func_test* test) {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(test->btree, &key, &value);
	CTEST_ASSERT_TRUE(cutil_btree_erase(test->btree, &key));

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->key_trait_tracker), 1);
}

void key_compare_on_insert(btree_trait_func_test* test) {
	char* key1 = "test key1";
	char* key2 = "test key2";
	char* value = "test value";

	cutil_btree_insert(test->btree, &key1, &value);
	cutil_btree_insert(test->btree, &key2, &value);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_compare_count(test->key_trait_tracker), 1);
}

void key_destroy_on_clear(btree_trait_func_test* test) {
	int item_count = 10;
	_btree_test_insert_test_strings(test->btree, item_count);

	cutil_btree_clear(test->btree);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->key_trait_tracker), item_count);
}

void key_destroy_on_destroy(btree_trait_func_test* test) {
	int item_count = 10;
	_btree_test_insert_test_strings(test->btree, item_count);

	cutil_btree_destroy(test->btree);
	test->btree = NULL;

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->key_trait_tracker), item_count);
}

void val_copy_on_insert(btree_trait_func_test* test) {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(test->btree, &key, &value);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(test->val_trait_tracker), 1);
}

void val_destroy_on_erase(btree_trait_func_test* test) {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(test->btree, &key, &value);
	CTEST_ASSERT_TRUE(cutil_btree_erase(test->btree, &key));

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->val_trait_tracker), 1);
}

void val_destroy_on_clear(btree_trait_func_test* test) {
	int item_count = 10;
	_btree_test_insert_test_strings(test->btree, item_count);

	cutil_btree_clear(test->btree);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->val_trait_tracker), item_count);
}

void val_destroy_on_destroy(btree_trait_func_test* test) {
	int item_count = 10;
	_btree_test_insert_test_strings(test->btree, item_count);

	cutil_btree_destroy(test->btree);
	test->btree = NULL;

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->val_trait_tracker), item_count);
}

void val_insert_replace(btree_trait_func_test* test) {
    char* key = "key", *val1 = "val1", *val2 = "val2";
    size_t copy_count, destroy_count;
    cutil_btree_insert(test->btree, &key, &val1);

    copy_count = cutil_test_trait_tracker_copy_count(test->val_trait_tracker);
    destroy_count = cutil_test_trait_tracker_destroy_count(test->val_trait_tracker);

    cutil_btree_insert(test->btree, &key, &val2);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(test->val_trait_tracker), copy_count + 1);
    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->val_trait_tracker), destroy_count + 1);
}

void add_btree_tests() {
    CTEST_ADD_TEST_F(btree_create, invalid_parameters);
    CTEST_ADD_TEST_F(btree_create, invalid_key_trait_no_compare_func);
    CTEST_ADD_TEST_F(btree_create, order_sets_correct_value);

    CTEST_ADD_TEST_F(btree_size, empty);
    CTEST_ADD_TEST_F(btree_size, non_empty);

    CTEST_ADD_TEST_F(btree_clear, removes_all_items);

    CTEST_ADD_TEST_F(btree_contains, returns_true_if_key_in_leaf_node);
    CTEST_ADD_TEST_F(btree_contains, returns_true_if_key_in_interior_node);
    CTEST_ADD_TEST_F(btree_contains, returns_true_if_key_in_root_node);
    CTEST_ADD_TEST_F(btree_contains, returns_false_if_key_not_present);

    CTEST_ADD_TEST_F(btree_insert, replaces_existing_key);

    CTEST_ADD_TEST_F(btree_insert, odd_split_interior_node_push_up);
    CTEST_ADD_TEST_F(btree_insert, odd_split_leaf_node_right);
    CTEST_ADD_TEST_F(btree_insert, odd_split_leaf_node_left);
    CTEST_ADD_TEST_F(btree_insert, odd_split_leaf_node_middle);

    CTEST_ADD_TEST_F(btree_insert, odd_split_right_new_root);
    CTEST_ADD_TEST_F(btree_insert, odd_split_middle_new_root);
    CTEST_ADD_TEST_F(btree_insert, odd_split_left_new_root);

    CTEST_ADD_TEST_F(btree_insert, even_split_leaf_node_left);
    CTEST_ADD_TEST_F(btree_insert, even_split_leaf_node_middle);
    CTEST_ADD_TEST_F(btree_insert, even_split_leaf_node_right);

    CTEST_ADD_TEST_F(btree_insert, even_split_left_new_root);
    CTEST_ADD_TEST_F(btree_insert, even_split_middle_new_root);
    CTEST_ADD_TEST_F(btree_insert, even_split_right_new_root);

    CTEST_ADD_TEST_F(btree_get, get_pod);
    CTEST_ADD_TEST_F(btree_get, get_ptr);
    CTEST_ADD_TEST_F(btree_get, get_cstring);

    CTEST_ADD_TEST(btree_equals_with_different_traits);
    CTEST_ADD_TEST(btree_equals_empty);

    CTEST_ADD_TEST_F(btree_delete, returns_false_deleting_unknown_key);
    CTEST_ADD_TEST_F(btree_delete, returns_true_deleting_key_that_is_present);
    CTEST_ADD_TEST_F(btree_delete, decrements_size_when_removing_item);
    CTEST_ADD_TEST_F(btree_delete, does_not_decrement_size_when_removing_invalid_item);
    CTEST_ADD_TEST_F(btree_delete, simple_delete_leaf_node);
    CTEST_ADD_TEST_F(btree_delete, first_leaf_node_merge_with_parent);
    CTEST_ADD_TEST_F(btree_delete, second_leaf_node_merge_with_parent);
    CTEST_ADD_TEST_F(btree_delete, leaf_borrow_right);
    CTEST_ADD_TEST_F(btree_delete, leaf_borrow_left);
    CTEST_ADD_TEST_F(btree_delete, interior1);
    CTEST_ADD_TEST_F(btree_delete, interior2);

    CTEST_ADD_TEST_F(btree_trait, pod_methods);
    CTEST_ADD_TEST_F(btree_trait, cstring_methods);
    CTEST_ADD_TEST_F(btree_trait, ptr_methods);

	CTEST_ADD_TEST_F(btree_trait_func, key_copy_on_insert);
	CTEST_ADD_TEST_F(btree_trait_func, key_destroy_on_erase);
	CTEST_ADD_TEST_F(btree_trait_func, key_compare_on_insert);
	CTEST_ADD_TEST_F(btree_trait_func, key_destroy_on_clear);
	CTEST_ADD_TEST_F(btree_trait_func, key_destroy_on_destroy);

	CTEST_ADD_TEST_F(btree_trait_func, val_copy_on_insert);
	CTEST_ADD_TEST_F(btree_trait_func, val_destroy_on_erase);
	CTEST_ADD_TEST_F(btree_trait_func, val_destroy_on_clear);
	CTEST_ADD_TEST_F(btree_trait_func, val_destroy_on_destroy);

    CTEST_ADD_TEST_F(btree_trait_func, val_insert_replace);
}
