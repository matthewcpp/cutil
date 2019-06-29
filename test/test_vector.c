#include "test_suites.h"

#include "ctest/testing.h"

#include "cutil/trait.h"
#include "cutil/vector.h"

#include <stdlib.h>

cutil_trait* g_vector_trait = NULL;
cutil_vector* g_vector = NULL;

void vector_before_each() {
	g_vector_trait = cutil_trait_create_int();
    g_vector = cutil_vector_create(g_vector_trait);
}

void vector_after_each() {
    cutil_vector_destroy(g_vector);
	free(g_vector_trait);

    g_vector = NULL;
	g_vector_trait = NULL;
}

void vectorp_before_each() {
	g_vector_trait = cutil_trait_create_ptr();
	g_vector = cutil_vector_create(g_vector_trait);
}

/* Initializing Vector sets size to 0 */
void vector_init_size_0() {
    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_size(g_vector), 0);
}

/* getting vector trait returns correct pointer */
void vector_trait() {
	CUTIL_TESTING_ASSERT_PTR_EQ(cutil_vector_trait(g_vector), g_vector_trait);
}

/* pushing an item to an empty vector grows it by one */
void vector_push_empty(){
    int i = 7;
    cutil_vector_push(g_vector, &i);

    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), 1);
    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_size(g_vector), 1);
}

/* pushing a number of items to the vector will add them to the end */
void vector_push_multiple(){
    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push(g_vector, &i);
    }

    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_size(g_vector), 10);
    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), 16);
}

/* popping an empty vector does nothing */
void vector_pop_empty(){
    cutil_vector_pop(g_vector);
}

/* popping an item from the vector reduces its size */
void vector_pop_non_empty(){
    size_t previous_size;

    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push(g_vector, &i);
    }

    previous_size = cutil_vector_size(g_vector);
    cutil_vector_pop(g_vector);
	CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_size(g_vector), previous_size - 1);
}

/* pop and add does not trigger array growth */
void vector_pop_and_add(){
    size_t expected_capacity;

    int i = 66;
    cutil_vector_push(g_vector, &i);

	expected_capacity = cutil_vector_capacity(g_vector);
    cutil_vector_pop(g_vector);
    cutil_vector_push(g_vector, &i);

    CUTIL_TESTING_ASSERT_INT_EQ(expected_capacity, cutil_vector_capacity(g_vector));
}

/* clearing an empty vector does nothing */
void vector_clear_empty(){
    cutil_vector_clear(g_vector);
}

/* clearing a vector resets size and capacity */
void vector_clear_reset(){
    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push(g_vector, &i);
    }

    cutil_vector_clear(g_vector);

    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_size(g_vector), 0);
    CUTIL_TESTING_ASSERT_INT_EQ(cutil_vector_capacity(g_vector), 0);
}

/* get retrieves the correct items for valid indices */
void vector_get_valid() {
    int actual_value = -1;

    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push(g_vector, &i);
    }

    for (i = 0; i < 10; i++){
        int get_result = cutil_vector_get(g_vector, i, &actual_value);

        CUTIL_TESTING_ASSERT_TRUE(get_result);
        CUTIL_TESTING_ASSERT_INT_EQ(actual_value, i);
    }
}

/* false is returned when trying to get value from empty vector */
void vector_get_empty() {
    int actual_value = -1;

    int get_result = cutil_vector_get(g_vector, 0, &actual_value);
    CUTIL_TESTING_ASSERT_FALSE(get_result);
}

/* false is returned when trying to get invalid indices */
void vector_get_invalid() {
    int actual_value = -1;
    int get_result;

    int i;
    for (i = 0; i < 10; i++){
        cutil_vector_push(g_vector, &i);
    }

    get_result = cutil_vector_get(g_vector, 111, &actual_value);
    CUTIL_TESTING_ASSERT_FALSE(get_result);
}

/* popping and getting a value works as expected */
void vector_pop_get() {
    int int_val1 = 66;
    int int_val2 = 77;
    int actual_val = -1;
    int get_result;

    cutil_vector_push(g_vector, &int_val1);
    cutil_vector_pop(g_vector);
    cutil_vector_push(g_vector, &int_val2);

    get_result = cutil_vector_get(g_vector, 0, &actual_val);
    CUTIL_TESTING_ASSERT_TRUE(get_result);
    CUTIL_TESTING_ASSERT_INT_EQ(actual_val, int_val2);
}

/* pushing and getting a pointer work correctly */
void vectorp_push_and_get(){
    int expected_val = 999;
    int* actual_ptr;

    int* int_ptr = (int*)malloc(sizeof(int));
    *int_ptr = expected_val;

    cutil_vector_push(g_vector, &int_ptr);
    cutil_vector_get(g_vector, 0, &actual_ptr);

    CUTIL_TESTING_ASSERT_PTR_EQ(actual_ptr, int_ptr);
    CUTIL_TESTING_ASSERT_INT_EQ(expected_val, *actual_ptr);

    free(int_ptr);
}

void add_vector_tests(){
    cutil_testing_suite("vector");
    cutil_testing_suite_before_each(&vector_before_each);
    cutil_testing_suite_after_each(&vector_after_each);

	CUTIL_TESTING_ADD(vector_init_size_0);
	CUTIL_TESTING_ADD(vector_trait);

    CUTIL_TESTING_ADD(vector_push_empty);
    CUTIL_TESTING_ADD(vector_push_multiple);

    CUTIL_TESTING_ADD(vector_pop_empty);
    CUTIL_TESTING_ADD(vector_pop_non_empty);
    CUTIL_TESTING_ADD(vector_pop_and_add);
    CUTIL_TESTING_ADD(vector_pop_get);

    CUTIL_TESTING_ADD(vector_get_invalid);
    CUTIL_TESTING_ADD(vector_get_empty);
    CUTIL_TESTING_ADD(vector_get_valid);

    CUTIL_TESTING_ADD(vector_clear_empty);
    CUTIL_TESTING_ADD(vector_clear_reset);

    cutil_testing_suite("vectorp");
    cutil_testing_suite_before_each(&vectorp_before_each);
    cutil_testing_suite_after_each(&vector_after_each);

    CUTIL_TESTING_ADD(vectorp_push_and_get);
}
