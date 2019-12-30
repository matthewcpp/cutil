#include "test_suites.h"

#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include "cutil/list.h"
#include "ctest/ctest.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    cutil_list* list;
} list_test;

void list_test_setup(list_test* test) {
    test->list = NULL;
}

void list_test_teardown(list_test* test) {
    if (test->list) {
        cutil_list_destroy(test->list);
    }

    cutil_trait_destroy();
}

typedef struct {
    cutil_list* list;
    cutil_trait* trait_tracker;
} list_trait_func_test;

void list_trait_func_test_setup(list_trait_func_test* test) {
    memset(test, 0, sizeof(list_trait_func_test));
}

void list_trait_func_test_teardown(list_trait_func_test* test) {
    if (test->list) {
        cutil_list_destroy(test->list);
    }

    if (test->trait_tracker) {
        cutil_test_destroy_trait_tracker(test->trait_tracker);
    }

    cutil_trait_destroy();
}

CTEST_FIXTURE(list, list_test, list_test_setup, list_test_teardown)
CTEST_FIXTURE(list_trait_func, list_trait_func_test, list_trait_func_test_setup, list_trait_func_test_teardown)

/* Initializing a list sets size to 0 */
void init_size_0_list(list_test* test){
    test->list = cutil_list_create(cutil_trait_int());

    CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), 0);
}

/* Clearing a list sets its size to 0 */
void clear_size_0(list_test* test) {
    int value = 10;

    test->list = cutil_list_create(cutil_trait_int());

    cutil_list_push_front(test->list, &value);
    cutil_list_clear(test->list);

    CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), 0);
}

/* Clearing an empty list does nothing */
void clear_empty_list(list_test* test) {
    test->list = cutil_list_create(cutil_trait_int());
    cutil_list_clear(test->list);
}

/* Pushing data to the front of the list increases its size */
void push_front_size(list_test* test) {
    int value = 10;
    int i;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < 10; i++) {
        cutil_list_push_front(test->list, &value);

        CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), i + 1);
    }
}

/* Pushing items to the front appends them to the list */
void push_front_multiple(list_test* test) {
    int actual_value;
    int i;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < 5; i++) {
        int inserted_value = i * 10;
        cutil_list_push_front(test->list, &inserted_value);
    }

    for (i = 4; i >= 0; i--) {
        int expected_value = (4 - i) * 10;
        int result = cutil_list_at(test->list, i, &actual_value);

        CTEST_ASSERT_TRUE(result);
        CTEST_ASSERT_INT_EQ(expected_value, actual_value);
    }
}

/* Pushing data to the back of the list increases its size */
void push_back_size(list_test* test) {
    int value = 10;
    int i;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < 10; i++) {
        cutil_list_push_back(test->list, &value);
        
        CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), i + 1);
    }
}

/* Pushing items to the back appends them to the list */
void push_back_multiple_list(list_test* test) {
    int actual_value;
    int i;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < 5; i++) {
        int inserted_value = i * 10;
        cutil_list_push_back(test->list, &inserted_value);
    }

    for (i = 0; i < 5; i++) {
        int expected_value = i * 10;
        int result = cutil_list_at(test->list, i, &actual_value);

        CTEST_ASSERT_TRUE(result);
        CTEST_ASSERT_INT_EQ(expected_value, actual_value);
    }
}

/* Popping an item from the back removes the item */
void pop_back_removes_item(list_test* test) {
    int list_size = 5;
    int actual_value;
    int i;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < list_size; i++) {
        cutil_list_push_back(test->list, &i);
    }

    cutil_list_pop_back(test->list);

    CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), list_size - 1);
    CTEST_ASSERT_FALSE(cutil_list_at(test->list, list_size - 1, &actual_value));
}

/* Popping the only item from the front correctly sets list pointers */
void pop_back_one_item(list_test* test) {
    int val = 55;

    test->list = cutil_list_create(cutil_trait_int());

    cutil_list_push_back(test->list, &val);
    cutil_list_pop_back(test->list);

    CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), 0);
}

/* Popping the back of an empty list does nothing */
void pop_back_empty_list(list_test* test) {
    test->list = cutil_list_create(cutil_trait_int());

    CTEST_ASSERT_FALSE(cutil_list_pop_front(test->list));
}

