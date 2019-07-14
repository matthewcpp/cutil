#include "cutil/btree.h"

#include "ctest/ctest.h"
#include "test_suites.h"

#include "test_btree_util.h"
#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_ODD_BTREE_ORDER 5
#define DEFAULT_EVEN_BTREE_ORDER 4

cutil_btree* g_btree = NULL;

cutil_trait* g_btree_key_trait = NULL;
cutil_trait* g_btree_val_trait = NULL;


void btree_before_each() {
	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
}

void btree_after_each() {
	if (g_btree) {
		cutil_btree_destroy(g_btree);
		g_btree = NULL;
	}

	if (g_btree_key_trait) {
		cutil_test_destroy_trait_tracker(g_btree_key_trait);
		g_btree_key_trait = NULL;
	}

	if (g_btree_val_trait) {
		cutil_test_destroy_trait_tracker(g_btree_val_trait);
		g_btree_val_trait = NULL;
	}

	cutil_trait_destroy();
}

void btree_even_order_before_each() {
	g_btree = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
}

void btree_test_size_empty() {
	CTEST_ASSERT_INT_EQ(cutil_btree_size(g_btree), 0);
}

void btree_test_size_non_empty() {
	int expected_tree_size = 5;

	int i;
	for (i = 0; i < expected_tree_size; ++i) {
		cutil_btree_insert(g_btree, &i, &i);
		CTEST_ASSERT_INT_EQ(cutil_btree_size(g_btree), i + 1);
	}
}

void btree_invalid_key_trait_no_copy() {
	cutil_btree* result;

	cutil_trait* bogus_trait = malloc(sizeof(cutil_trait));
	memcpy(bogus_trait, cutil_trait_int(), sizeof(cutil_trait));
	bogus_trait->compare_func = NULL;

	result = cutil_btree_create(5, bogus_trait, cutil_trait_int());
	CTEST_EXPECT_PTR_NULL(result);

	free(bogus_trait);
}

