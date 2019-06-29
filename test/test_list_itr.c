#include "test_suites.h"

#include "cutil/list.h"
#include "ctest/ctest.h"

#include <stdlib.h>

cutil_trait* g_itr_trait = NULL;
cutil_list* g_itr_list = NULL;

void list_itr_before_each() {
	g_itr_trait = cutil_trait_create_int();
	g_itr_list = cutil_list_create(g_itr_trait);
}

void list_itr_after_each() {
	cutil_list_destroy(g_itr_list);
	free(g_itr_trait);

	g_itr_list = NULL;
	g_itr_trait = NULL;
}

void list_itrp_before_each() {
	g_itr_trait = cutil_trait_create_ptr();
	g_itr_list = cutil_list_create(g_itr_trait);
}

void list_itr_empty_list_has_next() {
	cutil_list_itr* it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_has_next(it));

	cutil_list_itr_destroy(it);
}

void list_itr_empty_list_has_prev() {
	cutil_list_itr* it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_has_prev(it));

	cutil_list_itr_destroy(it);
}

void list_itr_empty_list_next() {
	cutil_list_itr* it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_next(it, NULL));

	cutil_list_itr_destroy(it);
}

void list_itr_empty_list_prev() {
	cutil_list_itr* it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_prev(it, NULL));

	cutil_list_itr_destroy(it);
}

void list_itr_has_next() {
	cutil_list_itr* it = NULL;

	int i = 55;
	cutil_list_push_back(g_itr_list, &i);

	it = cutil_list_itr_create(g_itr_list);
	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_has_next(it));

	cutil_list_itr_destroy(it);
}

void list_itr_has_prev() {
	cutil_list_itr* it = NULL;

	int i = 55;
	cutil_list_push_back(g_itr_list, &i);

	it = cutil_list_itr_create(g_itr_list);
	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_has_prev(it));

	cutil_list_itr_destroy(it);
}

void list_itr_next() {
	cutil_list_itr* it;
	int actual_val = 0;

	int expected_val = 55;
	cutil_list_push_back(g_itr_list, &expected_val);

	it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_next(it, &actual_val));
	CUTIL_TESTING_ASSERT_INT_EQ(expected_val, actual_val);

	cutil_list_itr_destroy(it);
}

void list_itr_prev() {
	cutil_list_itr* it = NULL;
	int actual_val = 0;

	int expected_val = 55;
	cutil_list_push_back(g_itr_list, &expected_val);

	it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_prev(it, &actual_val));
	CUTIL_TESTING_ASSERT_INT_EQ(expected_val, actual_val);

	cutil_list_itr_destroy(it);
}

void list_itr_next_null() {
	cutil_list_itr* it;

	int int_val = 55;
	cutil_list_push_back(g_itr_list, &int_val);

	it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_next(it, NULL));

	cutil_list_itr_destroy(it);
}

void list_itr_prev_null() {
	cutil_list_itr* it;

	int int_val = 55;
	cutil_list_push_back(g_itr_list, &int_val);

	it = cutil_list_itr_create(g_itr_list);

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_prev(it, NULL));

	cutil_list_itr_destroy(it);
}

void list_itr_iterate_list_next() {
	cutil_list_itr* it = NULL;
	int expected_sum = 0;
	int actual_sum = 0;
	int current_val = 0;
	int prev_val = -1;

	int i;
	for (i = 0; i < 10; i++) {
		expected_sum += i;
		cutil_list_push_back(g_itr_list, &i);
	}

	it = cutil_list_itr_create(g_itr_list);

	while (cutil_list_itr_next(it, &current_val)) {
		actual_sum += current_val;
		
		CUTIL_TESTING_ASSERT_TRUE(current_val > prev_val);
		prev_val = current_val;
	}

	CUTIL_TESTING_ASSERT_INT_EQ(expected_sum, actual_sum);

	cutil_list_itr_destroy(it);
}

void list_itr_iterate_list_prev() {
	cutil_list_itr* it = NULL;

	int expected_sum = 0;
	int actual_sum = 0;
	int current_val = 0;
	int prev_val = 10;

	int i;
	for (i = 0; i < 10; i++) {
		expected_sum += i;
		cutil_list_push_back(g_itr_list, &i);
	}

	it = cutil_list_itr_create(g_itr_list);

	while (cutil_list_itr_prev(it, &current_val)) {
		actual_sum += current_val;

		CUTIL_TESTING_ASSERT_TRUE(current_val < prev_val);
		prev_val = current_val;
	}

	CUTIL_TESTING_ASSERT_INT_EQ(expected_sum, actual_sum);

	cutil_list_itr_destroy(it);
}