/* Popping an item from the front removes the item */
void pop_front_removes_item(list_test* test) {
    int list_size = 5;
    int actual_value = 0;
    int at_result;
    int i;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < list_size; i++) {
        cutil_list_push_back(test->list, &i);
    }

    cutil_list_pop_front(test->list);
    CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), list_size - 1);


    at_result = cutil_list_at(test->list, 0, &actual_value);
    CTEST_ASSERT_TRUE(at_result);
    CTEST_ASSERT_INT_EQ(actual_value, 1);
}

/* Popping the only item from the back correctly sets list pointers */
void pop_front_one_item(list_test* test) {
    int val = 55;

    test->list = cutil_list_create(cutil_trait_int());

    cutil_list_push_back(test->list, &val);
    cutil_list_pop_front(test->list);

    CTEST_ASSERT_INT_EQ(cutil_list_size(test->list), 0);
}

/* Popping the front of an empty list does nothing */
void pop_front_empty(list_test* test) {
    test->list = cutil_list_create(cutil_trait_int());

    CTEST_ASSERT_FALSE(cutil_list_pop_front(test->list));
}

/* Pushing a pointer to the front and retrieving it functions correctly */
void front_pointer(list_test* test) {
    int* frontptr = NULL;
    int get_front_result;
    int ptr_val = 55;
    int* iptr = &ptr_val;

    test->list = cutil_list_create(cutil_trait_ptr());

    cutil_list_push_front(test->list, &iptr);

    get_front_result = cutil_list_front(test->list, &frontptr);

    CTEST_ASSERT_TRUE(get_front_result);
    CTEST_ASSERT_PTR_EQ(frontptr, iptr);
}

/* Pushing a pointer to the back and retrieving it functions correctly */
void back_pointer(list_test* test) {
    int* backptr = NULL;
    int get_back_result;
    int ptr_val = 55;
    int* iptr = &ptr_val;

    test->list = cutil_list_create(cutil_trait_ptr());

    cutil_list_push_back(test->list, &iptr);

    get_back_result = cutil_list_back(test->list, &backptr);

    CTEST_ASSERT_TRUE(get_back_result);
    CTEST_ASSERT_PTR_EQ(backptr, iptr);
}

/* Pushing data to the front and retrieving it functions correctly */
void front_pod(list_test* test) {
    int ival = 3;
    int front_val;
    int get_front_result;

    test->list = cutil_list_create(cutil_trait_int());

    cutil_list_push_front(test->list, &ival);
    get_front_result = cutil_list_front(test->list, &front_val);

    CTEST_ASSERT_TRUE(get_front_result);
    CTEST_ASSERT_INT_EQ(front_val, ival);
}

/* Get data from list by index*/
void at_valid_index(list_test* test) {
    int item_count = 5, i = 0, actual_value = 0;

    test->list = cutil_list_create(cutil_trait_int());
    
    for (i = 0; i < item_count; i++) {
        cutil_list_push_back(test->list, &i);
    }

    for (i = 0; i < item_count; i++) {
        CTEST_ASSERT_TRUE(cutil_list_at(test->list, (size_t)i, &actual_value));

        CTEST_ASSERT_INT_EQ(actual_value, i);
    }
}

/* Get data from invalid index returns false*/
void at_invalid_index(list_test* test) {
    int item_count = 5, i = 0, actual_value = 0;

    test->list = cutil_list_create(cutil_trait_int());

    for (i = 0; i < item_count; i++) {
        cutil_list_push_back(test->list, &i);
    }

    CTEST_ASSERT_FALSE(cutil_list_at(test->list, (size_t)item_count, &actual_value));
}

/* Getting the front item of an empty list returns false */
void front_empty(list_test* test) {
    int front_val = 0;
    int get_front_result = 0;

    test->list = cutil_list_create(cutil_trait_int());

    get_front_result = cutil_list_front(test->list, (void*)&front_val);

    CTEST_ASSERT_FALSE(get_front_result);
}

/* Pushing data to the back and retrieving it functions correctly */
void back_pod(list_test* test) {
    int ival = 3;
    int back_val;
    int get_back_result;

    test->list = cutil_list_create(cutil_trait_int());

    cutil_list_push_back(test->list, &ival);
    get_back_result = cutil_list_back(test->list, (void*)&back_val);

    CTEST_ASSERT_TRUE(get_back_result);
    CTEST_ASSERT_INT_EQ(back_val, ival);
}