void btree_invalid_creation_parameters() {
	cutil_btree* result = cutil_btree_create(-1, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_PTR_NULL(result);

	result = cutil_btree_create(5, NULL, cutil_trait_int());
	CTEST_ASSERT_PTR_NULL(result);

	result = cutil_btree_create(5, cutil_trait_int(), NULL);
	CTEST_ASSERT_PTR_NULL(result);
}

void btree_create_order_sets_correct_value() {
	cutil_btree* btree = cutil_btree_create(4, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_INT_EQ(cutil_btree_get_order(btree), 4);
	cutil_btree_destroy(btree);

	CTEST_ASSERT_INT_EQ(cutil_btree_get_order(g_btree), DEFAULT_ODD_BTREE_ORDER);
}

void btree_contains_returns_true_if_key_in_leaf_node(){
	int expected_to_contain_value = 34;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_left_result"));
	CTEST_ASSERT_TRUE(cutil_btree_contains(g_btree, &expected_to_contain_value));
}

void btree_contains_returns_true_if_key_in_interior_node(){
	int expected_to_contain_value = 30;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_left_result"));
	CTEST_ASSERT_TRUE(cutil_btree_contains(g_btree, &expected_to_contain_value));
}

void btree_contains_returns_true_if_key_in_root_node(){
	int expected_to_contain_value = 20;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_left_result"));
	CTEST_ASSERT_TRUE(cutil_btree_contains(g_btree, &expected_to_contain_value));
}

void btree_contains_returns_false_if_key_not_present(){
	int i;
	for (i =0; i < 10; ++i){
		cutil_btree_insert(g_btree, &i, &i);
	}

	for (i =100; i < 110; ++i){
		CTEST_ASSERT_FALSE(cutil_btree_contains(g_btree, &i));
	}
}

void btree_clear_removes_all_items(){
	int i;
	for (i =0; i < 10; ++i){
		cutil_btree_insert(g_btree, &i, &i);
	}

	CTEST_ASSERT_INT_EQ(cutil_btree_size(g_btree), 10);

	cutil_btree_clear(g_btree);

	CTEST_ASSERT_INT_EQ(cutil_btree_size(g_btree), 0);
	for (i = 0; i < 10; ++i) {
		CTEST_ASSERT_FALSE(cutil_btree_contains(g_btree, &i));
	}

}

void btree_insert_replaces_existing_key() {
    int key = 123;
    int expected_value1 = 1000, expected_value2 = 2000;
    int actual_value = 0;

    cutil_btree_insert(g_btree, &key, &expected_value1);
    CTEST_ASSERT_TRUE(cutil_btree_get(g_btree, &key, &actual_value));
    CTEST_ASSERT_INT_EQ(expected_value1, actual_value);

    cutil_btree_insert(g_btree, &key, &expected_value2);
    CTEST_ASSERT_TRUE(cutil_btree_get(g_btree, &key, &actual_value));
    CTEST_ASSERT_INT_EQ(expected_value2, actual_value);
}

void btree_split_interior_node_push_up(){
	int key = 123;
	cutil_btree* expected_btree = NULL;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_push_up"));
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(4, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_push_up_result"));

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree  with odd number order that requires a leaf node to be split to the right of the node pivot */
void btree_test_split_leaf_node_right() {
	cutil_btree* expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_leaf_new_root_result"));

	insert_char_sequence(g_btree,"ABCDE", NULL);

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with odd number order that requires a leaf node to be split to the left of the node pivot */
void btree_test_split_leaf_node_left() {
	cutil_btree* expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_leaf_new_root_result"));

	insert_char_sequence(g_btree, "ACDEB", NULL);

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with odd number order that requires a leaf node to be split at the node pivot */
void btree_test_split_leaf_node_middle() {
	cutil_btree* expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_leaf_new_root_result"));

	insert_char_sequence(g_btree, "ABDEC", NULL);

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	loads a tree and inserts a value that will require a split of an interior node at the right of the pivot and
	creates a new root node */
void btree_test_split_right_new_root() {
	cutil_btree* expected_btree = NULL;
	int key = 50;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior"));

	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_interior_right_result"));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}


/*	loads a tree and inserts a value that will require a split of an interior node at the pivot and
	creates a new root node */
void btree_test_split_middle_new_root() {
	cutil_btree* expected_btree = NULL;
	int key = 25;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior"));

	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_interior_middle_result"));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	loads a tree and inserts a value that will require a split of an interior node to the left of a pivot and
	creates a new root node */
void btree_test_split_left_new_root() {
	cutil_btree* expected_btree = NULL;
	int key = 1;
	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior"));

	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_interior_left_result"));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with even number order that requires a leaf node to be split to the left of the node pivot */
void btree_even_test_split_leaf_node_left() {
	cutil_btree* expected_btree = NULL;
	int key = (int)'B';

	insert_char_sequence(g_btree, "ADF", NULL);
	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_leaf_new_root_result"));

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with even number order that requires a leaf node to be split at the node pivot */
void btree_even_test_split_leaf_node_middle() {
	cutil_btree* expected_btree = NULL;
	int key = (int)'D';

	insert_char_sequence(g_btree, "ABF", NULL);
	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_leaf_new_root_result"));

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with even number order that requires a leaf node to be split to the right of the node pivot */
void btree_even_test_split_leaf_node_right() {
	cutil_btree* expected_btree = NULL;
	int key = (int)'F';

	insert_char_sequence(g_btree, "ABD", NULL);
	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_leaf_new_root_result"));

	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the left of the pivot and
creates a new root node */
void btree_even_test_split_left_new_root() {
	cutil_btree* expected_btree = NULL;
	int key = 1;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior"));
	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_left_result"));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the pivot and
creates a new root node */
void btree_even_test_split_middle_new_root() {
	int key = 21;
	cutil_btree* expected_btree = NULL;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior"));
	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_middle_result"));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the right of the pivot and
creates a new root node */
void btree_even_test_split_right_new_root() {
	cutil_btree* expected_btree = NULL;
	int key = 38;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior"));
	cutil_btree_insert(g_btree, &key, &key);
	CTEST_ASSERT_TRUE(validate_btree(g_btree));

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_right_result"));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

void btree_delete_decrements_size_when_removing_item() {
	size_t starting_size = 0;
	size_t actual_size = 0;
	int value = (int)'A';

	insert_char_sequence(g_btree, "ABF", NULL);
	starting_size = cutil_btree_size(g_btree);

	cutil_btree_erase(g_btree, &value);
	actual_size = cutil_btree_size(g_btree);
	CTEST_ASSERT_INT_EQ(actual_size, starting_size - 1);
}

void btree_delete_does_not_decrement_size_when_removing_invalid_item() {
	size_t starting_size = 0;
	size_t actual_size = 0;
	int value = (int)'P';

	insert_char_sequence(g_btree, "ABF", NULL);
	starting_size = cutil_btree_size(g_btree);

	cutil_btree_erase(g_btree, &value);
	actual_size = cutil_btree_size(g_btree);
	CTEST_ASSERT_INT_EQ(actual_size, starting_size);
}

void btree_delete_returns_true_deleting_key_that_is_present() {
	int key = (int)'A';

	insert_char_sequence(g_btree, "ABF", NULL);
	CTEST_ASSERT_TRUE(cutil_btree_erase(g_btree, &key));
}

void btree_delete_returns_false_deleting_unknown_key() {
	int key = (int)'Z';

	insert_char_sequence(g_btree, "ABF", NULL);
	CTEST_ASSERT_FALSE(cutil_btree_erase(g_btree, &key));
}

/** utility method for running a delete test.  Loads a tree, removes a key, and validates it aginst the expected result */
void do_btree_delete_test(const char* test_tree_data, const char* expected_tree_data, int* keys_to_delete, int key_count) {
	size_t original_tree_size = 0;
	size_t actual_tree_size = 0;
	cutil_btree* expected_btree = NULL;
	int i;

	CTEST_ASSERT_TRUE(read_btree_from_file(g_btree, test_tree_data));
	original_tree_size = cutil_btree_size(g_btree);

	for (i = 0; i < key_count; i++) {
		CTEST_ASSERT_TRUE(cutil_btree_erase(g_btree, keys_to_delete + i));
		CTEST_ASSERT_FALSE(cutil_btree_contains(g_btree, keys_to_delete + i));
		CTEST_ASSERT_TRUE(validate_btree(g_btree));
	}

	expected_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	CTEST_ASSERT_TRUE(read_btree_from_file(expected_btree, expected_tree_data));
	CTEST_ASSERT_TRUE(cutil_btree_equals(expected_btree, g_btree));

	actual_tree_size = cutil_btree_size(g_btree);
	CTEST_ASSERT_INT_EQ(actual_tree_size, original_tree_size - key_count);

	cutil_btree_destroy(expected_btree);
}

/* deletes a key from a leaf node that has extra keys and does not require any rebalancing. */
void btree_simple_delete_leaf_node() {
	int keys[] = { 5 };
	do_btree_delete_test("btree3_delete_leaf_simple", "btree3_delete_leaf_simple_result", keys, 1);
}

/** deletes a leaf from the first branch of the root.  Requires merging the first branch with its right sibling and removal of an invalid root */
void btree_delete_first_leaf_node_merge_with_parent() {
	int keys[] = { 10 };
	do_btree_delete_test("btree3_delete_simple_merge_and_reblance_root", "btree3_delete_first_leaf_node_merge_with_parent_result", keys, 1);
}

/** deletes a leaf node from the second branch of the root.  Requires merging the from the left node (first branch) and removal of an invalid root*/
void btree_delete_second_leaf_node_merge_with_parent() {
	int keys[] = { 200 };
	do_btree_delete_test("btree3_delete_simple_merge_and_reblance_root", "btree3_delete_second_leaf_node_merge_with_parent_result", keys, 1);
}

void btree_leaf_delete_borrow_right() {
	int keys[] = { 20 };
	do_btree_delete_test("btree3_leaf_delete_borrow_right", "btree3_leaf_delete_borrow_right_result", keys, 1);
}

void btree_leaf_delete_borrow_left() {
	int keys[] = { 50 };
	do_btree_delete_test("btree3_leaf_delete_borrow_left", "btree3_leaf_delete_borrow_left_result", keys, 1);
}

void btree_delete_interior1() {
	int keys[] = { 36 };
	do_btree_delete_test("btree4_delete_interior", "btree4_delete_interior_result_1", keys, 1);
}

void btree_delete_interior2() {
	int keys[] = { 36, 28, 22 };
	do_btree_delete_test("btree4_delete_interior", "btree4_delete_interior_result_2", keys, 3);
}

void btree_trait_pod() {
	int i, item_count = 15;

	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

	for (i = item_count - 1; i >= 0; i--) {
		cutil_btree_insert(g_btree, &i, &i);
		CTEST_EXPECT_TRUE(cutil_btree_contains(g_btree, &i));
	}

	for (i = item_count - 1; i >= 0; i--) {
		CTEST_ASSERT_TRUE(cutil_btree_erase(g_btree, &i));
	}

	CTEST_ASSERT_INT_EQ(0, cutil_btree_size(g_btree));
}

void btree_trait_cstring() {
	int i, item_count = 15;
	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_cstring(), cutil_trait_cstring());

	for (i = item_count - 1; i >= 0; i--) {
		int contains_result;
		char* insert_str = malloc(32);
		cutil_snprintf_func(insert_str, 32, "test string %i", i);

		cutil_btree_insert(g_btree, &insert_str, &insert_str);
		contains_result = cutil_btree_contains(g_btree, &insert_str);
		free(insert_str);

		CTEST_ASSERT_TRUE(contains_result);
	}

	CTEST_ASSERT_INT_EQ(item_count, cutil_btree_size(g_btree));

	for (i = item_count - 1; i >= 0; i--) {
		int delete_result;
		char* insert_str = malloc(32);
		cutil_snprintf_func(insert_str, 32, "test string %i", i);

		delete_result = cutil_btree_erase(g_btree, &insert_str);
		free(insert_str);

		CTEST_ASSERT_TRUE(delete_result);
	}

	CTEST_ASSERT_INT_EQ(0, cutil_btree_size(g_btree));
}



void btree_trait_ptr() {
	int i, item_count = 15;
	int** test_ptrs = calloc(item_count, sizeof(int*));

	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_ptr(), cutil_trait_ptr());

	for (i = item_count -1; i >= 0; i--) {
		void* ptr_val = NULL;
		int* test_ptr = malloc(sizeof(int));
		*test_ptr = i;
		test_ptrs[i] = test_ptr;

		cutil_btree_insert(g_btree, &test_ptr, &ptr_val);
		CTEST_EXPECT_TRUE(cutil_btree_contains(g_btree, &test_ptr));
	}

	for (i = item_count - 1; i >= 0; i--) {
		int* test_ptr = test_ptrs[i];
		CTEST_EXPECT_TRUE(cutil_btree_erase(g_btree, &test_ptr));
	}

	/* test cleanup */
	for (i = item_count - 1; i >= 0; i--) {
		free(test_ptrs[i]);
	}

	CTEST_ASSERT_INT_EQ(0, cutil_btree_size(g_btree));

	free(test_ptrs);
}

void btree_test_get_pod() {
	int i, item_count = 10;

	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());


	for (i = 1; i <= item_count; i++) {
		int expected_value = i * 10;
		cutil_btree_insert(g_btree, &i, &expected_value);
	}

	for (i = 1; i <= item_count; i++) {
		int expected_value = i * 10;
		int actual_value = 0;
		int result = cutil_btree_get(g_btree, &i, &actual_value);
		CTEST_ASSERT_TRUE(result)
		CTEST_ASSERT_INT_EQ(expected_value, actual_value);
	}
}


void bree_test_get_ptr() {
	int* expected_ptr = NULL;
	int i, item_count = 10;

	int** keys = calloc(item_count, sizeof(int*));
	int** values = calloc(item_count, sizeof(int*));

	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_ptr(), cutil_trait_ptr());

	for (i = 0; i < item_count; i++) {
		keys[i] = malloc(sizeof(int));
		values[i] = malloc(sizeof(int));

		cutil_btree_insert(g_btree, &keys[i], &values[i]);
	}

	for (i = 0; i < item_count; i++) {
		cutil_btree_get(g_btree, &keys[i], &expected_ptr);

		CTEST_EXPECT_PTR_EQ(values[i], expected_ptr);
	}


	/* test cleanup */
	for (i = 0; i < item_count; i++) {
		free(keys[i]);
		free(values[i]);
	}

	free(keys);
	free(values);
}

void btree_test_get_cstring() {
	int i, item_count = 10;

	char key_str[32];
	char value_str[32];
	char actual_str[32];

	char* key_ptr = &key_str[0];
	char* value_ptr = &value_str[0];
	char* actual_ptr = &actual_str[0];

	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_cstring(), cutil_trait_cstring());

	for (i = 0; i < item_count; i++) {
		cutil_snprintf_func(key_str, 32, "key %i", i);
		cutil_snprintf_func(value_str, 32, "value %i", i);

		cutil_btree_insert(g_btree, &key_ptr, &value_ptr);
	}

	for (i = 0; i < item_count; i++) {
		cutil_snprintf_func(key_str, 32, "key %i", i);
		cutil_snprintf_func(value_str, 32, "value %i", i);

		cutil_btree_get(g_btree, &key_ptr, &actual_ptr);

		CTEST_ASSERT_INT_EQ(strcmp(value_ptr, actual_ptr), 0);
	}
}
void btree_test_equals_traits() {
	cutil_btree* btree1 = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	cutil_btree* btree2 = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_cstring(), cutil_trait_int());

	/* key trait */
	CTEST_EXPECT_FALSE(cutil_btree_equals(btree1, btree2));
	cutil_btree_destroy(btree2);

	/* value trait */
	btree2 = cutil_btree_create(DEFAULT_EVEN_BTREE_ORDER, cutil_trait_int(), cutil_trait_cstring());
	CTEST_EXPECT_FALSE(cutil_btree_equals(btree1, btree2));
	cutil_btree_destroy(btree2);

	cutil_btree_destroy(btree1);
}

void btree_test_equals_empty() {
	cutil_btree* btree1 = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());
	cutil_btree* btree2 = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, cutil_trait_int(), cutil_trait_int());

	/* empty trees should be equal */
	CTEST_EXPECT_TRUE(cutil_btree_equals(btree1, btree2));

	cutil_btree_destroy(btree1);
	cutil_btree_destroy(btree2);
}

void btree_trait_func_before_each() {
	g_btree_key_trait = cutil_test_create_trait_tracker(cutil_trait_cstring());
	g_btree_val_trait = cutil_test_create_trait_tracker(cutil_trait_cstring());

	g_btree = cutil_btree_create(DEFAULT_ODD_BTREE_ORDER, g_btree_key_trait, g_btree_val_trait);
}

void btree_key_trait_copy_on_insert() {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(g_btree, &key, &value);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(g_btree_key_trait), 1);
}

void btree_key_trait_destroy_on_erase() {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(g_btree, &key, &value);
	CTEST_ASSERT_TRUE(cutil_btree_erase(g_btree, &key));

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_key_trait), 1);
}

void btree_key_trait_compare_on_insert() {
	char* key1 = "test key1";
	char* key2 = "test key2";
	char* value = "test value";

	cutil_btree_insert(g_btree, &key1, &value);
	cutil_btree_insert(g_btree, &key2, &value);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_compare_count(g_btree_key_trait), 1);
}


void btree_val_trait_copy_on_insert() {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(g_btree, &key, &value);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(g_btree_val_trait), 1);
}

void btree_val_trait_destroy_on_erase() {
	char* key = "test key";
	char* value = "test value";

	cutil_btree_insert(g_btree, &key, &value);
	CTEST_ASSERT_TRUE(cutil_btree_erase(g_btree, &key));

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_val_trait), 1);
}

void _btree_test_insert_test_strings(cutil_btree* btree, int item_count) {
	int i;

	char key_str[32];
	char value_str[32];

	char* key_ptr = &key_str[0];
	char* value_ptr = &value_str[0];

	for (i = 0; i < item_count; i++) {
		cutil_snprintf_func(key_str, 32, "key %i", i);
		cutil_snprintf_func(value_str, 32, "value %i", i);

		cutil_btree_insert(btree, &key_ptr, &value_ptr);
	}
}

void btree_val_trait_destroy_on_clear() {
	int item_count = 10;
	_btree_test_insert_test_strings(g_btree, item_count);

	cutil_btree_clear(g_btree);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_val_trait), item_count);
}

