#include "test_suites.h"

#include "ctest/ctest.h"
#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include "cutil/trait.h"
#include "cutil/vector.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    cutil_vector* vector;
} vector_test;

void vector_test_setup(vector_test* test) {
    test->vector = NULL;
}
void vector_test_teardown(vector_test* test) {
    if (test->vector) {
        cutil_vector_destroy(test->vector);
    }

    cutil_trait_destroy();
}

typedef struct {
    cutil_vector* vector;
    cutil_trait* trait_tracker;
} vector_trait_func_test;

void vector_trait_func_test_setup(vector_trait_func_test* test) {
    memset(test, 0, sizeof(vector_trait_func_test));
}

void vector_trait_func_test_teardown(vector_trait_func_test* test) {
    if (test->vector) {
        cutil_vector_destroy(test->vector);
    }

    if (test->trait_tracker) {
        cutil_test_destroy_trait_tracker(test->trait_tracker);
    }

    cutil_trait_destroy();
}

CTEST_FIXTURE(vector, vector_test, vector_test_setup, vector_test_teardown)
CTEST_FIXTURE(vector_trait_func, vector_trait_func_test, vector_trait_func_test_setup, vector_trait_func_test_teardown)

/* Initializing Vector sets size to 0 */
void init_size_0_vec(vector_test* test) {
    test->vector = cutil_vector_create(cutil_trait_int());

    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), 0);
}

/* getting vector trait returns correct pointer */
void get_vector_trait(vector_test* test) {
    test->vector = cutil_vector_create(cutil_trait_int());

    CTEST_ASSERT_PTR_EQ(cutil_vector_trait(test->vector), cutil_trait_int());
}

/* pushing an item to an empty vector grows it by one */
void push_back_empty(vector_test* test){
    int i = 7;

    test->vector = cutil_vector_create(cutil_trait_int());
    cutil_vector_push_back(test->vector, &i);

    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(test->vector), 1);
    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), 1);
}

/* pushing a number of items to the vector will add them to the end */
void push_back_multiple_vec(vector_test* test){
    int i;

    test->vector = cutil_vector_create(cutil_trait_int());

    for (i = 0; i < 10; i++){
        cutil_vector_push_back(test->vector, &i);
    }

    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), 10);
    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(test->vector), 16);
}

/* popping an empty vector does nothing */
void pop_back_empty_vec(vector_test* test){
    test->vector = cutil_vector_create(cutil_trait_int());

    cutil_vector_pop_back(test->vector);
}

/* popping an item from the vector reduces its size */
void pop_back_non_empty(vector_test* test){
    size_t previous_size;
    int i;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(test->vector, &i);
    }

    previous_size = cutil_vector_size(test->vector);
    cutil_vector_pop_back(test->vector);
    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), previous_size - 1);
}

/* pop and add does not trigger array growth */
void pop_back_and_add(vector_test* test){
    size_t expected_capacity;
    int i = 66;

    test->vector = cutil_vector_create(cutil_trait_int());
    cutil_vector_push_back(test->vector, &i);

    expected_capacity = cutil_vector_capacity(test->vector);
    cutil_vector_pop_back(test->vector);
    cutil_vector_push_back(test->vector, &i);

    CTEST_ASSERT_INT_EQ(expected_capacity, cutil_vector_capacity(test->vector));
}

/* clearing an empty vector does nothing */
void clear_empty_vec(vector_test* test){
    test->vector = cutil_vector_create(cutil_trait_int());
    cutil_vector_clear(test->vector);
}

/* clearing a vector resets size and leaves capacity unmodified */
void clear(vector_test* test){
    int i;
    size_t expected_capacity;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(test->vector, &i);
    }

    expected_capacity = cutil_vector_capacity(test->vector);
    cutil_vector_clear(test->vector);

    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), 0);
    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(test->vector), expected_capacity);
}

/* resetting a vector resets size and capacity */
void reset(vector_test* test) {
    int i;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    cutil_vector_reset(test->vector);

    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), 0);
    CTEST_ASSERT_INT_EQ(cutil_vector_capacity(test->vector), 0);
}