/* Getting the front item of an empty list returns false */
void back_empty(list_test* test) {
    int front_val = 0;
    int get_back_result = 0;

    test->list = cutil_list_create(cutil_trait_int());

    get_back_result = cutil_list_back(test->list, (void*)&front_val);

    CTEST_ASSERT_FALSE(get_back_result);
}

void _list_insert_test_strings(cutil_list* list, int count, int push_back) {
    int i;
    char* buffer = malloc(20);

    for (i = 0; i < count; i++) {
        cutil_snprintf_func(buffer, 20, "str %d", i);

        if (push_back) {
            cutil_list_push_back(list, &buffer);
        }
        else {
            cutil_list_push_front(list, &buffer);
        }
    }

    free(buffer);
}

void copy_on_push_back(list_trait_func_test* test) {
    int expected_copy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->list = cutil_list_create(test->trait_tracker);

    _list_insert_test_strings(test->list, expected_copy_count, 1);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(test->trait_tracker), expected_copy_count);
}

void copy_on_push_front(list_trait_func_test* test) {
    int expected_copy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->list = cutil_list_create(test->trait_tracker);

    _list_insert_test_strings(test->list, expected_copy_count, 0);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(test->trait_tracker), expected_copy_count);
}

void delete_pop_front(list_trait_func_test* test) {
    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->list = cutil_list_create(test->trait_tracker);

    _list_insert_test_strings( test->list, 1, 1);

    cutil_list_pop_front( test->list);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->trait_tracker), 1);
}

void delete_pop_back(list_trait_func_test* test) {
    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->list = cutil_list_create(test->trait_tracker);
    
    _list_insert_test_strings(test->list, 1, 1);

    cutil_list_pop_back(test->list);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->trait_tracker), 1);
}

void delete_on_destroy(list_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->list = cutil_list_create(test->trait_tracker);
    
    _list_insert_test_strings(test->list, expected_destroy_count, 1);

    cutil_list_destroy(test->list);
    test->list = NULL;

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->trait_tracker), expected_destroy_count);
}

void delete_on_clear(list_trait_func_test* test) {
    int expected_destroy_count = 10;

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->list = cutil_list_create(test->trait_tracker);
    
    _list_insert_test_strings(test->list, expected_destroy_count, 1);

    cutil_list_clear(test->list);

    CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(test->trait_tracker), expected_destroy_count);
}

void add_list_tests(){
    CTEST_ADD_TEST_F(list, init_size_0_list);

    CTEST_ADD_TEST_F(list, clear_size_0);
    CTEST_ADD_TEST_F(list, clear_empty_list);

    CTEST_ADD_TEST_F(list, push_front_size);
    CTEST_ADD_TEST_F(list, push_front_multiple);

    CTEST_ADD_TEST_F(list, push_back_size);
    CTEST_ADD_TEST_F(list, push_back_multiple_list);
    
    CTEST_ADD_TEST_F(list, pop_back_removes_item);
    CTEST_ADD_TEST_F(list, pop_back_one_item);

    CTEST_ADD_TEST_F(list, pop_front_removes_item);
    CTEST_ADD_TEST_F(list, pop_front_one_item);
    CTEST_ADD_TEST_F(list, pop_front_empty);
    CTEST_ADD_TEST_F(list, pop_back_empty_list);

    CTEST_ADD_TEST_F(list, front_pod);
    CTEST_ADD_TEST_F(list, front_pointer);
    CTEST_ADD_TEST_F(list, front_empty);

    CTEST_ADD_TEST_F(list, back_pod);
    CTEST_ADD_TEST_F(list, back_pointer);
    CTEST_ADD_TEST_F(list, back_empty);
    
    CTEST_ADD_TEST_F(list, at_valid_index);
    CTEST_ADD_TEST_F(list, at_invalid_index);

    CTEST_ADD_TEST_F(list_trait_func, copy_on_push_back);
    CTEST_ADD_TEST_F(list_trait_func, copy_on_push_front);
    CTEST_ADD_TEST_F(list_trait_func, delete_pop_front);
    CTEST_ADD_TEST_F(list_trait_func, delete_pop_back);
    CTEST_ADD_TEST_F(list_trait_func, delete_on_destroy);
    CTEST_ADD_TEST_F(list_trait_func, delete_on_clear);
}
