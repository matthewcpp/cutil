#include "test_suites.h"

#include "ctest/ctest.h"
#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include "cutil/trait.h"
#include "cutil/vector.h"

#include <stdlib.h>
#include <stdio.h>

cutil_vector* g_vector = NULL;
cutil_trait* g_vector_trait = NULL;

void vector_before_each() {
    g_vector = cutil_vector_create(cutil_trait_int());
}

void vector_after_each() {
    cutil_vector_destroy(g_vector);
	cutil_trait_destroy();

    g_vector = NULL;
}

void vectorp_before_each() {
	g_vector = cutil_vector_create(cutil_trait_ptr());
}

/* Initializing Vector sets size to 0 */
void vector_init_size_0() {
    CTEST_ASSERT_INT_EQ(cutil_vector_size(g_vector), 0);
}

/* getting vector trait returns correct pointer */
void vector_trait() {
	CTEST_ASSERT_PTR_EQ(cutil_vector_trait(g_vector), cutil_trait_int());
}

/* pushing an item to an empty vector grows it by one */
void vector_push_empty(){
    int i = 7;
    cutil_vector_push_back(g_vector, &i);

    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), 1);
    CTEST_ASSERT_INT_EQ(cutil_vector_size(g_vector), 1);
}

/* pushing a number of items to the vector will add them to the end */
void vector_push_multiple(){
    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(g_vector, &i);
    }

    CTEST_ASSERT_INT_EQ(cutil_vector_size(g_vector), 10);
    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), 16);
}

/* popping an empty vector does nothing */
void vector_pop_empty(){
    cutil_vector_pop_back(g_vector);
}

/* popping an item from the vector reduces its size */
void vector_pop_non_empty(){
    size_t previous_size;

    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(g_vector, &i);
    }

    previous_size = cutil_vector_size(g_vector);
    cutil_vector_pop_back(g_vector);
	CTEST_ASSERT_INT_EQ(cutil_vector_size(g_vector), previous_size - 1);
}

/* pop and add does not trigger array growth */
void vector_pop_and_add(){
    size_t expected_capacity;

    int i = 66;
    cutil_vector_push_back(g_vector, &i);

	expected_capacity = cutil_vector_capacity(g_vector);
    cutil_vector_pop_back(g_vector);
    cutil_vector_push_back(g_vector, &i);

    CTEST_ASSERT_INT_EQ(expected_capacity, cutil_vector_capacity(g_vector));
}

/* clearing an empty vector does nothing */
void vector_clear_empty(){
    cutil_vector_clear(g_vector);
}

/* clearing a vector resets size and leaves capacity unmodified */
void vector_clear(){
    int i;
	size_t expected_capacity;

    for (i = 0; i < 10; i++){
        cutil_vector_push_back(g_vector, &i);
    }

	expected_capacity = cutil_vector_capacity(g_vector);
    cutil_vector_clear(g_vector);

    CTEST_ASSERT_INT_EQ(cutil_vector_size(g_vector), 0);
    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), expected_capacity);
}

/* resetting a vector resets size and capacity */
void vector_reset() {
	int i;
	for (i = 0; i < 10; i++) {
		cutil_vector_push_back(g_vector, &i);
	}

	cutil_vector_reset(g_vector);

	CTEST_ASSERT_INT_EQ(cutil_vector_size(g_vector), 0);
	CTEST_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), 0);
}

/* get retrieves the correct items for valid indices */
void vector_get_valid() {
    int actual_value = -1;

    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(g_vector, &i);
    }

    for (i = 0; i < 10; i++){
        int get_result = cutil_vector_get(g_vector, i, &actual_value);

        CTEST_ASSERT_TRUE(get_result);
        CTEST_ASSERT_INT_EQ(actual_value, i);
    }
}

/* false is returned when trying to get value from empty vector */
void vector_get_empty() {
    int actual_value = -1;

    int get_result = cutil_vector_get(g_vector, 0, &actual_value);
    CTEST_ASSERT_FALSE(get_result);
}

/* false is returned when trying to get invalid indices */
void vector_get_invalid() {
    int actual_value = -1;
    int get_result;

    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(g_vector, &i);
    }

    get_result = cutil_vector_get(g_vector, 111, &actual_value);
    CTEST_ASSERT_FALSE(get_result);
}

/* popping and getting a value works as expected */
void vector_pop_get() {
    int int_val1 = 66;
    int int_val2 = 77;
    int actual_val = -1;
    int get_result;

    cutil_vector_push_back(g_vector, &int_val1);
    cutil_vector_pop_back(g_vector);
    cutil_vector_push_back(g_vector, &int_val2);

    get_result = cutil_vector_get(g_vector, 0, &actual_val);
    CTEST_ASSERT_TRUE(get_result);
    CTEST_ASSERT_INT_EQ(actual_val, int_val2);
}