void btree_key_trait_destroy_on_clear() {
	int item_count = 10;
	_btree_test_insert_test_strings(g_btree, item_count);

	cutil_btree_clear(g_btree);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_key_trait), item_count);
}

void btree_val_trait_destroy_on_destroy() {
	int item_count = 10;
	_btree_test_insert_test_strings(g_btree, item_count);

	cutil_btree_destroy(g_btree);
	g_btree = NULL;

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_val_trait), item_count);
}

void btree_key_trait_destroy_on_destroy() {
	int item_count = 10;
	_btree_test_insert_test_strings(g_btree, item_count);

	cutil_btree_destroy(g_btree);
	g_btree = NULL;

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_key_trait), item_count);
}

void btree_val_trait_insert_replace() {
    char* key = "key", *val1 = "val1", *val2 = "val2";
    size_t copy_count, destroy_count;
    cutil_btree_insert(g_btree, &key, &val1);

    copy_count = cutil_test_trait_tracker_copy_count(g_btree_val_trait);
    destroy_count = cutil_test_trait_tracker_destroy_count(g_btree_val_trait);

    cutil_btree_insert(g_btree, &key, &val2);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(g_btree_val_trait), copy_count + 1);
    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_btree_val_trait), destroy_count + 1);
}

void add_btree_tests() {
    ctest_suite("btree_insert_odd_order");
    ctest_suite_before_each(&btree_before_each);
    ctest_suite_after_each(&btree_after_each);

    CTEST_ADD_TEST(btree_invalid_creation_parameters);
	CTEST_ADD_TEST(btree_invalid_key_trait_no_copy);
    CTEST_ADD_TEST(btree_create_order_sets_correct_value);

    CTEST_ADD_TEST(btree_test_size_empty);
    CTEST_ADD_TEST(btree_test_size_non_empty);

    CTEST_ADD_TEST(btree_clear_removes_all_items);

    CTEST_ADD_TEST(btree_contains_returns_true_if_key_in_leaf_node);
    CTEST_ADD_TEST(btree_contains_returns_true_if_key_in_interior_node);
    CTEST_ADD_TEST(btree_contains_returns_true_if_key_in_root_node);
    CTEST_ADD_TEST(btree_contains_returns_false_if_key_not_present);

    CTEST_ADD_TEST(btree_insert_replaces_existing_key);

    CTEST_ADD_TEST(btree_split_interior_node_push_up);

    CTEST_ADD_TEST(btree_test_split_leaf_node_right);
    CTEST_ADD_TEST(btree_test_split_leaf_node_left);
    CTEST_ADD_TEST(btree_test_split_leaf_node_middle);

    CTEST_ADD_TEST(btree_test_split_right_new_root);
    CTEST_ADD_TEST(btree_test_split_middle_new_root);
    CTEST_ADD_TEST(btree_test_split_left_new_root);

    ctest_suite("btree_insert_even_order");
    ctest_suite_before_each(&btree_even_order_before_each);
    ctest_suite_after_each(&btree_after_each);

    CTEST_ADD_TEST(btree_even_test_split_leaf_node_left);
    CTEST_ADD_TEST(btree_even_test_split_leaf_node_middle);
    CTEST_ADD_TEST(btree_even_test_split_leaf_node_right);

    CTEST_ADD_TEST(btree_even_test_split_left_new_root);
    CTEST_ADD_TEST(btree_even_test_split_right_new_root);
    CTEST_ADD_TEST(btree_even_test_split_middle_new_root);

    ctest_suite("btree_get");
    ctest_suite_after_each(&btree_after_each);
    CTEST_ADD_TEST(btree_test_get_pod);
    CTEST_ADD_TEST(bree_test_get_ptr);
    CTEST_ADD_TEST(btree_test_get_cstring);

    ctest_suite("btree_equals");
	ctest_suite_after_each(&btree_after_each);
    CTEST_ADD_TEST(btree_test_equals_traits);
    CTEST_ADD_TEST(btree_test_equals_empty);

    ctest_suite("btree_delete");
    ctest_suite_before_each(&btree_before_each);
    ctest_suite_after_each(&btree_after_each);
    CTEST_ADD_TEST(btree_delete_returns_false_deleting_unknown_key);
    CTEST_ADD_TEST(btree_delete_returns_true_deleting_key_that_is_present);
    CTEST_ADD_TEST(btree_delete_decrements_size_when_removing_item);
    CTEST_ADD_TEST(btree_delete_does_not_decrement_size_when_removing_invalid_item);
    CTEST_ADD_TEST(btree_simple_delete_leaf_node);
    CTEST_ADD_TEST(btree_delete_first_leaf_node_merge_with_parent);
    CTEST_ADD_TEST(btree_delete_second_leaf_node_merge_with_parent);
    CTEST_ADD_TEST(btree_leaf_delete_borrow_right);
    CTEST_ADD_TEST(btree_leaf_delete_borrow_left);
    CTEST_ADD_TEST(btree_delete_interior1);
    CTEST_ADD_TEST(btree_delete_interior2);

    ctest_suite("btree_traits");
    ctest_suite_after_each(&btree_after_each);
    CTEST_ADD_TEST(btree_trait_pod);
    CTEST_ADD_TEST(btree_trait_cstring);
    CTEST_ADD_TEST(btree_trait_ptr);

	ctest_suite("btree_trait_func");
	ctest_suite_before_each(&btree_trait_func_before_each);
	ctest_suite_after_each(&btree_after_each);

	CTEST_ADD_TEST(btree_key_trait_copy_on_insert);
	CTEST_ADD_TEST(btree_key_trait_compare_on_insert);
	CTEST_ADD_TEST(btree_key_trait_destroy_on_erase);
	CTEST_ADD_TEST(btree_key_trait_destroy_on_clear);
	CTEST_ADD_TEST(btree_key_trait_destroy_on_destroy);

	CTEST_ADD_TEST(btree_val_trait_copy_on_insert);
	CTEST_ADD_TEST(btree_val_trait_destroy_on_erase);
	CTEST_ADD_TEST(btree_val_trait_destroy_on_clear);
	CTEST_ADD_TEST(btree_val_trait_destroy_on_destroy);

    CTEST_ADD_TEST(btree_val_trait_insert_replace);
}