void list_itrp_iterate_list_ptr_next() {
	cutil_list_itr* it = NULL;

	int expected_sum = 0;
	int actual_sum = 0;
	int* current_ptr = NULL;
	int prev_val = -1;

	int i;
	for (i = 0; i < 10; i++) {
		int* intptr = malloc(sizeof(int));
		*intptr = i;
		cutil_list_push_back(g_itr_list, &intptr);

		expected_sum += i;
	}

	it = cutil_list_itr_create(g_itr_list);

	while (cutil_list_itr_next(it, &current_ptr)) {
		actual_sum += *current_ptr;

		CUTIL_TESTING_ASSERT_TRUE(*current_ptr > prev_val);
		prev_val = *current_ptr;

		free(current_ptr);
	}

	CUTIL_TESTING_ASSERT_INT_EQ(expected_sum, actual_sum);

	cutil_list_itr_destroy(it);
}

void list_itrp_iterate_list_ptr_prev() {
	cutil_list_itr* it = NULL;

	int expected_sum = 0;
	int actual_sum = 0;
	int* current_ptr = NULL;
	int prev_val = 10;

	int i;
	for (i = 0; i < 10; i++) {
		int* intptr = malloc(sizeof(int));
		*intptr = i;

		cutil_list_push_back(g_itr_list, &intptr);

		expected_sum += i;
	}

	it = cutil_list_itr_create(g_itr_list);

	while (cutil_list_itr_prev(it, &current_ptr)) {
		actual_sum += *current_ptr;

		CUTIL_TESTING_ASSERT_TRUE(*current_ptr < prev_val);
		prev_val = *current_ptr;

		free(current_ptr);
	}

	CUTIL_TESTING_ASSERT_INT_EQ(expected_sum, actual_sum);

	cutil_list_itr_destroy(it);
}

void list_itr_forward_back() {
    cutil_list_itr* it = NULL;
	int i = 27;
	cutil_list_push_back(g_itr_list, &i);

	it = cutil_list_itr_create(g_itr_list);
	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_next(it, NULL));
	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_prev(it, NULL));

	cutil_list_itr_destroy(it);
}

void list_itr_back_forward() {
    cutil_list_itr* it = NULL;
	int i = 27;
	cutil_list_push_back(g_itr_list, &i);

	it = cutil_list_itr_create(g_itr_list);
	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_next(it, NULL));
	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_prev(it, NULL));

	cutil_list_itr_destroy(it);
}

void add_list_itr_tests() {
	cutil_testing_suite("list_itr");
	cutil_testing_suite_before_each(&list_itr_before_each);
	cutil_testing_suite_after_each(&list_itr_after_each);

	CUTIL_TESTING_ADD(list_itr_empty_list_has_next);
	CUTIL_TESTING_ADD(list_itr_empty_list_has_prev);

	CUTIL_TESTING_ADD(list_itr_empty_list_next);
	CUTIL_TESTING_ADD(list_itr_empty_list_prev);

	CUTIL_TESTING_ADD(list_itr_has_next);
	CUTIL_TESTING_ADD(list_itr_has_prev);

	CUTIL_TESTING_ADD(list_itr_next);
	CUTIL_TESTING_ADD(list_itr_prev);

	CUTIL_TESTING_ADD(list_itr_next_null);
	CUTIL_TESTING_ADD(list_itr_prev_null);

	CUTIL_TESTING_ADD(list_itr_iterate_list_next);
	CUTIL_TESTING_ADD(list_itr_iterate_list_prev);

	CUTIL_TESTING_ADD(list_itr_forward_back);
	CUTIL_TESTING_ADD(list_itr_back_forward);

	cutil_testing_suite("list_itrp");
	cutil_testing_suite_before_each(&list_itrp_before_each);
	cutil_testing_suite_after_each(&list_itr_after_each);

	CUTIL_TESTING_ADD(list_itrp_iterate_list_ptr_next);
	CUTIL_TESTING_ADD(list_itrp_iterate_list_ptr_prev);
}
