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

void btree_even_order_before_each() {
	g_btree = cutil_btree_create_with_order(4);
}

void btree_test_size_empty() {
	CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_size(g_btree), 0);
}

void btree_test_size_non_empty() {
	int expected_tree_size = 5;

	for (int i = 0; i < expected_tree_size; ++i) {
		cutil_btree_insert(g_btree, i);
        CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_size(g_btree), i + 1);
	}
}

void btree_create_order_sets_correct_value() {
	cutil_btree* btree = cutil_btree_create_with_order(4);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_get_order(btree), 4);

	cutil_btree_destroy(btree);
}

void btree_find_returns_true_if_key_in_leaf_node(){
    CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_left_result"));
    CUTIL_TESTING_ASSERT_TRUE(cutil_btree_find(g_btree, 34));
}

void btree_find_returns_true_if_key_in_interior_node(){
    CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_left_result"));
    CUTIL_TESTING_ASSERT_TRUE(cutil_btree_find(g_btree, 30));
}

void btree_find_returns_true_if_key_in_root_node(){
    CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_left_result"));
    CUTIL_TESTING_ASSERT_TRUE(cutil_btree_find(g_btree, 20));
}

void btree_find_returns_false_if_key_not_present(){
    for (int i =0; i < 10; ++i){
        cutil_btree_insert(g_btree, i);
    }

    for (int i =100; i < 110; ++i){
        CUTIL_TESTING_ASSERT_FALSE(cutil_btree_find(g_btree, i));
    }
}

void btree_clear_removes_all_items(){
    for (int i =0; i < 10; ++i){
        cutil_btree_insert(g_btree, i);
    }

    CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_size(g_btree), 10);
    CUTIL_TESTING_ASSERT_TRUE(cutil_btree_find(g_btree, 5));

    cutil_btree_clear(g_btree);

    CUTIL_TESTING_ASSERT_INT_EQ(cutil_btree_size(g_btree), 0);
    CUTIL_TESTING_ASSERT_FALSE(cutil_btree_find(g_btree, 5));
}

void btree_split_interior_node_push_up(){
    CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior_push_up"));
    CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

    cutil_btree_insert(g_btree, 123);
    CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

    cutil_btree *expected_btree = cutil_btree_create();
    read_btree_from_file(expected_btree, "btree4_split_interior_push_up_result");

    CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
    cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree  with odd number order that requires a leaf node to be split to the right of the node pivot */
void btree_test_split_leaf_node_right() {
	insert_char_sequence(g_btree,"ABCDE");

	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree5_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with odd number order that requires a leaf node to be split to the left of the node pivot */
void btree_test_split_leaf_node_left() {
	insert_char_sequence(g_btree, "ACDEB");

	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree5_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with odd number order that requires a leaf node to be split at the node pivot */
void btree_test_split_leaf_node_middle() {
	insert_char_sequence(g_btree, "ABDEC");

	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree5_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	loads a tree and inserts a value that will require a split of an interior node at the right of the pivot and
	creates a new root node */
void btree_test_split_right_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior"));

	cutil_btree_insert(g_btree, 50);
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_interior_right_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}


/*	loads a tree and inserts a value that will require a split of an interior node at the pivot and
	creates a new root node */
void btree_test_split_middle_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior"));

	cutil_btree_insert(g_btree, 25);
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_interior_middle_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	loads a tree and inserts a value that will require a split of an interior node to the left of a pivot and
	creates a new root node */
void btree_test_split_left_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior"));

	cutil_btree_insert(g_btree, 1);
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_split_interior_left_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with even number order that requires a leaf node to be split to the left of the node pivot */
void btree_even_test_split_leaf_node_left() {
	insert_char_sequence(g_btree, "ADF");
	cutil_btree_insert(g_btree, (int)'B');

	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));
	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree4_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with even number order that requires a leaf node to be split at the node pivot */
void btree_even_test_split_leaf_node_middle() {
	insert_char_sequence(g_btree, "ABF");
	cutil_btree_insert(g_btree, (int)'D');

	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));
	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree4_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	inserts items into the tree with even number order that requires a leaf node to be split to the right of the node pivot */
void btree_even_test_split_leaf_node_right() {
	insert_char_sequence(g_btree, "ABD");
	cutil_btree_insert(g_btree, (int)'F');

	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));
	cutil_btree *expected_btree = cutil_btree_create();
	read_btree_from_file(expected_btree, "btree4_split_leaf_new_root_result");

	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));
	cutil_btree_destroy(expected_btree);
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the left of the pivot and
creates a new root node */
void btree_even_test_split_left_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior"));

	cutil_btree_insert(g_btree, 1);
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_left_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the the pivot and
creates a new root node */
void btree_even_test_split_middle_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior"));

	cutil_btree_insert(g_btree, 21);
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_middle_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/*	loads a tree of even order and inserts a value that will require a split of an interior node at the right of the pivot and
creates a new root node */
void btree_even_test_split_right_new_root() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree4_split_interior"));

	cutil_btree_insert(g_btree, 38);
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree4_split_interior_right_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/* deletes a key from a leaf node that has extra keys and does not require any borrowing.
*/
void btree_simple_delete_leaf_node() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_delete_leaf_noborrow"));

	CUTIL_TESTING_ASSERT_TRUE(cutil_btree_delete(g_btree, 75));
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	CUTIL_TESTING_ASSERT_FALSE(cutil_btree_find(g_btree, 75));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_delete_leaf_noborrow_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/* deletes a key from a leaf node that requires a borrow from its sibing on the right.
*/
void btree_delete_leaf_borrow_right() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_delete_leaf_borrow_right"));

	CUTIL_TESTING_ASSERT_TRUE(cutil_btree_delete(g_btree, 55));
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	CUTIL_TESTING_ASSERT_FALSE(cutil_btree_find(g_btree, 55));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_delete_leaf_borrow_right_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}

