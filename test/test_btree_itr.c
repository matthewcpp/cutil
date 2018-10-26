#include "btree.h"

#include "testing.h"
#include "test_suites.h"

#include "test_btree_util.h"

extern cutil_btree* g_btree;
extern void btree_before_each();
extern void btree_after_each();

void btree_itr_empty_tree() {
	cutil_btree_itr*  itr = cutil_btree_itr_create(g_btree);

	CUTIL_TESTING_ASSERT_FALSE(cutil_btree_itr_has_next(itr));

	cutil_btree_itr_destroy(itr);
}

/* ensures that the forward iterator visits nodes in the expected order */
void btree_itr_forward_iteration_order() {
	CUTIL_TESTING_ASSERT_TRUE(read_btree_from_file(g_btree, "btree5_split_interior_left_result"));
	
	int key = 0;
	int prev_key = 0;
	int i = 0;

	cutil_btree_itr*  itr = cutil_btree_itr_create(g_btree);

	while (cutil_btree_itr_next(itr, &key)) {
		if (i) {
			CUTIL_TESTING_ASSERT_TRUE(prev_key < key);
		}

		prev_key = key;
	}

	cutil_btree_itr_destroy(itr);
}



void add_btree_itr_tests() {
	cutil_testing_suite("btree_itr");
	cutil_testing_suite_before_each(&btree_before_each);
	cutil_testing_suite_after_each(&btree_after_each);

	CUTIL_TESTING_ADD(btree_itr_empty_tree);
	CUTIL_TESTING_ADD(btree_itr_forward_iteration_order);
}
