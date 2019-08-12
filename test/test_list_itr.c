#include "test_suites.h"

#include "cutil/list.h"
#include "ctest/ctest.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    cutil_list* list;
    cutil_list_itr* itr;
} list_itr_test;

void list_itr_test_setup(list_itr_test* test) {
    memset(test, 0, sizeof(list_itr_test));
}

void list_itr_test_teardown(list_itr_test* test) {
    if (test->itr) {
        cutil_list_itr_destroy(test->itr);
    }
    if (test->list) {
        cutil_list_destroy(test->list);
    }

    cutil_trait_destroy();
}

CTEST_FIXTURE(list_itr, list_itr_test, list_itr_test_setup, list_itr_test_teardown)


void has_next_empty(list_itr_test* test) {
    test->list = cutil_list_create(cutil_trait_int());
	test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_FALSE(cutil_list_itr_has_next(test->itr));
}

void has_prev_empty(list_itr_test* test) {
    test->list = cutil_list_create(cutil_trait_int());
    test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_FALSE(cutil_list_itr_has_prev(test->itr));
}

void next_empty(list_itr_test* test) {
    test->list = cutil_list_create(cutil_trait_int());
    test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_FALSE(cutil_list_itr_next(test->itr, NULL));
}

void prev_empty(list_itr_test* test) {
    test->list = cutil_list_create(cutil_trait_int());
    test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_FALSE(cutil_list_itr_prev(test->itr, NULL));
}

void has_next(list_itr_test* test) {
	int i = 55;

    test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &i);

    test->itr = cutil_list_itr_create(test->list);
	CTEST_ASSERT_TRUE(cutil_list_itr_has_next(test->itr));
}

void has_prev(list_itr_test* test) {
	int i = 55;

    test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &i);

	test->itr = cutil_list_itr_create(test->list);
	CTEST_ASSERT_TRUE(cutil_list_itr_has_prev(test->itr));
}

void next_one_item(list_itr_test* test) {
	int actual_val = 0;
	int expected_val = 55;

    test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &expected_val);

	test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_TRUE(cutil_list_itr_next(test->itr, &actual_val));
	CTEST_ASSERT_INT_EQ(expected_val, actual_val);
}

void prev_one_item(list_itr_test* test) {
	int actual_val = 0;
	int expected_val = 55;

    test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &expected_val);

	test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_TRUE(cutil_list_itr_prev(test->itr, &actual_val));
	CTEST_ASSERT_INT_EQ(expected_val, actual_val);
}

void next_null(list_itr_test* test) {
	int int_val = 55;

    test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &int_val);

	test->itr = cutil_list_itr_create(test->list);

	CTEST_ASSERT_TRUE(cutil_list_itr_next(test->itr, NULL));
}

void prev_null(list_itr_test* test) {
    int int_val = 55;

    test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &int_val);

	test->itr = cutil_list_itr_create(test->list );

	CTEST_ASSERT_TRUE(cutil_list_itr_prev(test->itr, NULL));
}

void iterate_list_next(list_itr_test* test) {
	int expected_sum = 0;
	int actual_sum = 0;
	int current_val = 0;
	int prev_val = -1;
	int i;

    test->list = cutil_list_create(cutil_trait_int());

	for (i = 0; i < 10; i++) {
		expected_sum += i;
		cutil_list_push_back(test->list, &i);
	}

	test->itr = cutil_list_itr_create(test->list);

	while (cutil_list_itr_next(test->itr, &current_val)) {
		actual_sum += current_val;
		
		CTEST_ASSERT_TRUE(current_val > prev_val);
		prev_val = current_val;
	}

	CTEST_ASSERT_INT_EQ(expected_sum, actual_sum);
}

