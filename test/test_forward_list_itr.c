#include "cutil/forward_list.h"

#include "ctest/ctest.h"

#include <string.h>

typedef struct {
    cutil_forward_list* forward_list;
    cutil_forward_list_itr* itr;
} forward_list_itr_test;

void forward_list_itr_test_setup(forward_list_itr_test* test) {
    memset(test, 0, sizeof(forward_list_itr_test));
}

void forward_list_itr_test_teardown(forward_list_itr_test* test) {
    if (test->itr) {
        cutil_forward_list_itr_destroy(test->itr);
    }
    if (test->forward_list) {
        cutil_forward_list_destroy(test->forward_list);
    }

    cutil_trait_destroy();
}

CTEST_FIXTURE(forward_list_itr, forward_list_itr_test, forward_list_itr_test_setup, forward_list_itr_test_teardown)


void has_next_empty_forward_list(forward_list_itr_test* test) {
    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    test->itr = cutil_forward_list_itr_create(test->forward_list);

    CTEST_ASSERT_FALSE(cutil_forward_list_itr_has_next(test->itr));
}

void has_next_forward_list(forward_list_itr_test* test) {
    int i = 55;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    cutil_forward_list_push_front(test->forward_list, &i);

    test->itr = cutil_forward_list_itr_create(test->forward_list);
    CTEST_ASSERT_TRUE(cutil_forward_list_itr_has_next(test->itr));
}

void next_null_forward_list(forward_list_itr_test* test) {
    int int_val = 55;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    cutil_forward_list_push_front(test->forward_list, &int_val);

    test->itr = cutil_forward_list_itr_create(test->forward_list);

    CTEST_ASSERT_TRUE(cutil_forward_list_itr_next(test->itr, NULL));
}

void next_one_item_forward_list(forward_list_itr_test* test) {
    int actual_val = 0;
    int expected_val = 55;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    cutil_forward_list_push_front(test->forward_list, &expected_val);

    test->itr = cutil_forward_list_itr_create(test->forward_list);

    CTEST_ASSERT_TRUE(cutil_forward_list_itr_next(test->itr, &actual_val));
    CTEST_ASSERT_INT_EQ(expected_val, actual_val);

    CTEST_ASSERT_FALSE(cutil_forward_list_itr_next(test->itr, &actual_val));
}

void iterate_forward_list_next(forward_list_itr_test* test) {
    int i = 0, item_count = 10;
    int current_val = 0, prev_val = item_count;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());

    for (i = 0; i < item_count; i++) {
        cutil_forward_list_push_front(test->forward_list, &i);
    }

    test->itr = cutil_forward_list_itr_create(test->forward_list);

    i = 0;
    while (cutil_forward_list_itr_next(test->itr, &current_val)) {
        CTEST_ASSERT_TRUE(current_val == prev_val - 1);
        prev_val = current_val;
        i += 1;
    }

    CTEST_ASSERT_INT_EQ(i, item_count);
}

void add_forward_list_itr_tests() {
    CTEST_ADD_TEST_F(forward_list_itr, has_next_empty_forward_list);
    CTEST_ADD_TEST_F(forward_list_itr, has_next_forward_list);

    CTEST_ADD_TEST_F(forward_list_itr, next_one_item_forward_list);
    CTEST_ADD_TEST_F(forward_list_itr, next_null_forward_list);

    CTEST_ADD_TEST_F(forward_list_itr, iterate_forward_list_next);
}
