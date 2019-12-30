#include "test_btree_fixtures.h"

#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include <string.h>
#include <stdlib.h>

void btree_test_setup(btree_test* test) {
    memset(test, 0, sizeof(btree_test));
}

void btree_test_teardown(btree_test* test) {
    if (test->btree) {
        cutil_btree_destroy(test->btree);
    }

    cutil_trait_destroy();
}

void btree_expect_test_setup(btree_expect_test* test) {
    memset(test, 0, sizeof(btree_expect_test));
}

void btree_expect_test_teardown(btree_expect_test* test) {
    if (test->actual_btree) {
        cutil_btree_destroy(test->actual_btree);
    }

    if (test->expected_btree) {
        cutil_btree_destroy(test->expected_btree);
    }

    cutil_trait_destroy();
}

void btree_get_test_setup(btree_get_test* test) {
    memset(test, 0 , sizeof(btree_get_test));
}

void btree_get_test_teardown(btree_get_test* test) {
    if (test->btree){
        cutil_btree_destroy(test->btree);
    }

    CTEST_UTIL_FREE_PTR_ARRAY(test->int_keys, test->count);
    CTEST_UTIL_FREE_PTR_ARRAY(test->int_values, test->count);

    cutil_trait_destroy();
}

void btree_trait_func_test_setup(btree_trait_func_test* test) {
    test->key_trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->val_trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());

    test->btree = cutil_btree_create(5, test->key_trait_tracker, test->val_trait_tracker);
}

void btree_trait_func_test_teardown(btree_trait_func_test* test) {
    if (test->btree) {
        cutil_btree_destroy(test->btree);
    }

    if (test->key_trait_tracker) {
        cutil_test_destroy_trait_tracker(test->key_trait_tracker);
    }

    if (test->val_trait_tracker) {
        cutil_test_destroy_trait_tracker(test->val_trait_tracker);
    }

    cutil_trait_destroy();
}
