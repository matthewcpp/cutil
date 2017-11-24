#include "btree.h"

#include "testing.h"
#include "test_suites.h"

#include "test_btree_util.h"

extern cutil_btree *g_btree;
extern void btree_before_each();
extern void btree_after_each();

void btree_itr_forward_iter() {
	insert_char_sequence(g_btree, "ACGNHEKQMFWLTZDPRXYS");

	const char* expected_sequence = "ACDEFGHKLMNPQRSTWXYZ";
	CUTIL_TESTING_ASSERT_TRUE(confirm_forward_iteration_sequence(g_btree, expected_sequence));
}

void add_btree_itr_tests() {
	cutil_testing_suite("btree_itr");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_itr_forward_iter);
}