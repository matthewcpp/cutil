#include "cutil/heap.h"
#include "test_heap_util.h"

#include "ctest/ctest.h"
#include "test_util/defs.h"
#include "test_util/trait_tracker.h"

#include <string.h>

typedef struct {
	cutil_heap* heap;
} heap_test;

void heap_test_setup(heap_test* test){
	test->heap = NULL;
}

void heap_test_teardown(heap_test* test) {
	if (test->heap) {
		cutil_heap_destroy(test->heap);
	}

	cutil_trait_destroy();
}

typedef struct {
	cutil_heap* heap;
	cutil_trait* trait_tracker;
} heap_trait_func_test;

void heap_trait_func_test_setup(heap_trait_func_test* test) {
	memset(test, 0, sizeof(heap_trait_func_test));
}

void heap_trait_func_test_teardown(heap_trait_func_test* test) {
	if (test->heap) {
		cutil_heap_destroy(test->heap);
	}

	if (test->trait_tracker) {
		cutil_test_destroy_trait_tracker(test->trait_tracker);
	}

	cutil_trait_destroy();
}

CTEST_FIXTURE(heap, heap_test, heap_test_setup, heap_test_teardown)
CTEST_FIXTURE(heap_trait_func, heap_trait_func_test, heap_trait_func_test_setup, heap_trait_func_test_teardown)

/* using a trait with no comparison function will result in no heap being created */
void create_invalid_trait(heap_test* test) {
	cutil_trait invalid_trait;
	memcpy(&invalid_trait, cutil_trait_int(), sizeof(cutil_trait));
	invalid_trait.compare_func = NULL;

	test->heap = cutil_heap_create(&invalid_trait);

	CTEST_ASSERT_PTR_NULL(test->heap);
}

void create_size_0(heap_test* test) {
	test->heap = cutil_heap_create(cutil_trait_int());

	CTEST_ASSERT_INT_EQ(cutil_heap_size(test->heap), 0);
}

void insert_valid_heap(heap_test* test) {
    int nums[] = {19, 100, 17, 36, 55, 2};
    int i;

	test->heap = cutil_heap_create(cutil_trait_int());

	for (i = 0; i < 6; i++) {
		cutil_heap_insert(test->heap, &nums[i]);
		CTEST_ASSERT_TRUE(validate_heap(test->heap));
	}
}

void insert_increase_size(heap_test* test) {
    int item_count = 10;
    int i;
    test->heap = cutil_heap_create(cutil_trait_int());

    for(i = 0; i < item_count; i++) {
        cutil_heap_insert(test->heap, &i);
    }

    CTEST_ASSERT_INT_EQ(cutil_heap_size(test->heap), item_count);
}

void peek_empty_heap(heap_test* test) {
    int actual_value;
    test->heap = cutil_heap_create(cutil_trait_int());

    CTEST_ASSERT_FALSE(cutil_heap_peek(test->heap, &actual_value));
}

void peek_valid_items(heap_test* test) {
    int value1 = 50, value2 = 13, value3 = 20;
    int actual_val;

    test->heap = cutil_heap_create(cutil_trait_int());
    cutil_heap_insert(test->heap, &value1);
    CTEST_ASSERT_TRUE(cutil_heap_peek(test->heap, &actual_val));
    CTEST_ASSERT_INT_EQ(value1, actual_val);

    cutil_heap_insert(test->heap, &value2);
    CTEST_ASSERT_TRUE(cutil_heap_peek(test->heap, &actual_val));
    CTEST_ASSERT_INT_EQ(value2, actual_val);

    cutil_heap_insert(test->heap, &value3);
    CTEST_ASSERT_TRUE(cutil_heap_peek(test->heap, &actual_val));
    CTEST_ASSERT_INT_EQ(value2, actual_val);
    
}

void get_heap_trait(heap_test* test) {
	cutil_trait* trait = cutil_trait_int();
	test->heap = cutil_heap_create(trait);

	CTEST_ASSERT_PTR_EQ(cutil_heap_trait(test->heap), trait);
}

void remove_empty_heap(heap_test* test) {
	test->heap = cutil_heap_create(cutil_trait_int());
	CTEST_ASSERT_FALSE(cutil_heap_pop(test->heap));
}

void remove_one_item(heap_test* test) {
	int val = 5566;

	test->heap = cutil_heap_create(cutil_trait_int());
	cutil_heap_insert(test->heap, &val);

	CTEST_ASSERT_TRUE(cutil_heap_pop(test->heap));
	CTEST_ASSERT_INT_EQ(cutil_heap_size(test->heap), 0);
}