/* get retrieves the correct items for valid indices */
void get_valid(vector_test* test) {
    int actual_value = -1;
    int i;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(test->vector, &i);
    }

    for (i = 0; i < 10; i++){
        int get_result = cutil_vector_get(test->vector, i, &actual_value);

        CTEST_ASSERT_TRUE(get_result);
        CTEST_ASSERT_INT_EQ(actual_value, i);
    }
}

/* updates the value for a valid index */
void set_valid(vector_test* test) {
    int item_count = 10;
    int i, actual_value, expected_value, vector_set_result;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < item_count; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    for (i = 0; i < item_count; i++) {
        actual_value = i * 10;
        vector_set_result = cutil_vector_set(test->vector, i, &actual_value);
        CTEST_ASSERT_TRUE(vector_set_result);
    }

    for (i = 0; i < item_count; i++) {
        expected_value = i * 10;
        cutil_vector_get(test->vector, i, &actual_value);

        CTEST_ASSERT_INT_EQ(expected_value, actual_value);
    }
}

/* returns false if index is invalid */
void set_invalid(vector_test* test) {
    int item_count = 10;
    int set_result = 1;
    int i;

    test->vector = cutil_vector_create(cutil_trait_int());

    for (i = 0; i < item_count; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    set_result = cutil_vector_set(test->vector, item_count + 1, &i);
    CTEST_ASSERT_FALSE(set_result);
}

/* false is returned when trying to get value from empty vector */
void get_empty(vector_test* test) {
    int actual_value = -1;
    int get_result = -1;

    test->vector = cutil_vector_create(cutil_trait_int());
    get_result = cutil_vector_get(test->vector, 0, &actual_value);

    CTEST_ASSERT_FALSE(get_result);
}

/* false is returned when trying to get invalid indices */
void get_invalid(vector_test* test) {
    int actual_value = -1;
    int get_result;
    int i;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++){
        cutil_vector_push_back(test->vector, &i);
    }

    get_result = cutil_vector_get(test->vector, 111, &actual_value);
    CTEST_ASSERT_FALSE(get_result);
}

/* Tests that items are updated correctly when inserted in middle of vector */
void insert_middle(vector_test* test) {
    int result;
    int test_index = 5, item_count = 10;
    int actual_val = 50, expected_val;
    size_t i, actual_vector_size;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    result = cutil_vector_insert(test->vector, test_index, &actual_val);
    CTEST_ASSERT_TRUE(result);

    actual_vector_size = cutil_vector_size(test->vector);
    CTEST_ASSERT_INT_EQ(actual_vector_size, item_count + 1);

    CTEST_ASSERT_TRUE(cutil_vector_get(test->vector, test_index, &actual_val));
    CTEST_ASSERT_INT_EQ(actual_val, 50);

    for (i = test_index + 1; i < actual_vector_size; i++) {
        expected_val = (int)i - 1;
        cutil_vector_get(test->vector, i, &actual_val);
        CTEST_ASSERT_INT_EQ(actual_val, expected_val);
    }
}

/* Tests that insertion at an invalid index does not modify the vector. */
void insert_invalid(vector_test* test) {
    int i, result;
    int invalid_index = 500, item_count = 10;
    int actual_val = 50;
    size_t actual_vector_size;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    result = cutil_vector_insert(test->vector, invalid_index, &actual_val);
    CTEST_ASSERT_FALSE(result);

    actual_vector_size = cutil_vector_size(test->vector);
    CTEST_ASSERT_INT_EQ(actual_vector_size, item_count);

    for (i = 0; i < item_count; i++) {
        cutil_vector_get(test->vector, i, &actual_val);
        CTEST_ASSERT_INT_EQ(actual_val, i);
    }
}

