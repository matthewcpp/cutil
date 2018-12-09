#include "test_suites.h"

#include "list.h"
#include "testing.h"

#include <stdlib.h>

cutil_list* g_list = NULL;

void list_before_each() {
	g_list = cutil_list_create(sizeof(int));
}

void list_after_each() {
	cutil_list_destroy(g_list);
	g_list = NULL;
}

void listp_before_each() {
	g_list = cutil_list_create(sizeof(int*));
}

// Initializing a list sets size to 0
void list_init_size_0(){
    CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

// Clearing an empty list does nothing
void list_clear_empty() {
	cutil_list_clear(g_list);
}

// Clearing a list sets its size to 0
void list_clear_size_0 (){
	int value = 10;

	cutil_list_push_front(g_list, &value);
	cutil_list_clear(g_list);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

// Pushing data to the front of the list increases its size
void list_push_data_front_size() {
	int value = 10;

	for (int i = 0; i < 10; i++) {
		cutil_list_push_front(g_list, &value);

		CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), i + 1);
	}
}

// Pushing data to the back of the list increases its size
void list_push_data_back_size() {
	int value = 10;

	for (int i = 0; i < 10; i++) {
		cutil_list_push_back(g_list, &value);
		
		CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), i + 1);
	}
}

// Pushing items to the back appends them to the list
void list_data_push_multiple_back() {
	for (int i = 0; i < 5; i++) {
		int inserted_value = i * 10;
		cutil_list_push_back(g_list, &inserted_value);
	}

	int actual_value;
	for (int i = 0; i < 5; i++) {
		int expected_value = i * 10;
		bool result = cutil_list_at(g_list, i, &actual_value);

		CUTIL_TESTING_ASSERT_TRUE(result);
		CUTIL_TESTING_ASSERT_INT_EQ(expected_value, actual_value);
	}
}

// Pushing items to the front appends them to the list
void list_data_push_multiple_front() {
	for (int i = 0; i < 5; i++) {
		int inserted_value = i * 10;
		cutil_list_push_front(g_list, &inserted_value);
	}

	int actual_value;
	for (int i = 4; i >= 0; i--) {
		int expected_value = (4 - i) * 10;
		bool result = cutil_list_at(g_list, i, &actual_value);

		CUTIL_TESTING_ASSERT_TRUE(result);
		CUTIL_TESTING_ASSERT_INT_EQ(expected_value, actual_value);
	}
}

// Popping an item from the back removes the item
void list_multiple_pop_back_removes_item() {
	int list_size = 5;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(g_list, &i);
	}

	cutil_list_pop_back(g_list);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), list_size - 1);

	int actual_value;
	CUTIL_TESTING_ASSERT_FALSE(cutil_list_at(g_list, list_size - 1, &actual_value));
}

// Popping an item from the front removes the item
void list_multiple_pop_front_removes_item() {
	int list_size = 5;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(g_list, &i);
	}

	cutil_list_pop_front(g_list);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), list_size - 1);
	
	int actual_value = 0;
	bool result = cutil_list_at(g_list, 0, &actual_value);

	CUTIL_TESTING_ASSERT_TRUE(result);
	CUTIL_TESTING_ASSERT_INT_EQ(actual_value, 1);
}

