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

void add_btree_tests() {
	cutil_testing_suite("btree");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_test_size_empty);
	CUTIL_TESTING_ADD(btree_test_size_non_empty);
}