/* Tests that items are updated correctly when inserted in middle of vector */
void remove_middle(vector_test* test) {
    int result;
    int test_index = 0, item_count = 10;
    int actual_val = 50, expected_val;
    size_t i, actual_vector_size;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    result = cutil_vector_remove(test->vector, test_index);
    CTEST_ASSERT_TRUE(result);

    actual_vector_size = cutil_vector_size(test->vector);
    CTEST_ASSERT_INT_EQ(actual_vector_size, item_count - 1);

    for (i = test_index; i < actual_vector_size; i++) {
        expected_val = (int)i + 1;
        cutil_vector_get(test->vector, i, &actual_val);
        CTEST_ASSERT_INT_EQ(actual_val, expected_val);
    }
}

/* Tests removal of last item in the vector */
void remove_last(vector_test* test) {
    int result;
    char* test_str1 = "test1";
    char* test_str2 = "test2";
    char actual_value[16];

    char* actual_ptr = &actual_value[0];

    test->vector = cutil_vector_create(cutil_trait_cstring());
    cutil_vector_push_back(test->vector, &test_str1);
    cutil_vector_push_back(test->vector, &test_str2);

    result = cutil_vector_remove(test->vector, 1);
    CTEST_ASSERT_TRUE(result);

    CTEST_ASSERT_INT_EQ(cutil_vector_size(test->vector), 1);

    cutil_vector_get(test->vector, 0, &actual_ptr);
    CTEST_ASSERT_INT_EQ(strcmp(test_str1, actual_ptr), 0);
}

