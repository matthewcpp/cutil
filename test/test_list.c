#include "test_suites.h"

#include "testing.h"
#include "list.h"

#include <stdlib.h>

// Initializing a list sets size to 0
void list_init_size_0(){
    cutil_list list;
    cutil_list_init(&list, sizeof(int));

    CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
}

// Initializing a list sets front and back to NULL
void list_init_null_pointers() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);
}

// Clearing a list sets front and back to NULL
void list_clear_null_pointers() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int value = 10;

	cutil_list_push_front(&list, &value);

	cutil_list_clear(&list);

	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);
}

// Clearing an empty list does nothing
void list_clear_empty() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	cutil_list_clear(&list);

	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);
}

// Clearing a list sets its size to 0
void list_clear_size_0 (){
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int value = 10;

	cutil_list_push_front(&list, &value);

	cutil_list_clear(&list);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
}

// Pushing data to the front of the list increses its size
void list_push_data_front_size() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int value = 10;

	for (int i = 0; i < 10; i++) {
		cutil_list_push_front(&list, &value);
		CUTIL_TESTING_ASSERT_INT_EQ(list.size, i + 1);
	}
	
	cutil_list_destroy(&list);
}

// Pushing data to the back of the list increses its size
void list_push_data_back_size() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int value = 10;

	for (int i = 0; i < 10; i++) {
		cutil_list_push_back(&list, &value);
		CUTIL_TESTING_ASSERT_INT_EQ(list.size, i + 1);
	}

	cutil_list_destroy(&list);
}

// Pushing an item to the front of the list sets its pointers
void list_push_data_one_front_list_pointers() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int value = 10;
	cutil_list_push_front(&list, &value);

	CUTIL_TESTING_ASSERT_PTR_EQ(list.front, list.back);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front->next);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front->prev);

	cutil_list_destroy(&list);
}

// Pushing an item to the back of the list sets its pointers
void list_push_data_one_back_list_pointers() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int value = 10;
	cutil_list_push_back(&list, &value);

	CUTIL_TESTING_ASSERT_PTR_EQ(list.front, list.back);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back->next);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back->prev);

	cutil_list_destroy(&list);
}

// Pushing items to the back appends them to the list
void list_data_push_multiple_back() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	for (int i = 0; i < 10; i++) {
		cutil_list_push_back(&list, &i);
	}

	cutil_list_node* current_node = list.front;
	for (unsigned int i = 0; i < list.size; i++) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->next;
	}

	current_node = list.back;
	for (unsigned int i = list.size - 1; i > 0; i--) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->prev;
	}

	cutil_list_destroy(&list);
}


// Pushing items to the front appends them to the list
void list_data_push_multiple_front() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	for (int i = 0; i < 10; i++) {
		cutil_list_push_front(&list, &i);
	}

	cutil_list_node* current_node = list.back;
	for (unsigned int i = 0; i < list.size; i++) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->prev;
	}

	current_node = list.front;
	for (unsigned int i = list.size - 1; i > 0; i--) {
		CUTIL_TESTING_ASSERT_PTR_NOT_NULL(current_node);

		int current_node_value = *((int *)current_node->data);
		CUTIL_TESTING_ASSERT_INT_EQ(current_node_value, i);

		current_node = current_node->next;
	}

	cutil_list_destroy(&list);
}


// Popping an item from the back removes the item
void list_multiple_pop_back_removes_item() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int list_size = 10;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(&list, &i);
	}

	cutil_list_pop_back(&list);
	int back_node_value = *((int *)list.back->data);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, list_size - 1);
	CUTIL_TESTING_ASSERT_INT_EQ(back_node_value, list_size - 2);

	cutil_list_destroy(&list);
}

// Popping an item from the front removes the item
void list_multiple_pop_front_removes_item() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int list_size = 10;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(&list, &i);
	}

	cutil_list_pop_front(&list);
	int front_node_value = *((int *)list.front->data);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, list_size - 1);
	CUTIL_TESTING_ASSERT_INT_EQ(front_node_value, 1);

	cutil_list_destroy(&list);
}

// Popping an item from the back updates node pointers
void list_pop_back_updates_node_pointers() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int list_size = 10;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(&list, &i);
	}

	cutil_list_node* expected_back = list.back->prev;

	cutil_list_pop_back(&list);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back->next);
	CUTIL_TESTING_ASSERT_PTR_EQ(list.back, expected_back);

	cutil_list_destroy(&list);
}

// Popping an item from the front updates node pointers
void list_pop_front_updates_node_pointers() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int list_size = 10;

	for (int i = 0; i < list_size; i++) {
		cutil_list_push_back(&list, &i);
	}

	cutil_list_node* expected_front = list.front->next;

	cutil_list_pop_front(&list);
	CUTIL_TESTING_ASSERT_PTR_EQ(list.front, expected_front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front->prev);

	cutil_list_destroy(&list);
}


