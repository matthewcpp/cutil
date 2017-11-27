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

void btree_test_split_leaf_node_right() {
	insert_char_sequence(g_btree,"ABCDE");

	CUTIL_TESTING_ASSERT_INT_EQ(g_btree->_root->item_count, 1);
	CUTIL_TESTING_ASSERT_INT_EQ(g_btree->_root->keys[0], (int)'C');

	_btree_node* branch = g_btree->_root->branches[0];
	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(branch);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->item_count, 2);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[0], (int)'A');
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[1], (int)'B');

	branch = g_btree->_root->branches[1];
	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(branch);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->item_count, 2);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[0], (int)'D');
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[1], (int)'E');
}


void btree_test_split_leaf_node_left() {
	insert_char_sequence(g_btree, "ACDEB");

	CUTIL_TESTING_ASSERT_INT_EQ(g_btree->_root->item_count, 1);
	CUTIL_TESTING_ASSERT_INT_EQ(g_btree->_root->keys[0], (int)'C');

	_btree_node* branch = g_btree->_root->branches[0];
	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(branch);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->item_count, 2);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[0], (int)'A');
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[1], (int)'B');

	branch = g_btree->_root->branches[1];
	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(branch);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->item_count, 2);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[0], (int)'D');
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[1], (int)'E');
}


void btree_test_split_leaf_node_middle() {
	insert_char_sequence(g_btree, "ABDEC");

	CUTIL_TESTING_ASSERT_INT_EQ(g_btree->_root->item_count, 1);
	CUTIL_TESTING_ASSERT_INT_EQ(g_btree->_root->keys[0], (int)'C');

	_btree_node* branch = g_btree->_root->branches[0];
	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(branch);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->item_count, 2);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[0], (int)'A');
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[1], (int)'B');

	branch = g_btree->_root->branches[1];
	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(branch);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->item_count, 2);
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[0], (int)'D');
	CUTIL_TESTING_ASSERT_INT_EQ(branch->keys[1], (int)'E');
}


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


