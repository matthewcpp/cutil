#include "test_suites.h"

#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include "cutil/list.h"
#include "ctest/ctest.h"

#include <stdlib.h>
#include <stdio.h>

cutil_list* g_list = NULL;
cutil_trait* g_list_trait = NULL;

void list_before_each() {
	g_list = cutil_list_create(cutil_trait_int());
}

void list_after_each() {
	cutil_list_destroy(g_list);
	cutil_trait_destroy();

	g_list = NULL;
}

void listp_before_each() {
	g_list = cutil_list_create(cutil_trait_ptr());
}

/* Initializing a list sets size to 0 */
void list_init_size_0(){
    CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

/* Clearing an empty list does nothing */
void list_clear_empty() {
	cutil_list_clear(g_list);
}

/* Clearing a list sets its size to 0 */
void list_clear_size_0 (){
	int value = 10;

	cutil_list_push_front(g_list, &value);
	cutil_list_clear(g_list);

	CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

/* Pushing data to the front of the list increases its size */
void list_push_data_front_size() {
	int value = 10;

	int i;
	for (i = 0; i < 10; i++) {
		cutil_list_push_front(g_list, &value);

		CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), i + 1);
	}
}

/* Pushing data to the back of the list increases its size */
void list_push_data_back_size() {
	int value = 10;

	int i;
	for (i = 0; i < 10; i++) {
		cutil_list_push_back(g_list, &value);
		
		CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), i + 1);
	}
}

/* Pushing items to the back appends them to the list */
void list_data_push_multiple_back() {
	int actual_value;

	int i;
	for (i = 0; i < 5; i++) {
		int inserted_value = i * 10;
		cutil_list_push_back(g_list, &inserted_value);
	}

	for (i = 0; i < 5; i++) {
		int expected_value = i * 10;
		int result = cutil_list_at(g_list, i, &actual_value);

		CTEST_ASSERT_TRUE(result);
		CTEST_ASSERT_INT_EQ(expected_value, actual_value);
	}
}

/* Pushing items to the front appends them to the list */
void list_data_push_multiple_front() {
	int actual_value;

	int i;
	for (i = 0; i < 5; i++) {
		int inserted_value = i * 10;
		cutil_list_push_front(g_list, &inserted_value);
	}

	for (i = 4; i >= 0; i--) {
		int expected_value = (4 - i) * 10;
		int result = cutil_list_at(g_list, i, &actual_value);

		CTEST_ASSERT_TRUE(result);
		CTEST_ASSERT_INT_EQ(expected_value, actual_value);
	}
}

/* Popping an item from the back removes the item */
void list_multiple_pop_back_removes_item() {
	int list_size = 5;
	int actual_value;

	int i;
	for (i = 0; i < list_size; i++) {
		cutil_list_push_back(g_list, &i);
	}

	cutil_list_pop_back(g_list);

	CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), list_size - 1);
	CTEST_ASSERT_FALSE(cutil_list_at(g_list, list_size - 1, &actual_value));
}

/* Popping an item from the front removes the item */
void list_multiple_pop_front_removes_item() {
	int list_size = 5;
	int actual_value = 0;
	int at_result;

	int i;
	for (i = 0; i < list_size; i++) {
		cutil_list_push_back(g_list, &i);
	}

	cutil_list_pop_front(g_list);
	CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), list_size - 1);


	at_result = cutil_list_at(g_list, 0, &actual_value);
	CTEST_ASSERT_TRUE(at_result);
	CTEST_ASSERT_INT_EQ(actual_value, 1);
}

