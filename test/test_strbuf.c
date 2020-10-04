#include "cutil/strbuf.h"
#include "ctest/ctest.h"

#include <string.h>

typedef struct {
    cutil_strbuf* strbuf;
} strbuf_test;

void strbuf_test_setup(strbuf_test* test) {
    test->strbuf = NULL;
}

void strbuf_test_teardown(strbuf_test* test) {
    if (test->strbuf) {
        cutil_strbuf_destroy(test->strbuf);
    }
}

CTEST_FIXTURE(strbuf, strbuf_test, strbuf_test_setup, strbuf_test_teardown)

static void create_empty(strbuf_test* test) {
    size_t actual_length;
    int compare_result;

    test->strbuf = cutil_strbuf_create();

    actual_length = cutil_strbuf_length(test->strbuf);
    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), "");

    CTEST_ASSERT_INT_EQ(actual_length, 0);
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void create_with_string(strbuf_test* test) {
    const char* source_string = "hello world";
    const char* expected_string = "hello";
    size_t actual_length;
    int compare_result;

    test->strbuf = cutil_strbuf_create_with_str(source_string, 5);
    actual_length = cutil_strbuf_length(test->strbuf);
    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), expected_string);

    CTEST_ASSERT_INT_EQ(strlen(expected_string), actual_length);
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void append_chars(strbuf_test* test) {
    const char* expected_string = "abcd";
    size_t actual_length;
    int compare_result;

    test->strbuf = cutil_strbuf_create();

    cutil_strbuf_append_char(test->strbuf, 'a');
    cutil_strbuf_append_char(test->strbuf, 'b');
    cutil_strbuf_append_char(test->strbuf, 'c');
    cutil_strbuf_append_char(test->strbuf, 'd');

    actual_length = cutil_strbuf_length(test->strbuf);
    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), expected_string);

    CTEST_ASSERT_INT_EQ(strlen(expected_string), actual_length);
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void append_cstrings(strbuf_test* test) {
    const char* expected_string = "helloworld";
    int compare_result;

    test->strbuf = cutil_strbuf_create();
    cutil_strbuf_append_cstring(test->strbuf, "hello");
    cutil_strbuf_append_cstring(test->strbuf, "world");

    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), expected_string);
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void append_mixed(strbuf_test* test) {
    const char* expected_string = "hello world";
    int compare_result;

    test->strbuf = cutil_strbuf_create();
    cutil_strbuf_append_cstring(test->strbuf, "hello");
    cutil_strbuf_append_char(test->strbuf, ' ');
    cutil_strbuf_append_cstring(test->strbuf, "world");

    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), expected_string);
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void clear_resets_size(strbuf_test* test) {
    size_t actual_length;

    test->strbuf = cutil_strbuf_create_with_cstring("hello");
    actual_length = cutil_strbuf_length(test->strbuf);
    CTEST_ASSERT_INT_EQ(actual_length, 5);

    cutil_strbuf_clear(test->strbuf);
    actual_length = cutil_strbuf_length(test->strbuf);
    CTEST_ASSERT_INT_EQ(actual_length, 0);
}

static void clear_resets_cstring(strbuf_test* test) {
    const char* test_string = "hello";
    int compare_result;

    test->strbuf = cutil_strbuf_create_with_cstring(test_string);
    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), test_string);
    CTEST_ASSERT_INT_EQ(compare_result, 0);

    cutil_strbuf_clear(test->strbuf);
    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), "");
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void clear_empty_buffer(strbuf_test* test) {
    size_t actual_length;
    int compare_result;

    test->strbuf = cutil_strbuf_create();
    cutil_strbuf_clear(test->strbuf);

    actual_length = cutil_strbuf_length(test->strbuf);
    compare_result = strcmp(cutil_strbuf_cstring(test->strbuf), "");

    CTEST_ASSERT_INT_EQ(actual_length, 0);
    CTEST_ASSERT_INT_EQ(compare_result, 0);
}

static void pop_back_empty_buffer(strbuf_test* test) {
    test->strbuf = cutil_strbuf_create();
    CTEST_ASSERT_FALSE(cutil_strbuf_pop_back(test->strbuf));
}

static void pop_back_removes_last_char(strbuf_test* test) {
    const char* expected_str = "tes";
    int result;

    test->strbuf = cutil_strbuf_create_with_cstring("test");

    result = cutil_strbuf_pop_back(test->strbuf);
    CTEST_ASSERT_TRUE(result);

    result = strcmp(expected_str, cutil_strbuf_cstring(test->strbuf));
    CTEST_ASSERT_INT_EQ(result, 0);
}

static void set_invalid_char(strbuf_test* test) {
    const char* test_str = "test";
    int result;

    test->strbuf = cutil_strbuf_create_with_cstring(test_str);

    result = cutil_strbuf_set_char(test->strbuf, 25, '!');
    CTEST_ASSERT_FALSE(result);

    result = strcmp(test_str, cutil_strbuf_cstring(test->strbuf));
    CTEST_ASSERT_INT_EQ(result, 0);
}

static void set_char_at_valid_index(strbuf_test* test) {
    const char* expected_str = "Test";
    int result;

    test->strbuf = cutil_strbuf_create_with_cstring("test");

    result = cutil_strbuf_set_char(test->strbuf, 0, 'T');
    CTEST_ASSERT_TRUE(result);

    result = strcmp(expected_str, cutil_strbuf_cstring(test->strbuf));
    CTEST_ASSERT_INT_EQ(result, 0);
}

void add_strbuf_tests() {
    CTEST_ADD_TEST_F(strbuf, create_empty);
    CTEST_ADD_TEST_F(strbuf, create_with_string);
    CTEST_ADD_TEST_F(strbuf, append_chars);
    CTEST_ADD_TEST_F(strbuf, append_cstrings);
    CTEST_ADD_TEST_F(strbuf, append_mixed);
    CTEST_ADD_TEST_F(strbuf, clear_resets_size);
    CTEST_ADD_TEST_F(strbuf, clear_resets_cstring);
    CTEST_ADD_TEST_F(strbuf, clear_empty_buffer);
    CTEST_ADD_TEST_F(strbuf, pop_back_empty_buffer);
    CTEST_ADD_TEST_F(strbuf, pop_back_removes_last_char);
    CTEST_ADD_TEST_F(strbuf, set_invalid_char);
    CTEST_ADD_TEST_F(strbuf, set_char_at_valid_index);
}
