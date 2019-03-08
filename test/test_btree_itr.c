#include "btree.h"

#include "testing.h"
#include "test_suites.h"

#include "test_btree_util.h"

#include <stdlib.h>
#include <string.h>

extern cutil_btree* g_btree;
cutil_btree_itr* g_btree_itr = NULL;

void btree_itr_after_each() {
	if (g_btree_itr) {
		cutil_btree_itr_destroy(g_btree_itr);
		g_btree_itr = NULL;
	}

	if (g_btree) {
		cutil_btree_destroy(g_btree);
		g_btree = NULL;
	}

	cutil_trait_destroy();
}

/* ensures that the forward iterator visits nodes in the expected order */
int insert_seq_mult_10(int key) {
	return key * 10;
}

void btree_itr_forward_pod() {
	const char* insert_sequence = "QWERTYUIOPASDFGHJKLZXCVBNM";
	const char* expected_sequence = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int expected_sequence_length = strlen(expected_sequence);
	int i = 0;

	g_btree = cutil_btree_create(5, cutil_trait_int(), cutil_trait_int());
	CUTIL_TESTING_ASSERT_TRUE(insert_char_sequence(g_btree, insert_sequence, insert_seq_mult_10));
	CUTIL_TESTING_ASSERT_INT_EQ(expected_sequence_length, cutil_btree_size(g_btree));

	g_btree_itr = cutil_btree_itr_create(g_btree);

	while (cutil_btree_itr_next(g_btree_itr)) {
		int expected_key = (int)expected_sequence[i];
		int expected_value = insert_seq_mult_10(expected_key);
		int actual_key = 0, actual_value = 0;

		cutil_btree_itr_get_key(g_btree_itr, &actual_key);
		cutil_btree_itr_get_value(g_btree_itr, &actual_value);

		CUTIL_TESTING_ASSERT_INT_EQ(expected_key, actual_key);
		CUTIL_TESTING_ASSERT_INT_EQ(expected_value, actual_value);

		i += 1;
	}
}

void btree_itr_forward_cstring() {
    char** test_keys = malloc(26 * sizeof(char*));
    char** test_values = malloc(26 * sizeof(char*));
    char c;
    int i = 0;

    g_btree = cutil_btree_create(5, cutil_trait_cstring(), cutil_trait_cstring());

    for (c = 'A'; c <= 'Z'; c++) {
        char* test_key = malloc(16);
        char* test_value = malloc(16);

        cutil_snprintf_func(test_key, 16, "key %c", c);
        cutil_snprintf_func(test_value, 16, "value %c", c);

        test_keys[i] = test_key;
        test_values[i] = test_value;
        i++;
    }

    for (i = 25; i >= 0; i--) {
        cutil_btree_insert(g_btree, &test_keys[i], &test_values[i]);
    }

    i = 0;
    g_btree_itr = cutil_btree_itr_create(g_btree);
    while (cutil_btree_itr_next(g_btree_itr)) {
        char* actual_value = NULL;
        cutil_btree_itr_get_value(g_btree_itr, &actual_value);
        i += 1;
    }


    for (i = 0; i < 26; i++) {
        free(test_keys[i]);
        free(test_values[i]);
    }

    free(test_keys);
    free(test_values);
}

void add_btree_itr_tests() {
	cutil_testing_suite("btree_itr");
	cutil_testing_suite_after_each(&btree_itr_after_each);

	CUTIL_TESTING_ADD(btree_itr_forward_pod);
    CUTIL_TESTING_ADD(btree_itr_forward_cstring);
}