/* pushing and getting a pointer work correctly */
void vectorp_push_and_get(){
    int expected_val = 999;
    int* actual_ptr;

    int* int_ptr = (int*)malloc(sizeof(int));
    *int_ptr = expected_val;

    cutil_vector_push_back(g_vector, &int_ptr);
    cutil_vector_get(g_vector, 0, &actual_ptr);

    CTEST_ASSERT_PTR_EQ(actual_ptr, int_ptr);
    CTEST_ASSERT_INT_EQ(expected_val, *actual_ptr);

    free(int_ptr);
}

void _vector_insert_test_strings(cutil_vector* vector, int count) {
    int i;
    char* buffer = malloc(20);

    for (i = 0; i < count; i++) {
		cutil_snprintf_func(buffer, 20, "str %d", i);

        cutil_vector_push_back(vector, &buffer);
    }

    free(buffer);
}

/* Tests that the vector calls the trait's copy function when pushing an item. */
void vector_trait_copy_on_push() {
    int expected_copy_count = 10;
    _vector_insert_test_strings(g_vector, expected_copy_count);

    CTEST_ASSERT_INT_EQ(expected_copy_count, cutil_test_trait_tracker_copy_count(g_vector_trait));
}

/* Tests that the trait's destroy function is called when popping data from the end of the vector. */
void vector_trait_destroy_on_pop_back() {
    int expected_destroy_count = 10;
    _vector_insert_test_strings(g_vector, expected_destroy_count);

    while (cutil_vector_size(g_vector) > 0) {
        cutil_vector_pop_back(g_vector);
    }

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(g_vector_trait));
}

void vector_trait_destroy_on_vec_destroy() {
    int expected_destroy_count = 10;
    _vector_insert_test_strings(g_vector, expected_destroy_count);

    cutil_vector_destroy(g_vector);
    g_vector = NULL;

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(g_vector_trait));
}

void vector_trait_destroy_on_vec_clear() {
    int expected_destroy_count = 10;
    _vector_insert_test_strings(g_vector, expected_destroy_count);

    cutil_vector_clear(g_vector);

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(g_vector_trait));
}

void vector_trait_destroy_on_vec_reset() {
    int expected_destroy_count = 10;
    _vector_insert_test_strings(g_vector, expected_destroy_count);

    cutil_vector_clear(g_vector);

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(g_vector_trait));
}

void vector_trait_before_each() {
	g_vector_trait = cutil_test_create_trait_tracker(cutil_trait_cstring());
	g_vector = cutil_vector_create(g_vector_trait);
}

void vector_trait_after_each() {
    if (g_vector) {
        cutil_vector_destroy(g_vector);
        g_vector = NULL;
    }

    if (g_vector_trait) {
        cutil_test_destroy_trait_tracker(g_vector_trait);
        g_vector_trait = NULL;
    }

    cutil_trait_destroy();

}

void add_vector_tests(){
    ctest_suite("vector");
    ctest_suite_before_each(&vector_before_each);
    ctest_suite_after_each(&vector_after_each);

    CTEST_ADD_TEST(vector_init_size_0);
    CTEST_ADD_TEST(vector_trait);

    CTEST_ADD_TEST(vector_push_empty);
    CTEST_ADD_TEST(vector_push_multiple);

    CTEST_ADD_TEST(vector_pop_empty);
    CTEST_ADD_TEST(vector_pop_non_empty);
    CTEST_ADD_TEST(vector_pop_and_add);
    CTEST_ADD_TEST(vector_pop_get);

    CTEST_ADD_TEST(vector_get_invalid);
    CTEST_ADD_TEST(vector_get_empty);
    CTEST_ADD_TEST(vector_get_valid);

    CTEST_ADD_TEST(vector_clear_empty);
    CTEST_ADD_TEST(vector_clear);
	CTEST_ADD_TEST(vector_reset);

    ctest_suite("vectorp");
    ctest_suite_before_each(&vectorp_before_each);
    ctest_suite_after_each(&vector_after_each);

    CTEST_ADD_TEST(vectorp_push_and_get);

	ctest_suite("vector-trait");
	ctest_suite_before_each(&vector_trait_before_each);
	ctest_suite_after_each(&vector_trait_after_each);

	CTEST_ADD_TEST(vector_trait_copy_on_push)
	CTEST_ADD_TEST(vector_trait_destroy_on_pop_back)
	CTEST_ADD_TEST(vector_trait_destroy_on_vec_destroy)
    CTEST_ADD_TEST(vector_trait_destroy_on_vec_reset)
    CTEST_ADD_TEST(vector_trait_destroy_on_vec_clear)
}
