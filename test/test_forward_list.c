#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include "cutil/forward_list.h"
#include "ctest/ctest.h"

#include <string.h>

typedef struct {
    cutil_forward_list* forward_list;
    cutil_forward_list_itr* itr;
} forward_list_test;

void forward_list_test_setup(forward_list_test* test) {
    memset(test, 0, sizeof(forward_list_test));
}

void forward_list_test_teardown(forward_list_test* test) {
    if (test->forward_list) {
        cutil_forward_list_destroy(test->forward_list);
    }

    if (test->itr) {
        cutil_forward_list_itr_destroy(test->itr);
    }

    cutil_trait_destroy();
}

CTEST_FIXTURE(forward_list, forward_list_test, forward_list_test_setup, forward_list_test_teardown)

/* Clearing an empty list does nothing */
void clear_empty_forward_list(forward_list_test* test) {
    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    cutil_forward_list_clear(test->forward_list);
}

/* creating new list is empty */
void empty_forward_list(forward_list_test* test) {
    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    CTEST_ASSERT_TRUE(cutil_forward_list_empty(test->forward_list));
}

void non_empty_forward_list(forward_list_test* test) {
    int i = 55;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    cutil_forward_list_push_front(test->forward_list, &i);

    CTEST_ASSERT_FALSE(cutil_forward_list_empty(test->forward_list));
}

/* Pushing items to the front appends them to the list */
void push_front_multiple_forward_list(forward_list_test* test) {
    int actual_value;
    int i, item_count = 5;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());

    for (i = 0; i < item_count; i++) {
        int inserted_value = i * 10;
        cutil_forward_list_push_front(test->forward_list, &inserted_value);
    }

    test->itr = cutil_forward_list_itr_create(test->forward_list);

    for (i = item_count - 1; i >= 0; i--) {
        int expected_value = i * 10;
        CTEST_ASSERT_TRUE(cutil_forward_list_itr_next(test->itr, &actual_value));
        CTEST_ASSERT_INT_EQ(expected_value, actual_value);
    }
}

/* Getting the front item of an empty list returns false */
void front_empty_forward_list(forward_list_test* test) {
    int front_val = 0;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());

    CTEST_ASSERT_FALSE(cutil_forward_list_front(test->forward_list, &front_val));
}

/* Pushing data to the front and retrieving it functions correctly */
void front_pod_forward_list(forward_list_test* test) {
    int expected_value = 3, actual_value = 0;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());

    cutil_forward_list_push_front(test->forward_list, &expected_value);
    CTEST_ASSERT_TRUE(cutil_forward_list_front(test->forward_list, &actual_value));

    CTEST_ASSERT_INT_EQ(expected_value, actual_value);
}

/* Pushing a pointer to the front and retrieving it functions correctly */
void front_pointer_forward_list(forward_list_test* test) {
    int array[2] = {0,1};
    int* expected_ptr = &array[0];
    int* actual_ptr = NULL;

    test->forward_list = cutil_forward_list_create(cutil_trait_ptr());
    cutil_forward_list_push_front(test->forward_list, &expected_ptr);

    CTEST_ASSERT_TRUE(cutil_forward_list_front(test->forward_list, &actual_ptr));
    CTEST_ASSERT_PTR_EQ(expected_ptr, actual_ptr);
}

/* Popping the front of an empty list does nothing */
void pop_front_empty_forward_list(forward_list_test* test) {
    test->forward_list = cutil_forward_list_create(cutil_trait_int());

    CTEST_ASSERT_FALSE(cutil_forward_list_pop_front(test->forward_list));
}

/* Popping the only item from the back correctly sets the list to empty */
void pop_front_one_item_forward_list(forward_list_test* test) {
    int value = 55;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());

    cutil_forward_list_push_front(test->forward_list, &value);
    cutil_forward_list_pop_front(test->forward_list);

    CTEST_ASSERT_TRUE(cutil_forward_list_empty(test->forward_list));
}

void pop_front_removes_items_forward_list(forward_list_test* test) {
    int i, item_count = 5;

    test->forward_list = cutil_forward_list_create(cutil_trait_int());
    for (i = 0; i < item_count; i++) {
        cutil_forward_list_push_front(test->forward_list, &i);
    }

    for (i = item_count - 1; i >=0; i--) {
        int actual_value = -1;
        CTEST_ASSERT_TRUE(cutil_forward_list_front(test->forward_list, &actual_value));
        CTEST_ASSERT_TRUE(cutil_forward_list_pop_front(test->forward_list));
        CTEST_ASSERT_INT_EQ(i, actual_value);
    }

    CTEST_ASSERT_TRUE(cutil_forward_list_empty(test->forward_list));
}

void add_forward_list_tests() {
    CTEST_ADD_TEST_F(forward_list, clear_empty_forward_list);

    CTEST_ADD_TEST_F(forward_list, empty_forward_list);
    CTEST_ADD_TEST_F(forward_list, non_empty_forward_list);

    CTEST_ADD_TEST_F(forward_list, push_front_multiple_forward_list);

    CTEST_ADD_TEST_F(forward_list, front_empty_forward_list);
    CTEST_ADD_TEST_F(forward_list, front_pod_forward_list);
    CTEST_ADD_TEST_F(forward_list, front_pointer_forward_list);

    CTEST_ADD_TEST_F(forward_list, pop_front_empty_forward_list);
    CTEST_ADD_TEST_F(forward_list, pop_front_one_item_forward_list);
    CTEST_ADD_TEST_F(forward_list, pop_front_removes_items_forward_list);
}
