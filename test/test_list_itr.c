#include "test_suites.h"

#include "list.h"

#include "testing.h"

#include <stdlib.h>

void list_itr_empty_list_has_next() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	cutil_list_itr* it = cutil_list_itr_create(&list);

	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_has_next(it));

	cutil_list_itr_destroy(it);
	cutil_list_uninit(&list);
}

void list_itr_empty_list_next() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	cutil_list_itr* it = cutil_list_itr_create(&list);

	CUTIL_TESTING_ASSERT_FALSE(cutil_list_itr_next(it, NULL));

	cutil_list_itr_destroy(it);
	cutil_list_uninit(&list);
}

void list_itr_has_next() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int i = 55;
	cutil_list_push_back(&list, &i);

	cutil_list_itr* it = cutil_list_itr_create(&list);

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_has_next(it));

	cutil_list_itr_destroy(it);
	cutil_list_uninit(&list);
}

void list_itr_next() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int expected_val = 55;
	cutil_list_push_back(&list, &expected_val);

	cutil_list_itr* it = cutil_list_itr_create(&list);

	int actual_val = 0;

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_next(it, &actual_val));
	CUTIL_TESTING_ASSERT_INT_EQ(expected_val, actual_val);

	cutil_list_itr_destroy(it);
	cutil_list_uninit(&list);
}

void list_itr_next_null() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int int_val = 55;
	cutil_list_push_back(&list, &int_val);

	cutil_list_itr* it = cutil_list_itr_create(&list);

	CUTIL_TESTING_ASSERT_TRUE(cutil_list_itr_next(it, NULL));

	cutil_list_itr_destroy(it);
	cutil_list_uninit(&list);
}

void list_itr_iterate_list() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int expected_sum = 0;
	for (int i = 0; i < 10; i++) {
		expected_sum += i;
		cutil_list_push_back(&list, &i);
	}

	cutil_list_itr* it = cutil_list_itr_create(&list);

	int actual_sum = 0;
	int current_val = 0;
	while (cutil_list_itr_next(it, &current_val)) {
		actual_sum += current_val;
	}

	CUTIL_TESTING_ASSERT_INT_EQ(expected_sum, actual_sum);

	cutil_list_uninit(&list);
	cutil_list_itr_destroy(it);
}

void list_itr_iterate_list_ptr() {
	cutil_list list;
	cutil_list_initp(&list);

	int expected_sum = 0;
	for (int i = 0; i < 10; i++) {
		expected_sum += i;

		int* intptr = malloc(sizeof(int));
		*intptr = i;

		cutil_list_push_backp(&list, intptr);
	}

	cutil_list_itr* it = cutil_list_itr_create(&list);

	int actual_sum = 0;
	int *current_ptr = NULL;

	while (cutil_list_itr_nextp(it, &current_ptr)) {
		actual_sum += *current_ptr;
		free(current_ptr);
	}

	CUTIL_TESTING_ASSERT_INT_EQ(expected_sum, actual_sum);

	cutil_list_uninit(&list);
	cutil_list_itr_destroy(it);
}

void add_list_itr_tests() {
	cutil_testing_suite("list_itr");

	CUTIL_TESTING_ADD(list_itr_empty_list_has_next);
	CUTIL_TESTING_ADD(list_itr_empty_list_next);

	CUTIL_TESTING_ADD(list_itr_has_next);
	CUTIL_TESTING_ADD(list_itr_next);
	CUTIL_TESTING_ADD(list_itr_next_null);

	CUTIL_TESTING_ADD(list_itr_iterate_list);
	CUTIL_TESTING_ADD(list_itr_iterate_list_ptr);
}