#include "btree.h"

#include "testing.h"
#include "test_suites.h"

#include "test_btree_util.h"

#include <stdlib.h>

cutil_btree *g_btree = NULL;


void btree_before_each() {
	g_btree = cutil_btree_create();
}

void btree_after_each() {
	cutil_btree_destroy(g_btree);
	g_btree = NULL;
}

void btree_test_size_empty() {
	CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_size(g_btree), 0);
}

void btree_test_size_non_empty() {
	int expected_tree_size = 5;

	for (int i = 0; i < expected_tree_size; ++i) {
		cutil_btree_insert(g_btree, i);
	}
	CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_size(g_btree), 5);
}

/*	inserts items into the tree that requires a leaf node to be split to the right of the node pivot */
void btree_test_split_leaf_node_right() {
	insert_char_sequence(g_btree,"ABCDE");

	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
}

/*	inserts items into the tree that requires a leaf node to be split to the left of the node pivot */
void btree_test_split_leaf_node_left() {
	insert_char_sequence(g_btree, "ACDEB");

	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
}

/*	inserts items into the tree that requires a leaf node to be split in the middle of the node */
void btree_test_split_leaf_node_middle() {
	insert_char_sequence(g_btree, "ABDEC");

	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
}

/*	loads a tree and inserts a value that will require a split of an interior node at the right of the pivot and
	creates a new root node */
void btree_test_split_right_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree_split_interior_right"));

	cutil_btree_insert(g_btree, 83);

	const char* expected_sequence = "ACDEFGHKLMNPQRSTWXYZ";
	CUTIL_TESTING_ASSERT_TRUE(confirm_forward_iteration_char_sequence(g_btree, expected_sequence));
}

/*	loads a tree and inserts a value that will require a split of an interior node at the pivot and
	creates a new root node */
void btree_test_split_middle_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree_split_interior_middle"));

	cutil_btree_insert(g_btree, 74);

	const char* expected_sequence = "ACDEFGHIJKLMNPQRTWXYZ";
	CUTIL_TESTING_ASSERT_TRUE(confirm_forward_iteration_char_sequence(g_btree, expected_sequence));
}

/*	loads a tree and inserts a value that will require a split of an interior node to the left of a pivot and
	creates a new root node */
void btree_test_split_left_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree_split_interior_left"));

	cutil_btree_insert(g_btree, 69);

	const char* expected_sequence = "ABCDEGHIJLMNOPQRST";
	CUTIL_TESTING_ASSERT_TRUE(confirm_forward_iteration_char_sequence(g_btree, expected_sequence));
}


void add_btree_tests() {
	cutil_testing_suite("btree");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_test_size_empty);
	CUTIL_TESTING_ADD(btree_test_size_non_empty);

	CUTIL_TESTING_ADD(btree_test_split_leaf_node_right);
	CUTIL_TESTING_ADD(btree_test_split_leaf_node_left);
	CUTIL_TESTING_ADD(btree_test_split_leaf_node_middle);

	CUTIL_TESTING_ADD(btree_test_split_right_new_root);
	CUTIL_TESTING_ADD(btree_test_split_middle_new_root);
	CUTIL_TESTING_ADD(btree_test_split_left_new_root);
}