// Popping the only item from the front correctly sets list pointers
void list_pop_back_one_item() {
	int val = 55;
	cutil_list_push_back(g_list, &val);

	cutil_list_pop_back(g_list);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

// Popping the only item from the back correctly sets list pointers
void list_pop_front_one_item() {
	int val = 55;
	cutil_list_push_back(g_list, &val);

	cutil_list_pop_front(g_list);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

// Popping the front of an empty list does nothing
void list_pop_front_empty() {
	CUTIL_TESTING_ASSERT_FALSE(cutil_list_pop_front(g_list));
}

// Popping the back of an empty list does nothing
void list_pop_back_empty() {
	CUTIL_TESTING_ASSERT_FALSE(cutil_list_pop_front(g_list));
}

// Pushing a pointer to the front and retrieving it functions correctly
void list_push_get_front_pointer() {
	int* iptr = malloc(sizeof(int));
	*iptr = 55;

	cutil_list_push_front(g_list, &iptr);

	int* frontptr;
	bool get_front_result = cutil_list_front(g_list, &frontptr);

	CUTIL_TESTING_ASSERT_TRUE(get_front_result);
	CUTIL_TESTING_ASSERT_PTR_EQ(frontptr, iptr);

	free(iptr);
}

// Pushing a pointer to the back and retrieving it functions correctly
void push_get_back_pointer() {
	int* iptr = malloc(sizeof(int));
	*iptr = 55;
	cutil_list_push_back(g_list, &iptr);

	int* backptr = NULL;
	bool get_back_result = cutil_list_back(g_list, &backptr);

	CUTIL_TESTING_ASSERT_TRUE(get_back_result);
	CUTIL_TESTING_ASSERT_PTR_EQ(backptr, iptr);

	free(iptr);
}

// Pushing data to the front and retrieving it functions correctly
void push_get_front_data() {
	int ival = 3;
	int front_val;

	cutil_list_push_front(g_list, &ival);
	bool get_front_result = cutil_list_front(g_list, &front_val);

	CUTIL_TESTING_ASSERT_TRUE(get_front_result);
	CUTIL_TESTING_ASSERT_INT_EQ(front_val, ival);
}

// Getting the front item of an empty list returns false
void list_get_front_data_empty() {
	int front_val = 0;
	bool get_front_result = cutil_list_front(g_list, (void*)&front_val);

	CUTIL_TESTING_ASSERT_FALSE(get_front_result);
}

// Pushing data to the back and retrieving it functions correctly
void push_get_back_data() {
	int ival = 3;
	int back_val;

	cutil_list_push_back(g_list, &ival);
	bool get_back_result = cutil_list_back(g_list, (void*)&back_val);

	CUTIL_TESTING_ASSERT_TRUE(get_back_result);
	CUTIL_TESTING_ASSERT_INT_EQ(back_val, ival);
}

// Getting the front item of an empty list returns false
void list_get_back_data_empty() {
	int front_val = 0;
	bool get_back_result = cutil_list_back(g_list, (void*)&front_val);

	CUTIL_TESTING_ASSERT_FALSE(get_back_result);
}

void add_list_tests(){
    cutil_testing_suite("list");
	cutil_testing_suite_before_each(&list_before_each);
	cutil_testing_suite_after_each(&list_after_each);

	CUTIL_TESTING_ADD(list_init_size_0);

	CUTIL_TESTING_ADD(list_clear_size_0);
	CUTIL_TESTING_ADD(list_clear_empty);

	CUTIL_TESTING_ADD(list_push_data_front_size);
	CUTIL_TESTING_ADD(list_push_data_back_size);

	CUTIL_TESTING_ADD(list_data_push_multiple_back);
	CUTIL_TESTING_ADD(list_data_push_multiple_front);

	CUTIL_TESTING_ADD(list_multiple_pop_back_removes_item);
	CUTIL_TESTING_ADD(list_multiple_pop_front_removes_item);

	CUTIL_TESTING_ADD(list_pop_back_one_item);
	CUTIL_TESTING_ADD(list_pop_front_one_item);

	CUTIL_TESTING_ADD(list_pop_front_empty);
	CUTIL_TESTING_ADD(list_pop_back_empty);

	CUTIL_TESTING_ADD(push_get_front_data);
	CUTIL_TESTING_ADD(push_get_back_data);

	CUTIL_TESTING_ADD(list_get_back_data_empty);
	CUTIL_TESTING_ADD(list_get_front_data_empty);

	cutil_testing_suite("listp");
	cutil_testing_suite_before_each(&listp_before_each);
	cutil_testing_suite_after_each(&list_after_each);

	CUTIL_TESTING_ADD(list_push_get_front_pointer);
	CUTIL_TESTING_ADD(push_get_back_pointer);
}