/* deletes a key from a leaf node that requires a borrow from its sibling on the left.
*/
void btree_delete_leaf_borrow_left() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_delete_leaf_borrow_left"));

	CUTIL_TESTING_ASSERT_TRUE(cutil_btree_delete(g_btree, 31));
	CUTIL_TESTING_ASSERT_TRUE(validate_btree(g_btree));

	CUTIL_TESTING_ASSERT_FALSE(cutil_btree_find(g_btree, 31));

	cutil_btree *expected_btree = cutil_btree_create();
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(expected_btree, "btree5_delete_leaf_borrow_left_result"));
	CUTIL_TESTING_ASSERT_TRUE(compare_btrees(expected_btree, g_btree));

	cutil_btree_destroy(expected_btree);
}


void add_btree_tests() {
	cutil_testing_suite("btree");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_create_order_sets_correct_value);

	CUTIL_TESTING_ADD(btree_test_size_empty);
	CUTIL_TESTING_ADD(btree_test_size_non_empty);

    CUTIL_TESTING_ADD(btree_clear_removes_all_items);

    CUTIL_TESTING_ADD(btree_find_returns_true_if_key_in_leaf_node);
    CUTIL_TESTING_ADD(btree_find_returns_true_if_key_in_interior_node);
    CUTIL_TESTING_ADD(btree_find_returns_true_if_key_in_root_node);
    CUTIL_TESTING_ADD(btree_find_returns_false_if_key_not_present);

    CUTIL_TESTING_ADD(btree_split_interior_node_push_up);

	CUTIL_TESTING_ADD(btree_test_split_leaf_node_right);
	CUTIL_TESTING_ADD(btree_test_split_leaf_node_left);
	CUTIL_TESTING_ADD(btree_test_split_leaf_node_middle);

	CUTIL_TESTING_ADD(btree_test_split_right_new_root);
	CUTIL_TESTING_ADD(btree_test_split_middle_new_root);
	CUTIL_TESTING_ADD(btree_test_split_left_new_root);

	cutil_testing_suite("btree_even_order");
	cutil_testing_suite_before_each(&btree_even_order_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_even_test_split_leaf_node_left);
	CUTIL_TESTING_ADD(btree_even_test_split_leaf_node_middle);
	CUTIL_TESTING_ADD(btree_even_test_split_leaf_node_right);

	CUTIL_TESTING_ADD(btree_even_test_split_left_new_root);
	CUTIL_TESTING_ADD(btree_even_test_split_right_new_root);
	CUTIL_TESTING_ADD(btree_even_test_split_middle_new_root);

	cutil_testing_suite("btree_delete");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);
	CUTIL_TESTING_ADD(btree_simple_delete_leaf_node);
	CUTIL_TESTING_ADD(btree_delete_leaf_borrow_right);
	CUTIL_TESTING_ADD(btree_delete_leaf_borrow_left);
}


