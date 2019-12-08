#include "cutil/allocator.h"
#include "allocator_private.h"

#include <ctest/ctest.h>

#include <string.h>

void default_allocator_test_setup() {}

void default_allocator_test_teardown() {
	cutil_default_allocator_destroy();
}

CTEST_SUITE(default_allocator, default_allocator_test_setup, default_allocator_test_teardown)

typedef struct {
	cutil_allocator* custom_allocator;
} current_allocator_test_fixture;

void current_allocator_test_setup(current_allocator_test_fixture* test) {
	memset(test, 0, sizeof(current_allocator_test_fixture));
}

void current_allocator_test_teardown(current_allocator_test_fixture* test) {
	if (test->custom_allocator) {
		free(test->custom_allocator);
	}
}

CTEST_FIXTURE(current_allocator, current_allocator_test_fixture, current_allocator_test_setup, current_allocator_test_teardown)

/* tests that the same default allocator is returned from subsequent calls */
void return_same_allocator_multiple_calls() {
	cutil_allocator* allocator1 = cutil_default_allocator();
	cutil_allocator* allocator2 = cutil_default_allocator();

	CTEST_ASSERT_PTR_EQ(allocator1, allocator2);
}

void destroy_default_allocator() {
	cutil_default_allocator();
	cutil_default_allocator_destroy();

	CTEST_ASSERT_PTR_NULL(cutil_get_defult_allocator());
}

void destroy_default_clears_current_allocator() {
	cutil_default_allocator();
	CTEST_ASSERT_PTR_NOT_NULL(cutil_current_allocator());
	cutil_default_allocator_destroy();

	CTEST_ASSERT_PTR_NULL(cutil_get_current_allocator());
}

void calls_funcs() {
	cutil_allocator* default_allocator = cutil_default_allocator();
	void* malloc_data = default_allocator->malloc(5, default_allocator->user_data);
	void* calloc_data = default_allocator->calloc(1, 5, default_allocator->user_data);
	void* realloc_data = default_allocator->malloc(5, default_allocator->user_data);
	realloc_data = default_allocator->realloc(realloc_data, 10, default_allocator->user_data);

	default_allocator->free(malloc_data, default_allocator->user_data);
	default_allocator->free(calloc_data, default_allocator->user_data);
	default_allocator->free(realloc_data, default_allocator->user_data);
}

void creates_and_returns_default_allocator_if_none_set(current_allocator_test_fixture* test) {
	cutil_allocator* current_allocator = NULL;
	(void)test;

	CTEST_ASSERT_PTR_NULL(cutil_get_defult_allocator());
	current_allocator = cutil_current_allocator();
	CTEST_ASSERT_PTR_EQ(current_allocator, cutil_get_defult_allocator());
}

void sets_current_allocator(current_allocator_test_fixture* test) {
	test->custom_allocator = calloc(1, sizeof(cutil_allocator));
	cutil_set_current_allocator(test->custom_allocator);
	CTEST_EXPECT_PTR_EQ(test->custom_allocator, cutil_current_allocator());
}

void add_default_allocator_tests() {
	CTEST_ADD_TEST_S(default_allocator, return_same_allocator_multiple_calls)
	CTEST_ADD_TEST_S(default_allocator, destroy_default_allocator)
	CTEST_ADD_TEST_S(default_allocator, destroy_default_clears_current_allocator)
	CTEST_ADD_TEST_S(default_allocator, calls_funcs)

	CTEST_ADD_TEST_F(current_allocator, creates_and_returns_default_allocator_if_none_set);
	CTEST_ADD_TEST_F(current_allocator, sets_current_allocator);
}
