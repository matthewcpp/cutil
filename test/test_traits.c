#include "test_suites.h"
#include "testing.h"
#include "trait.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

cutil_trait* g_trait = NULL;

void trait_int_before_each() {
	g_trait = cutil_trait_create_int();
}

void trait_after_each() {
	free(g_trait);
	g_trait = NULL;
}

void trait_int_compare() {
	int a1 = 1, a2 = 1, b = 2;

	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&a1, &b, g_trait->user_data), -1);
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&a1, &a2, g_trait->user_data), 0);
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&b, &a1, g_trait->user_data), 1);
}

void trait_int_copy() {
	int a = 5566;
	int b = 0;

	g_trait->copy_func(&b, &a, g_trait->user_data);

	CUTIL_TESTING_ASSERT_INT_EQ(a, b);
}

void trait_int_size() {
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->size, sizeof(int));
}

void trait_ptr_before_each() {
	g_trait = cutil_trait_create_ptr();
}

void trait_ptr_compare() {
	int* test = calloc(10, sizeof(int));

	int* beginning = test;
	int* end = test + 9;

	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&beginning, &end, g_trait->user_data), -1);
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&beginning, &beginning, g_trait->user_data), 0);
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&end, &beginning, g_trait->user_data), 1);

	free(test);
}

void trait_ptr_copy() {
	int* test = malloc(sizeof(int));
	*test = 55;

	int* expected = NULL;
	g_trait->copy_func(&expected, &test, g_trait->user_data);

	CUTIL_TESTING_ASSERT_INT_EQ(*test, *expected);
	CUTIL_TESTING_ASSERT_PTR_EQ(test, expected);

	// TODO: fix me.  if test fails, this will leak.
	free(test);
}

void trait_ptr_size() {
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->size, sizeof(void*));
}

void trait_cstring_before_each() {
	g_trait = cutil_trait_create_cstring();
}

void trait_cstring_size() {
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->size, sizeof(char**));
}

void trait_cstring_copy() {
	char* expected_string = "hello world!";
	char* actual_string = NULL;

	g_trait->copy_func(&actual_string, &expected_string, g_trait->user_data);

	CUTIL_TESTING_ASSERT_PTR_NOT_NULL(actual_string);
	CUTIL_TESTING_ASSERT_INT_EQ(strcmp(expected_string, actual_string), 0);

	// TODO: fix me.  if test fails, this will leak.
	free(actual_string);
}

void trait_cstring_compare() {
	char* str1 = "hello";
	char* str2 = "world";
	
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&str1, &str2, g_trait->user_data), -1);
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&str1, &str1, g_trait->user_data), 0);
	CUTIL_TESTING_ASSERT_INT_EQ(g_trait->compare_func(&str2, &str1, g_trait->user_data), 1);
}

void add_trait_tests() {
	cutil_testing_suite("trait_int");
	cutil_testing_suite_before_each(&trait_int_before_each);
	cutil_testing_suite_after_each(&trait_after_each);

	CUTIL_TESTING_ADD(trait_int_compare);
	CUTIL_TESTING_ADD(trait_int_copy);
	CUTIL_TESTING_ADD(trait_int_size);

	cutil_testing_suite("trait_ptr");
	cutil_testing_suite_before_each(&trait_ptr_before_each);
	cutil_testing_suite_after_each(&trait_after_each);

	CUTIL_TESTING_ADD(trait_ptr_compare);
	CUTIL_TESTING_ADD(trait_ptr_copy);
	CUTIL_TESTING_ADD(trait_ptr_size);

	cutil_testing_suite("trait_cstring");
	cutil_testing_suite_before_each(&trait_cstring_before_each);
	cutil_testing_suite_after_each(&trait_after_each);

	CUTIL_TESTING_ADD(trait_cstring_compare);
	CUTIL_TESTING_ADD(trait_cstring_copy);
	CUTIL_TESTING_ADD(trait_cstring_size);
	// TODO: Test cstring delete
}