void pop_all(heap_test* test) {
	int i, item_count = 5, pop_count = 0;

	test->heap = cutil_heap_create(cutil_trait_int());

	for (i = 0; i < item_count; i++) {
		cutil_heap_insert(test->heap, &i);
	}

	while (cutil_heap_size(test->heap) > 0) {
		cutil_heap_pop(test->heap);
		pop_count += 1;
	}

	CTEST_ASSERT_INT_EQ(pop_count, item_count);
}

/* pops a heap that causes the new root to trickle down to the left child */
void trickle_down_left(heap_test* test) {
	int start_vals[] = {7, 10, 20, 31, 22, 35};
	int expected_vals[] = { 10, 22, 20, 31, 35 };
    int heaps_equal;

	cutil_heap* expected = create_heap_from_array(expected_vals, 5);
	test->heap = create_heap_from_array(start_vals, 6);

	cutil_heap_pop(test->heap);

	heaps_equal = heaps_are_equal(expected, test->heap);
	cutil_heap_destroy(expected);

	CTEST_ASSERT_TRUE(heaps_equal);
}

/* pops a heap that causes the new root to trickle down to the right child */
void trickle_down_right(heap_test* test) {
    int start_vals[] = {7, 30, 20, 33, 34, 25, 26};
    int expected_vals[] = { 20, 30, 25, 33, 34, 26 };
    int heaps_equal;

    cutil_heap* expected = create_heap_from_array(expected_vals, 6);
    test->heap = create_heap_from_array(start_vals, 7);

    cutil_heap_pop(test->heap);

    heaps_equal = heaps_are_equal(expected, test->heap);
    cutil_heap_destroy(expected);

    CTEST_ASSERT_TRUE(heaps_equal);
}

void reset_heap(heap_test* test) {
	int i;

	test->heap = cutil_heap_create(cutil_trait_int());

	for (i = 0; i < 0; i++) {
		cutil_heap_insert(test->heap, &i);
	}

	cutil_heap_reset(test->heap);
	CTEST_ASSERT_INT_EQ(cutil_heap_size(test->heap), 0);
}

void clear_heap(heap_test* test) {
	int i;

	test->heap = cutil_heap_create(cutil_trait_int());

	for (i = 0; i < 0; i++) {
		cutil_heap_insert(test->heap, &i);
	}

	cutil_heap_clear(test->heap);
	CTEST_ASSERT_INT_EQ(cutil_heap_size(test->heap), 0);
}

void insert_calls_copy_constructor(heap_trait_func_test* test) {
    int expected_copy_count = 1;
    char* test_str = "test";

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->heap = cutil_heap_create(test->trait_tracker);

    cutil_heap_insert(test->heap, &test_str);

    CTEST_ASSERT_INT_EQ(expected_copy_count, cutil_test_trait_tracker_copy_count(test->trait_tracker));
}


void pop_calls_destructor(heap_trait_func_test* test) {
    int expected_delete_count = 1;
    char* test_str = "test";

    test->trait_tracker = cutil_test_create_trait_tracker(cutil_trait_cstring());
    test->heap = cutil_heap_create(test->trait_tracker);

    cutil_heap_insert(test->heap, &test_str);
    cutil_heap_pop(test->heap);

    CTEST_ASSERT_INT_EQ(expected_delete_count, cutil_test_trait_tracker_destroy_count(test->trait_tracker));
}

void add_heap_tests() {
	CTEST_ADD_TEST_F(heap, create_invalid_trait);
	CTEST_ADD_TEST_F(heap, create_size_0);

	CTEST_ADD_TEST_F(heap, insert_valid_heap);
	CTEST_ADD_TEST_F(heap, insert_increase_size);

    CTEST_ADD_TEST_F(heap, peek_empty_heap);
    CTEST_ADD_TEST_F(heap, peek_valid_items);

	CTEST_ADD_TEST_F(heap, get_heap_trait);

	CTEST_ADD_TEST_F(heap, reset_heap);
	CTEST_ADD_TEST_F(heap, clear_heap);

	CTEST_ADD_TEST_F(heap, remove_empty_heap);
	CTEST_ADD_TEST_F(heap, remove_one_item);
	CTEST_ADD_TEST_F(heap, pop_all);
	CTEST_ADD_TEST_F(heap, trickle_down_left);
    CTEST_ADD_TEST_F(heap, trickle_down_right);

    CTEST_ADD_TEST_F(heap_trait_func, insert_calls_copy_constructor);
    CTEST_ADD_TEST_F(heap_trait_func, pop_calls_destructor);
}
