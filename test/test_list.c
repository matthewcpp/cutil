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
#ifdef CUTIL_DEBUGGING
	int expected_debug_generation = g_list->_debug_generation;
	cutil_list_clear(g_list);
	int actual_debug_generation = g_list->_debug_generation;

	CUTIL_TESTING_ASSERT_INT_EQ(expected_debug_generation, actual_debug_generation);
#else
	cutil_list_clear(g_list);
#endif
}

// Clearing a list sets its size to 0
void list_clear_size_0 (){
	int value = 10;

	cutil_list_push_front(g_list, &value);
	cutil_list_clear(g_list);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), 0);
}

// Pushing data to the front of the list increses its size
void list_push_data_front_size() {
	int value = 10;

	for (int i = 0; i < 10; i++) {
		cutil_list_push_front(g_list, &value);

		CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), i + 1);
	}
}

// Pushing data to the back of the list increses its size
void list_push_data_back_size() {
	int value = 10;

	for (int i = 0; i < 10; i++) {
		cutil_list_push_back(g_list, &value);
		
		CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), i + 1);
	}
}

// Pushing an item to the front of the list sets its pointers
void list_push_data_one_front_list_pointers() {
	int value = 10;
	cutil_list_push_front(g_list, &value);

	_cutil_list_node* new_node = g_list->_base.next;

	CUTIL_TESTING_ASSERT_PTR_EQ(g_list->_base.next, new_node);
	CUTIL_TESTING_ASSERT_PTR_EQ(g_list->_base.prev, new_node);

	CUTIL_TESTING_ASSERT_PTR_EQ(new_node->prev, &g_list->_base);
	CUTIL_TESTING_ASSERT_PTR_EQ(new_node->next, &g_list->_base);
}

// Pushing an item to the back of the list sets its pointers
void list_push_data_one_back_list_pointers() {
	int value = 10;
	cutil_list_push_back(g_list, &value);

	_cutil_list_node* new_node = g_list->_base.next;

	CUTIL_TESTING_ASSERT_PTR_EQ(g_list->_base.next, new_node);
	CUTIL_TESTING_ASSERT_PTR_EQ(g_list->_base.prev, new_node);

	CUTIL_TESTING_ASSERT_PTR_EQ(new_node->prev, &g_list->_base);
	CUTIL_TESTING_ASSERT_PTR_EQ(new_node->next, &g_list->_base);
}

// Pushing items to the back appends them to the list
void list_data_push_multiple_back() {
	for (int i = 0; i < 10; i++) {
		cutil_list_push_back(g_list, &i);
	}

	_cutil_list_node* current_node = g_list->_base.next;
	unsigned int list_size = cutil_list_size(g_list);

	for (unsigned int i = 0; i < list_size; i++) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->next;
	}

	current_node = g_list->_base.prev;
	for (unsigned int i = list_size - 1; i > 0; i--) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->prev;
	}
}


// Pushing items to the front appends them to the list
void list_data_push_multiple_front() {
	for (int i = 0; i < 10; i++) {
		cutil_list_push_front(g_list, &i);
	}

	_cutil_list_node* current_node = g_list->_base.prev;
	unsigned int list_size = cutil_list_size(g_list);

	for (unsigned int i = 0; i < list_size; i++) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->prev;
	}

	current_node = g_list->_base.next;
	for (unsigned int i = list_size - 1; i > 0; i--) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->next;
	}
}


// Popping an item from the back removes the item
void list_multiple_pop_back_removes_item() {
	int list_size = 10;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(g_list, &i);
	}

	cutil_list_pop_back(g_list);
	int back_node_value = *((int *)g_list->_base.prev->data);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), list_size - 1);
	CUTIL_TESTING_ASSERT_INT_EQ(back_node_value, list_size - 2);
}

// Popping an item from the front removes the item
void list_multiple_pop_front_removes_item() {
	int list_size = 10;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(g_list, &i);
	}

	cutil_list_pop_front(g_list);
	int front_node_value = *((int *)g_list->_base.next->data);

	CUTIL_TESTING_ASSERT_INT_EQ(cutil_list_size(g_list), list_size - 1);
	CUTIL_TESTING_ASSERT_INT_EQ(front_node_value, 1);
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
#ifdef CUTIL_DEBUGGING
	int expected_debug_generation = g_list->_debug_generation;
	cutil_list_pop_front(g_list);
	int actual_debug_generation = g_list->_debug_generation;

	CUTIL_TESTING_ASSERT_INT_EQ(expected_debug_generation, actual_debug_generation);
#else
	cutil_list_pop_front(g_list);
#endif
}

// Popping the back of an empty list does nothing
void list_pop_back_empty() {
#ifdef CUTIL_DEBUGGING
	int expected_debug_generation = g_list->_debug_generation;
	cutil_list_pop_back(g_list);
	int actual_debug_generation = g_list->_debug_generation;

	CUTIL_TESTING_ASSERT_INT_EQ(expected_debug_generation, actual_debug_generation);
#else
	cutil_list_pop_front(g_list);
#endif
}

// Pushing a pointer to the front and retrieving it functions correctly
void list_push_get_front_pointer() {
	int *iptr = malloc(sizeof(int));
	*iptr = 55;

	cutil_list_push_front(g_list, &iptr);

	int *frontptr;
	bool get_front_result = cutil_list_get_front(g_list, &frontptr);

	CUTIL_TESTING_ASSERT_TRUE(get_front_result);
	CUTIL_TESTING_ASSERT_PTR_EQ(frontptr, iptr);

	free(iptr);
}

// Pushing a pointer to the back and retrieving it functions correctly
void push_get_back_pointer() {
	int *iptr = malloc(sizeof(int));
	*iptr = 55;
	cutil_list_push_back(g_list, &iptr);

	int *backptr = NULL;
	bool get_back_result = cutil_list_get_back(g_list, &backptr);

	CUTIL_TESTING_ASSERT_TRUE(get_back_result);
	CUTIL_TESTING_ASSERT_PTR_EQ(backptr, iptr);

	free(iptr);
}

// Pushing data to the front and retrieving it functions correctly
void push_get_front_data() {
	int ival = 3;
	int front_val;

	cutil_list_push_front(g_list, &ival);
	bool get_front_result = cutil_list_get_front(g_list, &front_val);

	CUTIL_TESTING_ASSERT_TRUE(get_front_result);
	CUTIL_TESTING_ASSERT_INT_EQ(front_val, ival);
}

// Getting the front item of an empty list returns false
void list_get_front_data_empty() {
	int front_val = 0;
	bool get_front_result = cutil_list_get_front(g_list, (void *)&front_val);

	CUTIL_TESTING_ASSERT_FALSE(get_front_result);
}

// Pushing data to the back and retrieving it functions correctly
void push_get_back_data() {
	int ival = 3;
	int back_val;

	cutil_list_push_back(g_list, &ival);
	bool get_back_result = cutil_list_get_back(g_list, (void *)&back_val);

	CUTIL_TESTING_ASSERT_TRUE(get_back_result);
	CUTIL_TESTING_ASSERT_INT_EQ(back_val, ival);
}

// Getting the front item of an empty list returns false
void list_get_back_data_empty() {
	int front_val = 0;
	bool get_back_result = cutil_list_get_back(g_list, (void *)&front_val);

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