// Popping the only item from the front correctly sets list pointers
void list_pop_back_one_item() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int val = 55;
	cutil_list_push_back(&list, &val);

	cutil_list_pop_back(&list);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);

	cutil_list_destroy(&list);
}

// Popping the only item from the back correctly sets list pointers
void list_pop_front_one_item() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	int val = 55;
	cutil_list_push_back(&list, &val);

	cutil_list_pop_front(&list);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);

	cutil_list_destroy(&list);
}

// Popping the front of an empty list does nothing
void list_pop_front_empty() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	cutil_list_pop_front(&list);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);

	cutil_list_destroy(&list);
}

// Popping the back of an empty list does nothing
void list_pop_back_empty() {
	cutil_list list;
	cutil_list_init(&list, sizeof(int));

	cutil_list_pop_back(&list);

	CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.front);
	CUTIL_TESTING_ASSERT_PTR_NULL(list.back);

	cutil_list_destroy(&list);
}

// Pushing a pointer to the front and retrieving it functions correctly
void push_get_front_pointer() {
	cutil_list list;
	cutil_list_initp(&list);

	int *iptr = malloc(sizeof(int));
	*iptr = 55;

	cutil_list_push_frontp(&list, iptr);

	int *frontptr;
	cutil_list_get_frontp(&list, &frontptr);

	CUTIL_TESTING_ASSERT_PTR_EQ(frontptr, iptr);

	cutil_list_destroy(&list);
	free(iptr);
}

// Pushing a pointer to the back and retrieving it functions correctly
void push_get_back_pointer() {
	cutil_list list;
	cutil_list_initp(&list);

	int *iptr = malloc(sizeof(int));
	*iptr = 55;
	cutil_list_push_backp(&list, iptr);

	int *backptr;
	cutil_list_get_backp(&list, &backptr);

	CUTIL_TESTING_ASSERT_PTR_EQ(backptr, iptr);

	cutil_list_destroy(&list);
	free(iptr);
}

// Pushing data to the front and retrieving it functions correctly
void push_get_front_data() {
	cutil_list list;
	cutil_list_init(&list, sizeof(float));

	float fval = 3.14f;
	float front_val;

	cutil_list_push_front(&list, &fval);
	cutil_list_get_front(&list, &front_val);

	CUTIL_TESTING_ASSERT_FLOAT_EQ(front_val, fval);

	cutil_list_destroy(&list);
}

// Pushing data to the back and retrieving it functions correctly
void push_get_back_data() {
	cutil_list list;
	cutil_list_init(&list, sizeof(float));

	float fval = 3.14f;
	float back_val;

	cutil_list_push_back(&list, &fval);
	cutil_list_get_back(&list, &back_val);

	CUTIL_TESTING_ASSERT_FLOAT_EQ(back_val, fval);

	cutil_list_destroy(&list);
}

void add_list_tests(){
    cutil_testing_suite("List");

	CUTIL_TESTING_ADD(list_init_size_0);
	CUTIL_TESTING_ADD(list_init_null_pointers);

	CUTIL_TESTING_ADD(list_clear_size_0);
	CUTIL_TESTING_ADD(list_clear_null_pointers);
	CUTIL_TESTING_ADD(list_clear_empty);


	CUTIL_TESTING_ADD(list_push_data_front_size);
	CUTIL_TESTING_ADD(list_push_data_back_size);

	CUTIL_TESTING_ADD(list_push_data_one_front_list_pointers);
	CUTIL_TESTING_ADD(list_push_data_one_back_list_pointers);

	CUTIL_TESTING_ADD(list_data_push_multiple_back);
	CUTIL_TESTING_ADD(list_data_push_multiple_front);

	CUTIL_TESTING_ADD(list_multiple_pop_back_removes_item);
	CUTIL_TESTING_ADD(list_multiple_pop_front_removes_item);

	CUTIL_TESTING_ADD(list_pop_back_updates_node_pointers);
	CUTIL_TESTING_ADD(list_pop_front_updates_node_pointers);

	CUTIL_TESTING_ADD(list_pop_back_one_item);
	CUTIL_TESTING_ADD(list_pop_front_one_item);

	CUTIL_TESTING_ADD(list_pop_front_empty);
	CUTIL_TESTING_ADD(list_pop_back_empty);

	CUTIL_TESTING_ADD(push_get_front_data);
	CUTIL_TESTING_ADD(push_get_back_data);

	CUTIL_TESTING_ADD(push_get_front_pointer);
	CUTIL_TESTING_ADD(push_get_back_pointer);
}