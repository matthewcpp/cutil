#include "test_suites.h"
#include "ctest/ctest.h"
#include "cutil/trait.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void trait_after_each() {
	cutil_trait_destroy();
}

void trait_int_compare() {
	cutil_trait* int_trait = cutil_trait_int();
	int a = -1, b1 = 1, b2 = 1, c = 2;

	CTEST_ASSERT_INT_LT(int_trait->compare_func(&a, &b1, int_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(int_trait->compare_func(&b1, &b2, int_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(int_trait->compare_func(&c, &a, int_trait->user_data), 0);
}

void trait_int_size() {
	cutil_trait* int_trait = cutil_trait_int();
	CTEST_ASSERT_INT_EQ(int_trait->size, sizeof(int));
}

void trait_uint_compare() {
	cutil_trait* uint_trait = cutil_trait_uint();
	unsigned int a1 = 1, a2 = 1, b = 2;

	CTEST_ASSERT_INT_LT(uint_trait->compare_func(&a1, &b, uint_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(uint_trait->compare_func(&a1, &a2, uint_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(uint_trait->compare_func(&b, &a1, uint_trait->user_data), 0);
}

void trait_float_size() {
	cutil_trait* float_trait = cutil_trait_float();
	CTEST_ASSERT_INT_EQ(float_trait->size, sizeof(float));
}

void trait_float_compare() {
	cutil_trait* float_trait = cutil_trait_int();
	float a = -1.0f, b1 = 1.0f, b2 = 1.0f, c = 2.0f;

	CTEST_ASSERT_INT_LT(float_trait->compare_func(&a, &b1, float_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(float_trait->compare_func(&b1, &b2, float_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(float_trait->compare_func(&c, &a, float_trait->user_data), 0);
}

void trait_uint_size() {
	cutil_trait* uint_trait = cutil_trait_uint();
	CTEST_ASSERT_INT_EQ(uint_trait->size, sizeof(unsigned int));
}

void trait_ptr_compare() {
	cutil_trait* ptr_trait = cutil_trait_ptr();
	int* test = calloc(10, sizeof(int));

	int* beginning = test;
	int* end = test + 9;

	CTEST_ASSERT_INT_LT(ptr_trait->compare_func(&beginning, &end, ptr_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(ptr_trait->compare_func(&beginning, &beginning, ptr_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(ptr_trait->compare_func(&end, &beginning, ptr_trait->user_data), 0);

	free(test);
}

void trait_ptr_size() {
	cutil_trait* ptr_trait = cutil_trait_ptr();
	CTEST_ASSERT_INT_EQ(ptr_trait->size, sizeof(void*));
}

void trait_cstring_size() {
	cutil_trait* cstring_trait = cutil_trait_cstring();
	CTEST_ASSERT_INT_EQ(cstring_trait->size, sizeof(char**));
}

void trait_cstring_copy() {
	cutil_trait* cstring_trait = cutil_trait_cstring();

	char* expected_string = "hello world!";
	char* str_ptr = NULL;
	cstring_trait->copy_func(&str_ptr, &expected_string, cstring_trait->user_data);

	CTEST_EXPECT_PTR_NOT_NULL(str_ptr);
	CTEST_EXPECT_INT_EQ(strcmp(expected_string, str_ptr), 0);

	free(str_ptr);
}

void trait_cstring_compare() {
	cutil_trait* cstring_trait = cutil_trait_cstring();
	char* str1 = "hello";
	char* str2 = "world";
	
	CTEST_ASSERT_INT_LT(cstring_trait->compare_func(&str1, &str2, cstring_trait->user_data), 0);
	CTEST_ASSERT_INT_EQ(cstring_trait->compare_func(&str1, &str1, cstring_trait->user_data), 0);
	CTEST_ASSERT_INT_GT(cstring_trait->compare_func(&str2, &str1, cstring_trait->user_data), 0);
}

void default_trait_int() {
	cutil_trait* int_trait1 = cutil_trait_int();
	cutil_trait* int_trait2 = cutil_trait_int();

	CTEST_ASSERT_PTR_EQ(int_trait1, int_trait2);
}

void default_trait_uint() {
	cutil_trait* uint_trait1 = cutil_trait_uint();
	cutil_trait* uint_trait2 = cutil_trait_uint();

	CTEST_ASSERT_PTR_EQ(uint_trait1, uint_trait2);
}

void default_trait_float() {
	cutil_trait* float_trait1 = cutil_trait_float();
	cutil_trait* float_trait2 = cutil_trait_float();

	CTEST_ASSERT_PTR_EQ(float_trait1, float_trait2);
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
    ctest_suite("traits");
    ctest_suite_after_each(&trait_after_each);

    CTEST_ADD_TEST(trait_int_compare);
    CTEST_ADD_TEST(trait_int_size);

	CTEST_ADD_TEST(trait_uint_compare);
	CTEST_ADD_TEST(trait_uint_size);

	CTEST_ADD_TEST(trait_float_compare);
	CTEST_ADD_TEST(trait_float_size);

    CTEST_ADD_TEST(trait_ptr_compare);
    CTEST_ADD_TEST(trait_ptr_size);

    CTEST_ADD_TEST(trait_cstring_compare);
    CTEST_ADD_TEST(trait_cstring_copy);
    CTEST_ADD_TEST(trait_cstring_size);
	/* TODO: Test cstring delete */

    CTEST_ADD_TEST(default_trait_int);
	CTEST_ADD_TEST(default_trait_uint);
	CTEST_ADD_TEST(default_trait_float);
    CTEST_ADD_TEST(default_trait_ptr);
    CTEST_ADD_TEST(default_trait_cstring);
}