void iterate_list_prev(list_itr_test* test) {
    int expected_sum = 0;
	int actual_sum = 0;
	int current_val = 0;
	int prev_val = 10;
	int i;

    test->list = cutil_list_create(cutil_trait_int());

	for (i = 0; i < 10; i++) {
		expected_sum += i;
		cutil_list_push_back(test->list, &i);
	}

	test->itr = cutil_list_itr_create(test->list);

	while (cutil_list_itr_prev(test->itr, &current_val)) {
		actual_sum += current_val;

		CTEST_ASSERT_TRUE(current_val < prev_val);
		prev_val = current_val;
	}

	CTEST_ASSERT_INT_EQ(expected_sum, actual_sum);
}

void iterate_list_next_ptr(list_itr_test* test) {
	int expected_sum = 0;
	int actual_sum = 0;
	int* current_ptr = NULL;
	int prev_val = -1;
	int i;
	int int_arr[10];

	test->list = cutil_list_create(cutil_trait_ptr());

	for (i = 0; i < 10; i++) {
		int* intptr = &int_arr[i];
		*intptr = i;
		cutil_list_push_back(test->list, &intptr);

		expected_sum += i;
	}

	test->itr = cutil_list_itr_create(test->list);

	while (cutil_list_itr_next(test->itr, &current_ptr)) {
		actual_sum += *current_ptr;

		CTEST_ASSERT_TRUE(*current_ptr > prev_val);
		prev_val = *current_ptr;
	}

	CTEST_ASSERT_INT_EQ(expected_sum, actual_sum);
}

void iterate_list_prev_ptr(list_itr_test* test) {
	int expected_sum = 0;
	int actual_sum = 0;
	int* current_ptr = NULL;
	int prev_val = 10;
	int i;
	int int_arr[10];

	test->list = cutil_list_create(cutil_trait_ptr());

	for (i = 0; i < 10; i++) {
		int* intptr = &int_arr[i];
		*intptr = i;

		cutil_list_push_back(test->list, &intptr);

		expected_sum += i;
	}

	test->itr = cutil_list_itr_create(test->list);

	while (cutil_list_itr_prev(test->itr, &current_ptr)) {
		actual_sum += *current_ptr;

		CTEST_ASSERT_TRUE(*current_ptr < prev_val);
		prev_val = *current_ptr;
	}

	CTEST_ASSERT_INT_EQ(expected_sum, actual_sum);
}

void forward_back(list_itr_test* test) {
	int i = 27;

	test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &i);

	test->itr = cutil_list_itr_create(test->list);
	CTEST_ASSERT_TRUE(cutil_list_itr_next(test->itr, NULL));
	CTEST_ASSERT_FALSE(cutil_list_itr_prev(test->itr, NULL));
}

void back_forward(list_itr_test* test) {
	int i = 27;

	test->list = cutil_list_create(cutil_trait_int());
	cutil_list_push_back(test->list, &i);

	test->itr = cutil_list_itr_create(test->list);
	CTEST_ASSERT_TRUE(cutil_list_itr_next(test->itr, NULL));
	CTEST_ASSERT_FALSE(cutil_list_itr_prev(test->itr, NULL));
}

void add_list_itr_tests() {
    CTEST_ADD_TEST_F(list_itr, has_next_empty);
    CTEST_ADD_TEST_F(list_itr, has_next);

    CTEST_ADD_TEST_F(list_itr, has_prev_empty);
    CTEST_ADD_TEST_F(list_itr, has_prev);

    CTEST_ADD_TEST_F(list_itr, next_empty);
    CTEST_ADD_TEST_F(list_itr, next_null);
    CTEST_ADD_TEST_F(list_itr, next_one_item);

    CTEST_ADD_TEST_F(list_itr, prev_empty);
    CTEST_ADD_TEST_F(list_itr, prev_one_item);
    CTEST_ADD_TEST_F(list_itr, prev_null);

    CTEST_ADD_TEST_F(list_itr, iterate_list_next);
	CTEST_ADD_TEST_F(list_itr, iterate_list_next_ptr);

    CTEST_ADD_TEST_F(list_itr, iterate_list_prev);
	CTEST_ADD_TEST_F(list_itr, iterate_list_prev_ptr);

    CTEST_ADD_TEST_F(list_itr, forward_back);
    CTEST_ADD_TEST_F(list_itr, back_forward);
}