/* Popping the only item from the front correctly sets list pointers */
void list_pop_back_one_item() {
	int val = 55;
	cutil_list_push_back(g_list, &val);

	cutil_list_pop_back(g_list);

	CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

/* Popping the only item from the back correctly sets list pointers */
void list_pop_front_one_item() {
	int val = 55;
	cutil_list_push_back(g_list, &val);

	cutil_list_pop_front(g_list);

	CTEST_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

/* Popping the front of an empty list does nothing */
void list_pop_front_empty() {
	CTEST_ASSERT_FALSE(cutil_list_pop_front(g_list));
}

/* Popping the back of an empty list does nothing */
void list_pop_back_empty() {
	CTEST_ASSERT_FALSE(cutil_list_pop_front(g_list));
}

/* Pushing a pointer to the front and retrieving it functions correctly */
void list_push_get_front_pointer() {
	int* frontptr = NULL;
	int get_front_result;

	int* iptr = malloc(sizeof(int));
	*iptr = 55;

	cutil_list_push_front(g_list, &iptr);


	get_front_result = cutil_list_front(g_list, &frontptr);

	CTEST_ASSERT_TRUE(get_front_result);
	CTEST_ASSERT_PTR_EQ(frontptr, iptr);

	free(iptr);
}

/* Pushing a pointer to the back and retrieving it functions correctly */
void push_get_back_pointer() {
	int* backptr = NULL;
	int get_back_result;

	int* iptr = malloc(sizeof(int));
	*iptr = 55;
	cutil_list_push_back(g_list, &iptr);

	get_back_result = cutil_list_back(g_list, &backptr);

	CTEST_ASSERT_TRUE(get_back_result);
	CTEST_ASSERT_PTR_EQ(backptr, iptr);

	free(iptr);
}

/* Pushing data to the front and retrieving it functions correctly */
void push_get_front_data() {
	int ival = 3;
	int front_val;
	int get_front_result;

	cutil_list_push_front(g_list, &ival);
	get_front_result = cutil_list_front(g_list, &front_val);

	CTEST_ASSERT_TRUE(get_front_result);
	CTEST_ASSERT_INT_EQ(front_val, ival);
}

/* Getting the front item of an empty list returns false */
void list_get_front_data_empty() {
	int front_val = 0;
	int get_front_result = cutil_list_front(g_list, (void*)&front_val);

	CTEST_ASSERT_FALSE(get_front_result);
}

/* Pushing data to the back and retrieving it functions correctly */
void push_get_back_data() {
	int ival = 3;
	int back_val;
	int get_back_result;

	cutil_list_push_back(g_list, &ival);
	get_back_result = cutil_list_back(g_list, (void*)&back_val);

	CTEST_ASSERT_TRUE(get_back_result);
	CTEST_ASSERT_INT_EQ(back_val, ival);
}

/* Getting the front item of an empty list returns false */
void list_get_back_data_empty() {
	int front_val = 0;
	int get_back_result = cutil_list_back(g_list, (void*)&front_val);

	CTEST_ASSERT_FALSE(get_back_result);
}

void list_trait_before_each() {
	g_list_trait = cutil_test_create_trait_tracker(cutil_trait_cstring());
	g_list = cutil_list_create(g_list_trait);
}

void list_trait_after_each() {
	if (g_list) {
		cutil_list_destroy(g_list);
		g_list = NULL;
	}

	if (g_list_trait) {
		cutil_test_destroy_trait_tracker(g_list_trait);
		g_list_trait = NULL;
	}

	cutil_trait_destroy();
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

void list_copy_on_push_back() {
	int expected_copy_count = 10;
	_list_insert_test_strings(g_list, expected_copy_count, 1);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(g_list_trait), expected_copy_count);
}

void list_copy_on_push_front() {
	int expected_copy_count = 10;
	_list_insert_test_strings(g_list, expected_copy_count, 0);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_copy_count(g_list_trait), expected_copy_count);
}

void list_copy_on_at() {
	int item_count = 1;
	char* buffer = NULL;

	_list_insert_test_strings(g_list, item_count, 1);

	CTEST_EXPECT_TRUE(cutil_list_at(g_list, 0, &buffer));

	CTEST_EXPECT_INT_EQ(cutil_test_trait_tracker_copy_count(g_list_trait), item_count + 1)

    if (buffer){
        free(buffer);
    }
}

void list_delete_pop_front() {
	_list_insert_test_strings(g_list, 1, 1);

	cutil_list_pop_front(g_list);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_list_trait), 1);
}

void list_delete_pop_back() {
	_list_insert_test_strings(g_list, 1, 1);

	cutil_list_pop_back(g_list);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_list_trait), 1);
}

void list_delete_on_destroy() {
	int expected_destroy_count = 10;
	_list_insert_test_strings(g_list, expected_destroy_count, 1);

	cutil_list_destroy(g_list);
	g_list = NULL;

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_list_trait), expected_destroy_count);
}

void list_delete_on_clear() {
	int expected_destroy_count = 10;
	_list_insert_test_strings(g_list, expected_destroy_count, 1);

	cutil_list_clear(g_list);

	CTEST_ASSERT_INT_EQ(cutil_test_trait_tracker_destroy_count(g_list_trait), expected_destroy_count);
}

void add_list_tests(){
    ctest_suite("list");
    ctest_suite_before_each(&list_before_each);
    ctest_suite_after_each(&list_after_each);

    CTEST_ADD_TEST(list_init_size_0);

    CTEST_ADD_TEST(list_clear_size_0);
    CTEST_ADD_TEST(list_clear_empty);

    CTEST_ADD_TEST(list_push_data_front_size);
    CTEST_ADD_TEST(list_push_data_back_size);

    CTEST_ADD_TEST(list_data_push_multiple_back);
    CTEST_ADD_TEST(list_data_push_multiple_front);

    CTEST_ADD_TEST(list_multiple_pop_back_removes_item);
    CTEST_ADD_TEST(list_multiple_pop_front_removes_item);

    CTEST_ADD_TEST(list_pop_back_one_item);
    CTEST_ADD_TEST(list_pop_front_one_item);

    CTEST_ADD_TEST(list_pop_front_empty);
    CTEST_ADD_TEST(list_pop_back_empty);

    CTEST_ADD_TEST(push_get_front_data);
    CTEST_ADD_TEST(push_get_back_data);

    CTEST_ADD_TEST(list_get_back_data_empty);
    CTEST_ADD_TEST(list_get_front_data_empty);

    ctest_suite("listp");
    ctest_suite_before_each(&listp_before_each);
    ctest_suite_after_each(&list_after_each);

    CTEST_ADD_TEST(list_push_get_front_pointer);
    CTEST_ADD_TEST(push_get_back_pointer);

	ctest_suite("list-trait");
	ctest_suite_before_each(&list_trait_before_each);
	ctest_suite_after_each(&list_trait_after_each);

	CTEST_ADD_TEST(list_copy_on_push_back)
	CTEST_ADD_TEST(list_copy_on_push_front)
	CTEST_ADD_TEST(list_copy_on_at)
	CTEST_ADD_TEST(list_delete_pop_front)
	CTEST_ADD_TEST(list_delete_pop_back);
	CTEST_ADD_TEST(list_delete_on_destroy);
	CTEST_ADD_TEST(list_delete_on_clear);
}