/* Tests that insertion at an invalid index does not modify the vector. */
void remove_invalid(vector_test* test) {
    int i, result;
    int invalid_index = 500, item_count = 10;
    int actual_val = 50;
    size_t actual_vector_size;

    test->vector = cutil_vector_create(cutil_trait_int());
    for (i = 0; i < 10; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    result = cutil_vector_remove(test->vector, invalid_index);
    CTEST_ASSERT_FALSE(result);

    actual_vector_size = cutil_vector_size(test->vector);
    CTEST_ASSERT_INT_EQ(actual_vector_size, item_count);

    for (i = 0; i < item_count; i++) {
        cutil_vector_get(test->vector, i, &actual_val);
        CTEST_ASSERT_INT_EQ(actual_val, i);
    }
}

/* Tests access to underlying data ptr*/
void data_ptr_check_null(vector_test* test) {
    int val = 5566;

    test->vector = cutil_vector_create(cutil_trait_int());
    CTEST_ASSERT_PTR_NULL(cutil_vector_data(test->vector));

    cutil_vector_push_back(test->vector, &val);

    cutil_vector_clear(test->vector);
    CTEST_ASSERT_PTR_NOT_NULL(cutil_vector_data(test->vector));

    cutil_vector_reset(test->vector);
    CTEST_ASSERT_PTR_NULL(cutil_vector_data(test->vector));
}

/* Tests access to underlying data ptr*/
void data_ptr_values(vector_test* test) {
    int i, item_count = 10;
    int* data;

    test->vector = cutil_vector_create(cutil_trait_int());

    for (i = 0; i < item_count; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    data = cutil_vector_data(test->vector);

    for (i = 0; i < item_count; i++) {
        CTEST_ASSERT_INT_EQ(data[i], i);
    }
}

/* popping and getting a value works as expected */
void pop_back_get(vector_test* test) {
    int int_val1 = 66;
    int int_val2 = 77;
    int actual_val = -1;
    int get_result;

    test->vector = cutil_vector_create(cutil_trait_int());
    cutil_vector_push_back(test->vector, &int_val1);
    cutil_vector_pop_back(test->vector);
    cutil_vector_push_back(test->vector, &int_val2);

    get_result = cutil_vector_get(test->vector, 0, &actual_val);
    CTEST_ASSERT_TRUE(get_result);
    CTEST_ASSERT_INT_EQ(actual_val, int_val2);
}

/* pushing and getting a pointer work correctly */
void ptr_push_and_get(vector_test* test){
    int expected_val = 999;
    int* actual_ptr;
    int int_val;
    int* int_ptr = &int_val;

    test->vector = cutil_vector_create(cutil_trait_ptr());

    *int_ptr = expected_val;

    cutil_vector_push_back(test->vector, &int_ptr);
    cutil_vector_get(test->vector, 0, &actual_ptr);

    CTEST_ASSERT_PTR_EQ(actual_ptr, int_ptr);
    CTEST_ASSERT_INT_EQ(expected_val, *actual_ptr);
}

void non_trait_equality(vector_test* test) {
    int i;
    cutil_vector* vector2 = NULL;
    cutil_trait* test_trait = malloc(sizeof(cutil_trait));
    memcpy(test_trait, cutil_trait_int(), sizeof(cutil_trait));
    test_trait->compare_func = NULL;

    test->vector = cutil_vector_create(test_trait);
    vector2 = cutil_vector_create(test_trait);

    for (i = 0; i < 10; i++) {
        cutil_vector_push_back(test->vector, &i);
        cutil_vector_push_back(vector2, &i);
    }

    CTEST_EXPECT_TRUE(cutil_vector_equals(test->vector, vector2));

    cutil_vector_destroy(vector2);

    /* Manually destroy vector because trait should not be freed before vectors that are using it */
    cutil_vector_destroy(test->vector);
    test->vector = NULL;

    free(test_trait);
}

void equality_false_diff_traits(vector_test* test) {
    cutil_vector* vector2 = cutil_vector_create(cutil_trait_ptr());
    test->vector = cutil_vector_create(cutil_trait_int());

    CTEST_EXPECT_FALSE(cutil_vector_equals(test->vector, vector2));

    cutil_vector_destroy(vector2);
}

void equality_false_diff_sizes(vector_test* test) {
    int i;
    cutil_vector* vector2 = cutil_vector_create(cutil_trait_int());
    test->vector = cutil_vector_create(cutil_trait_int());

    for (i = 0; i < 5; i++) {
        cutil_vector_push_back(test->vector, &i);
    }

    for (i = 0; i < 6; i++) {
        cutil_vector_push_back(vector2, &i);
    }

    CTEST_EXPECT_FALSE(cutil_vector_equals(test->vector, vector2));

    cutil_vector_destroy(vector2);
}

void equality_false_diff_values(vector_test* test) {
    int i;
    cutil_vector* vector2 = cutil_vector_create(cutil_trait_int());
    test->vector = cutil_vector_create(cutil_trait_int());

    for (i = 0; i < 5; i++) {
        cutil_vector_push_back(test->vector, &i);
        cutil_vector_push_back(vector2, &i);
    }

    i = 5566;
    cutil_vector_set(vector2, 3, &i);

    CTEST_EXPECT_FALSE(cutil_vector_equals(test->vector, vector2));

    cutil_vector_destroy(vector2);
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
void copy_on_push(vector_trait_func_test* test) {
    int expected_copy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_copy_count);

    CTEST_ASSERT_INT_EQ(expected_copy_count, cutil_test_trait_tracker_copy_count(test->trait_tracker));
}

/* Test that the vector copies new data when setting a particular element by index */
void copy_on_set(vector_trait_func_test* test) {
    int expected_copy_count = 1;
    char* str = "test";

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    cutil_vector_push_back(test->vector, &str);
    cutil_test_trait_tracker_reset_counts(test->trait_tracker);

    cutil_vector_set(test->vector, 0, &str);

    CTEST_ASSERT_INT_EQ(expected_copy_count, cutil_test_trait_tracker_copy_count(test->trait_tracker));
}

/* Test that the vector copies new data when inserting at an index */
void copy_on_insert(vector_trait_func_test* test) {
    int expected_copy_count = 1;
    char* str = "test";

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    cutil_vector_push_back(test->vector, &str);
    cutil_test_trait_tracker_reset_counts(test->trait_tracker);

    cutil_vector_insert(test->vector, 0, &str);

    CTEST_ASSERT_INT_EQ(expected_copy_count, cutil_test_trait_tracker_copy_count(test->trait_tracker));
}

/* Tests that the trait's destroy function is called when popping data from the end of the vector. */
void destroy_on_pop_back(vector_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_destroy_count);

    while (cutil_vector_size(test->vector) > 0) {
        cutil_vector_pop_back(test->vector);
    }

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

/* Tests that the trait's destroy function is called when removing data from middle of the vector. */
void destroy_on_vec_remove(vector_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_destroy_count);

    while (cutil_vector_size(test->vector) > 0) {
        cutil_vector_remove(test->vector, 0);
    }

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

void destroy_on_vec_destroy(vector_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_destroy_count);

    cutil_vector_destroy(test->vector);
    test->vector = NULL;

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

void destroy_on_vec_reset(vector_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_destroy_count);

    cutil_vector_reset(test->vector);

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

void destroy_on_vec_clear(vector_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_destroy_count);

    cutil_vector_clear(test->vector);

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

void destroy_on_vec_set(vector_trait_func_test* test) {
    int expected_destroy_count = 1;
    char* replace_string = "str";

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->vector = cutil_vector_create(test->trait_tracker);

    _vector_insert_test_strings(test->vector, expected_destroy_count);

    cutil_vector_set(test->vector, 0, &replace_string);

    CTEST_ASSERT_INT_EQ(expected_destroy_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

/* Tests that the comparison function is called on every item when the vector's trait defines it */
void comparison_on_vector_equals(vector_trait_func_test* test) {
    int i, expected_compare_count = 10;
    cutil_vector* vector2 = NULL;
    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_int());

    test->vector = cutil_vector_create(test->trait_tracker);
    vector2 = cutil_vector_create(test->trait_tracker);

    for (i = 0; i < expected_compare_count; i++) {
        cutil_vector_push_back(test->vector, &i);
        cutil_vector_push_back(vector2, &i);
    }

    CTEST_EXPECT_TRUE(cutil_vector_equals(test->vector, vector2));
    cutil_vector_destroy(vector2);
}

void add_vector_tests(){
    CTEST_ADD_TEST_F(vector, init_size_0_vec);
    CTEST_ADD_TEST_F(vector, get_vector_trait);

    CTEST_ADD_TEST_F(vector, push_back_empty);
    CTEST_ADD_TEST_F(vector, push_back_multiple_vec);

    CTEST_ADD_TEST_F(vector, pop_back_empty_vec);
    CTEST_ADD_TEST_F(vector, pop_back_non_empty);
    CTEST_ADD_TEST_F(vector, pop_back_and_add);
    CTEST_ADD_TEST_F(vector, pop_back_get);

    CTEST_ADD_TEST_F(vector, get_invalid);
    CTEST_ADD_TEST_F(vector, get_empty);
    CTEST_ADD_TEST_F(vector, get_valid);

    CTEST_ADD_TEST_F(vector, set_valid);
    CTEST_ADD_TEST_F(vector, set_invalid);

    CTEST_ADD_TEST_F(vector, insert_middle);
    CTEST_ADD_TEST_F(vector, insert_invalid);

    CTEST_ADD_TEST_F(vector, remove_middle);
    CTEST_ADD_TEST_F(vector, remove_last);
    CTEST_ADD_TEST_F(vector, remove_invalid);

    CTEST_ADD_TEST_F(vector, clear_empty_vec);
    CTEST_ADD_TEST_F(vector, clear);
    CTEST_ADD_TEST_F(vector, reset);

    CTEST_ADD_TEST_F(vector, data_ptr_values);
    CTEST_ADD_TEST_F(vector, data_ptr_check_null);

    CTEST_ADD_TEST_F(vector, non_trait_equality);
    CTEST_ADD_TEST_F(vector, equality_false_diff_traits);
    CTEST_ADD_TEST_F(vector, equality_false_diff_sizes);
    CTEST_ADD_TEST_F(vector, equality_false_diff_values);

    CTEST_ADD_TEST_F(vector, ptr_push_and_get);

    CTEST_ADD_TEST_F(vector_trait_func, copy_on_push);
    CTEST_ADD_TEST_F(vector_trait_func, copy_on_set);
    CTEST_ADD_TEST_F(vector_trait_func, copy_on_insert);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_pop_back);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_vec_destroy);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_vec_reset);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_vec_clear);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_vec_set);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_vec_set);
    CTEST_ADD_TEST_F(vector_trait_func, destroy_on_vec_remove);

    CTEST_ADD_TEST_F(vector_trait_func, comparison_on_vector_equals);
}
