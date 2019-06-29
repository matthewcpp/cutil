#include "test_suites.h"
#include "ctest/ctest.h"
#include "cutil/trait.h"

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

	CTEST_ASSERT_INT_LT(g_trait->compare_func(&a1, &b, g_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(g_trait->compare_func(&a1, &a2, g_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(g_trait->compare_func(&b, &a1, g_trait->user_data), 0);
}

void trait_int_size() {
	CTEST_ASSERT_INT_EQ(g_trait->size, sizeof(int));
}

void trait_ptr_before_each() {
	g_trait = cutil_trait_create_ptr();
}

void trait_ptr_compare() {
	int* test = calloc(10, sizeof(int));

	int* beginning = test;
	int* end = test + 9;

	CTEST_ASSERT_INT_LT(g_trait->compare_func(&beginning, &end, g_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(g_trait->compare_func(&beginning, &beginning, g_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(g_trait->compare_func(&end, &beginning, g_trait->user_data), 0);

	free(test);
}

void trait_ptr_size() {
	CTEST_ASSERT_INT_EQ(g_trait->size, sizeof(void*));
}

void trait_cstring_before_each() {
	g_trait = cutil_trait_create_cstring();
}

void trait_cstring_size() {
	CTEST_ASSERT_INT_EQ(g_trait->size, sizeof(char**));
}

void trait_cstring_copy() {
	char* expected_string = "hello world!";
	char* str_ptr = NULL;
	g_trait->copy_func(&str_ptr, &expected_string, g_trait->user_data);

	CTEST_EXPECT_PTR_NOT_NULL(str_ptr);
	CTEST_EXPECT_INT_EQ(strcmp(expected_string, str_ptr), 0);

	free(str_ptr);
}

void trait_cstring_compare() {
	char* str1 = "hello";
	char* str2 = "world";
	
	CTEST_ASSERT_INT_LT(g_trait->compare_func(&str1, &str2, g_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(g_trait->compare_func(&str1, &str1, g_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(g_trait->compare_func(&str2, &str1, g_trait->user_data), 0);
}

void default_trait_int() {
	cutil_trait* int_trait1 = cutil_trait_int();
	cutil_trait* int_trait2 = cutil_trait_int();

	CTEST_ASSERT_PTR_EQ(int_trait1, int_trait2);
}

void default_trait_ptr() {
	cutil_trait* ptr_trait1 = cutil_trait_ptr();
	cutil_trait* ptr_trait2 = cutil_trait_ptr();

	CTEST_ASSERT_PTR_EQ(ptr_trait1, ptr_trait2);
}

void default_trait_cstring() {
	cutil_trait* cstring_trait1 = cutil_trait_cstring();
	cutil_trait* cstring_trait2 = cutil_trait_cstring();

	CTEST_ASSERT_PTR_EQ(cstring_trait1, cstring_trait2);
}

void default_trait_after_each() {
	cutil_trait_destroy();
}

void add_trait_tests() {
    ctest_suite("trait_int");
    ctest_suite_before_each(&trait_int_before_each);
    ctest_suite_after_each(&trait_after_each);

    CTEST_ADD_TEST(trait_int_compare);
    CTEST_ADD_TEST(trait_int_size);

    ctest_suite("trait_ptr");
    ctest_suite_before_each(&trait_ptr_before_each);
    ctest_suite_after_each(&trait_after_each);

    CTEST_ADD_TEST(trait_ptr_compare);
    CTEST_ADD_TEST(trait_ptr_size);

    ctest_suite("trait_cstring");
    ctest_suite_before_each(&trait_cstring_before_each);
    ctest_suite_after_each(&trait_after_each);

    CTEST_ADD_TEST(trait_cstring_compare);
    CTEST_ADD_TEST(trait_cstring_copy);
    CTEST_ADD_TEST(trait_cstring_size);
	/* TODO: Test cstring delete */

    ctest_suite("default_traits");
    CTEST_ADD_TEST(default_trait_int);
    CTEST_ADD_TEST(default_trait_ptr);
    CTEST_ADD_TEST(default_trait_cstring);
    ctest_suite_after_each(&default_trait_after_each);
}
