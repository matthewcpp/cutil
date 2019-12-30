#include "cutil/btree.h"

#include "ctest/ctest.h"
#include "test_suites.h"

#include "test_util/defs.h"
#include "test_btree_util.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    cutil_btree* btree;
    cutil_btree_itr* itr;
} btree_itr_test;

void btree_itr_test_startup(btree_itr_test* test) {
    memset(test, 0, sizeof(btree_itr_test));
}

void btree_itr_test_teardown(btree_itr_test* test) {
    if (test->itr) {
        cutil_btree_itr_destroy(test->itr);
    }

    if (test->btree) {
        cutil_btree_destroy(test->btree);
    }

    cutil_trait_destroy();
}

typedef struct {
    btree_itr_test base;

    int* int_values;
    int value_count;
} btree_itr_ptr_test;

void btree_itr_ptr_test_setup(btree_itr_ptr_test* test) {
    memset(test, 0, sizeof(btree_itr_ptr_test));
}

void btree_itr_ptr_test_teardown(btree_itr_ptr_test* test) {
    if (test->int_values) {
        free(test->int_values);
    }

    btree_itr_test_teardown(&test->base);
}

typedef struct {
    btree_itr_test base;

    char** keys;
    char** values;
    int string_count;
} btree_itr_cstring_test;

void btree_itr_cstring_test_setup(btree_itr_cstring_test* test) {
    memset(test, 0, sizeof(btree_itr_cstring_test));
}

void btree_itr_cstring_test_teardown(btree_itr_cstring_test* test) {
    CTEST_UTIL_FREE_PTR_ARRAY(test->keys, test->string_count)
    CTEST_UTIL_FREE_PTR_ARRAY(test->values, test->string_count)

    btree_itr_test_teardown(&test->base);
}


CTEST_FIXTURE(btree_itr, btree_itr_test, btree_itr_test_startup, btree_itr_test_teardown)
CTEST_FIXTURE(btree_itr_ptr, btree_itr_ptr_test, btree_itr_ptr_test_setup, btree_itr_ptr_test_teardown)
CTEST_FIXTURE(btree_itr_cstring, btree_itr_cstring_test, btree_itr_cstring_test_setup, btree_itr_cstring_test_teardown)

void forward_empty(btree_itr_test* test)
{
    int visitied_nodes = 0;

    test->btree = cutil_btree_create(5, cutil_trait_int(), cutil_trait_int());
    test->itr = cutil_btree_itr_create(test->btree);

    while (cutil_btree_itr_next(test->itr)) {
        visitied_nodes = 1;
    }

    CTEST_ASSERT_FALSE(visitied_nodes);
}

/* ensures that the forward iterator visits nodes in the expected order */
int insert_seq_mult_10(int key) {
    return key * 10;
}

void forward_pod(btree_itr_test* test) {
    const char* insert_sequence = "QWERTYUIOPASDFGHJKLZXCVBNM";
    const char* expected_sequence = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t expected_sequence_length = strlen(expected_sequence);
    int i = 0;

    test->btree = cutil_btree_create(5, cutil_trait_int(), cutil_trait_int());
    CTEST_ASSERT_TRUE(insert_char_sequence(test->btree, insert_sequence, insert_seq_mult_10));
    CTEST_ASSERT_INT_EQ(expected_sequence_length, cutil_btree_size(test->btree));

    test->itr = cutil_btree_itr_create(test->btree);

    while (cutil_btree_itr_next(test->itr)) {
        int expected_key = (int)expected_sequence[i];
        int expected_value = insert_seq_mult_10(expected_key);
        int actual_key = 0, actual_value = 0;

        cutil_btree_itr_get_key(test->itr, &actual_key);
        cutil_btree_itr_get_value(test->itr, &actual_value);

        CTEST_ASSERT_INT_EQ(expected_key, actual_key);
        CTEST_ASSERT_INT_EQ(expected_value, actual_value);

        i += 1;
    }
}

void forward_cstring(btree_itr_cstring_test* test) {
    char c;
    int i = 0;

    test->string_count = 26;
    test->keys = malloc(26 * sizeof(char*));
    test->values = malloc(26 * sizeof(char*));


    test->base.btree = cutil_btree_create(5, cutil_trait_cstring(), cutil_trait_cstring());

    for (c = 'A'; c <= 'Z'; c++) {
        char* test_key = malloc(16);
        char* test_value = malloc(16);

        cutil_snprintf_func(test_key, 16, "key %c", c);
        cutil_snprintf_func(test_value, 16, "value %c", c);

        test->keys[i] = test_key;
        test->values[i] = test_value;
        i++;
    }

    for (i = 25; i >= 0; i--) {
        cutil_btree_insert(test->base.btree, &test->keys[i], &test->values[i]);
    }

    i = 0;
    test->base.itr = cutil_btree_itr_create(test->base.btree);
    while (cutil_btree_itr_next(test->base.itr)) {
        char* actual_key = NULL;
        char* actual_value = NULL;

        CTEST_ASSERT_TRUE(cutil_btree_itr_get_key(test->base.itr, &actual_key));
        CTEST_ASSERT_TRUE(cutil_btree_itr_get_value(test->base.itr, &actual_value));

        CTEST_ASSERT_INT_EQ(strcmp(test->keys[i], actual_key), 0);
        CTEST_ASSERT_INT_EQ(strcmp(test->values[i], actual_value), 0);
        i += 1;
    }
}

void forward_ptr(btree_itr_ptr_test* test) {
    int i = 0;

    test->value_count = 10;
    test->int_values = malloc(sizeof(int) * test->value_count);
    for (i = 0; i < test->value_count; i++) {
        test->int_values[i] = i * 10;
    }

    test->base.btree = cutil_btree_create(5, cutil_trait_int(), cutil_trait_ptr());

    for (i = 0; i < test->value_count; i++) {
        int* value = test->int_values + i;
        cutil_btree_insert(test->base.btree, &i, &value);
    }

    test->base.itr = cutil_btree_itr_create(test->base.btree);

    i = 0;
    while (cutil_btree_itr_next(test->base.itr)) {
        int key;
        int* value;

        CTEST_ASSERT_TRUE(cutil_btree_itr_get_key(test->base.itr, &key));
        CTEST_ASSERT_TRUE(cutil_btree_itr_get_value(test->base.itr, &value));
        CTEST_ASSERT_INT_EQ(key, i);
        CTEST_ASSERT_INT_EQ(*value, test->int_values[i]);

        i += 1;
    }
}

void add_btree_itr_tests() {
    CTEST_ADD_TEST_F(btree_itr, forward_empty);
    CTEST_ADD_TEST_F(btree_itr, forward_pod);
    CTEST_ADD_TEST_F(btree_itr_cstring, forward_cstring);
    CTEST_ADD_TEST_F(btree_itr_ptr, forward_ptr);
}
