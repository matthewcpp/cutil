#include "ctest/testing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*******************************************/
/* Various utility functions. */
char *_str_cpy(const char *src) {
	size_t len = strlen(src) + 1;
	char *s = malloc(len);
	if (s == NULL)
		return NULL;
	return (char *)memcpy(s, src, len);
}

int _cutil_testing_string_cmp(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

/*******************************************/
/* _cutil_test_entry describes a single unit test. */
typedef struct _cutil_test_entry {
	cutil_test_function test_func;
	char *test_name;
	unsigned int test_result;
	char *test_message;

	struct _cutil_test_entry* next;
} _cutil_test_entry;

_cutil_test_entry *_cutil_testing_entry_create(const char *test_name, cutil_test_function test_func) {
	_cutil_test_entry *test_entry = malloc(sizeof(_cutil_test_entry));

	test_entry->test_func = test_func;
	test_entry->test_name = _str_cpy(test_name);
	test_entry->test_result = 0;
	test_entry->test_message = NULL;
	test_entry->next = NULL;

	return test_entry;
}

void _cutil_testing_entry_destroy(_cutil_test_entry *test_entry) {
	if (test_entry->test_name) {
		free(test_entry->test_name);
	}

	if (test_entry->test_message) {
		free(test_entry->test_message);
	}

	free(test_entry);
}

/*******************************************/
/* _cutil_test_suite describes a collection of related unit tests. */
typedef struct _cutil_test_suite {
	char* name;
	cutil_test_function before_each;
	cutil_test_function after_each;
	_cutil_test_entry* _tests;

	struct _cutil_test_suite* next;
} _cutil_test_suite;

_cutil_test_suite *_cutil_testing_suite_create(const char *name) {
	_cutil_test_suite * suite = malloc(sizeof(_cutil_test_suite));

	suite->name = _str_cpy(name);
	suite->_tests = NULL;
	suite->before_each = NULL;
	suite->after_each = NULL;
	suite->next = NULL;
	return suite;
}
void _cutil_testing_suite_destroy(_cutil_test_suite *test_suite) {
	_cutil_test_entry *current_test = test_suite->_tests;
	_cutil_test_entry *free_ptr = NULL;

	if (test_suite->name) {
		free(test_suite->name);
	}

	while (current_test) {
		free_ptr = current_test;
		current_test = current_test->next;
		_cutil_testing_entry_destroy(free_ptr);
	}

	free(test_suite);
}

/*******************************************/
/* _cutil_test_suite top level object containing a collection of suites. */
typedef struct _cutil_test_system {
	_cutil_test_suite *_suites;
	_cutil_test_entry *_current_test;
	_cutil_test_suite *_current_suite;
	
	char** test_filters;
	int test_filter_count;
} _cutil_test_system;

_cutil_test_system* test_system = NULL;

void _cutil_testing_system_init(void) {
	test_system->_suites = NULL;
	test_system->_current_test = NULL;
	test_system->_current_suite = NULL;

	test_system->test_filters = NULL;
	test_system->test_filter_count = 0;
}

void _cutil_testing_system_destroy(void) {
	_cutil_test_suite *current_suite = test_system->_suites;
	_cutil_test_suite *free_ptr = NULL;

	if (test_system->test_filters) {
		int i;
		for (i = 0; i < test_system->test_filter_count; i++) {
			free(test_system->test_filters[i]);
		}

		free(test_system->test_filters);
	}

	while (current_suite) {
		free_ptr = current_suite;
		current_suite = current_suite->next;
		_cutil_testing_suite_destroy(free_ptr);
	}

	free(test_system);
}

/*******************************************/
/* Implementation for main testing interface. */

void cutil_testing_init() {
	if (!test_system) {
		test_system = malloc(sizeof(_cutil_test_system));
		_cutil_testing_system_init();
	}
}

void cutil_testing_destroy() {
	if (test_system) {
        _cutil_testing_system_destroy();
		test_system = NULL;
	}
}

void cutil_testing_suite(const char *name) {
	_cutil_test_suite* new_suite;

	cutil_testing_init();

	new_suite = _cutil_testing_suite_create(name);
	if (test_system->_current_suite) {
		test_system->_current_suite->next = new_suite;
	}
	else {
		test_system->_suites = new_suite;
	}

	test_system->_current_suite = new_suite;
	test_system->_current_test = NULL;
}

void _cutil_testing_add(const char *test_name, cutil_test_function test_func) {
	_cutil_test_entry* test_entry;
    cutil_testing_init();

	if (!test_system->_current_suite) {
		cutil_testing_suite("Default");
	}

	test_entry = _cutil_testing_entry_create(test_name, test_func);
	if (test_system->_current_test) {
		test_system->_current_test->next = test_entry;
	}
	else {
		test_system->_current_suite->_tests = test_entry;
	}

	test_system->_current_test = test_entry;
}

int cutil_testing_suite_before_each(cutil_test_function func) {
	if (test_system->_current_suite) {
		test_system->_current_suite->before_each = func;

		return 1;
	}
	else {
		return 0;
	}
}

int cutil_testing_suite_after_each(cutil_test_function func) {
	if (test_system->_current_suite) {
		test_system->_current_suite->after_each = func;

		return 1;
	}
	else {
		return 0;
	}
}

char** _cutil_testing_get_suite_names(const char* str, int* count) {
	int token_count = 0;
	int token_size = 1;

	char** tokens = malloc(sizeof(char*) * token_size);
	char* search_str = _str_cpy(str);

	char *token = strtok(search_str, ";");
	while (token) {
		tokens[token_count] = _str_cpy(token);

		token_count += 1;

		if (token_count == token_size) {
			token_size *= 2;
			tokens = realloc(tokens, sizeof(char*) * token_size);
		}

		token = strtok(NULL, ";");
	}

	free(search_str);

	qsort(tokens, token_count, sizeof(char*), _cutil_testing_string_cmp);

	*count = token_count;
	return tokens;
}

void cutil_testing_set_filter(const char* filter_str)
{
	test_system->test_filters = _cutil_testing_get_suite_names(filter_str, &test_system->test_filter_count);
}

int _cutil_testing_str_find(const char* needle, char** haystack, int start, int end) {
	int middle = start + ((end - start) / 2);
	int result = strcmp(needle, haystack[middle]);

	if (end <= start) {
		if (result == 0) {
			return 1;
		}
		else {
			return 0;
		}
	}

	if (result > 0) {
		return _cutil_testing_str_find(needle, haystack, middle + 1, end);
	}
	else if (result < 0) {
		return _cutil_testing_str_find(needle, haystack, start, middle - 1);
	}
	else {
		return 1;
	}
}

int _cutil_testing_should_run_test_suite(_cutil_test_suite * test_suite) {
	if (test_system->test_filters) {
		return _cutil_testing_str_find(test_suite->name, test_system->test_filters, 0, test_system->test_filter_count - 1);
	}
	else {
		return 1;
	}
}

int _cutil_testing_process_suite(_cutil_test_suite *current_suite, int* out_pass_count, int* out_fail_count) {
	int test_pass_count = 0;
	int test_fail_count = 0;

	printf("Test Suite: %s\n", current_suite->name);
	printf("-----------------------------------\n");

	test_system->_current_test = current_suite->_tests;
	while (test_system->_current_test) {
		printf("Test: %s\n", test_system->_current_test->test_name);
		if (current_suite->before_each) {
			current_suite->before_each();
		}

		test_system->_current_test->test_func();

		if (current_suite->after_each) {
			current_suite->after_each();
		}

		if (test_system->_current_test->test_result == 0) {
			printf("Result: PASSED\n");
			test_pass_count += 1;
		}
		else {
			printf("Result: FAILED\n");
			test_fail_count += 1;
		}

		test_system->_current_test = test_system->_current_test->next;
	}

	*out_pass_count = test_pass_count;
	*out_fail_count = test_fail_count;

	if (test_fail_count > 0)
		return 1;
	else
		return 0;
}

void _cutil_print_results(int total_pass_count, int total_fail_count) {
	printf("\n-----------------------------------\n");
	printf("TestResults:\n");
	printf("Total Tests: %d\n", total_pass_count + total_fail_count);
	printf("Tests Passed: %d\n", total_pass_count);
	printf("Tests Failed: %d\n", total_fail_count);
}

int cutil_testing_run() {
	int total_pass_count = 0;
	int total_fail_count = 0;

	test_system->_current_suite = test_system->_suites;

	while (test_system->_current_suite) {
		int suite_pass_count = 0;
		int suite_fail_count = 0;

		if (_cutil_testing_should_run_test_suite(test_system->_current_suite)) {
			_cutil_testing_process_suite(test_system->_current_suite, &suite_pass_count, &suite_fail_count);
		}

		total_pass_count += suite_pass_count;
		total_fail_count += suite_fail_count;

		test_system->_current_suite = test_system->_current_suite->next;
	}

	_cutil_print_results(total_pass_count, total_fail_count);

	if (total_fail_count > 0)
		return 1;
	else
		return 0;
}

int _cutil_testing_assert_true(const char *exppression_str, int result) {
	if (!result) {
		printf("Assertion: Expected: %s to be true\n", exppression_str);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_false(const char *exppression_str, int result) {
	if (result) {
		printf("Assertion: Expected: %s to be false\n", exppression_str);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_int_eq(const char *exppression_str, int expected, int result) {
	if (expected != result) {
		printf("Assertion: %s Expected: %i Actual: %i\n", exppression_str, expected, result);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_int_lt(const char *exppression_str, int expected, int result) {
	if (result >= expected) {
		printf("Assertion: %s < %i. Actual: %i\n", exppression_str, expected, result);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_int_gt(const char *exppression_str, int expected, int result) {
	if (result <= expected) {
		printf("Assertion: %s > %i. Actual: %i\n", exppression_str, expected, result);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_float_eq(const char *exppression_str, float expected, float result) {
	if (expected != result) {
		printf("Assertion: %s Expected: %f Actual: %f\n", exppression_str, expected, result);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_ptr_eq(const char *exppression_str, void* expected, void* result) {
	if (expected != result) {
		printf("Assertion: %s Expected: %p Actual: %p\n", exppression_str, expected, result);
		test_system->_current_test->test_result = 1;
		return 1;
	}
	else {
		return 0;
	}
}

int _cutil_testing_assert_ptr_null(const char *exppression_str, void* ptr) {
	if (ptr != NULL) {
		printf("Assertion: %s Expected: NULL Actual: %p\n", exppression_str, ptr);
		test_system->_current_test->test_result = 1;
		return 1;
	}

	return 0;
}

int _cutil_testing_assert_ptr_not_null(const char *exppression_str, void* ptr) {
	if (ptr == NULL) {
		printf("Assertion:  Expected: %s to be non-NULL\n", exppression_str);
		test_system->_current_test->test_result = 1;
		return 1;
	}

	return 0;
}

int _cutil_testing_assert_ptr_not_eq(const char *ptr1_str, void* ptr1, void* ptr2) {
	if (ptr1 == ptr2) {
		printf("Assertion:  Expected: %s to not equal %p\n", ptr1_str, ptr2);
		test_system->_current_test->test_result = 1;
		return 1;
	}

	return 0;
}
