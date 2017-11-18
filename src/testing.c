#include "testing.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct _cutil_test_entry {
	cutil_test_function test_func;
	char *test_name;
	unsigned int test_result;
	char *test_message;
} _cutil_test_entry;

typedef struct _cutil_test_suite {
	char* name;
	cutil_vector _tests;
} _cutil_test_suite;

typedef struct _cutil_test_system {
	cutil_vector _suites;
	_cutil_test_entry *_current_test;
	_cutil_test_suite *_current_suite;
} _cutil_test_system;

_cutil_test_system* test_system = NULL;

void _cutil_testing_system_init(void);
void _cutil_testing_system_destroy(void);
_cutil_test_suite *_cutil_testing_suite_create(const char *name);
void _cutil_testing_suite_destroy(_cutil_test_suite *test_suite);
_cutil_test_entry *_cutil_testing_entry_create(const char *test_name, cutil_test_function test_func);
void _cutil_testing_entry_destroy(_cutil_test_entry *test_entry);
char *_str_cpy(const char *src);
int _cutil_run_test_suites(cutil_vector *test_suites);
_cutil_test_suite *_cutil_get_test_suite(const char *name);

void _cutil_testing_system_init(void) {
	cutil_vector_initp(&test_system->_suites);
	test_system->_current_test = NULL;
	test_system->_current_suite = NULL;
}

void _cutil_testing_system_destroy(void) {
	_cutil_test_suite *suite;
	for (unsigned int i = 0; i < test_system->_suites.size; i++) {
		cutil_vector_getp(&test_system->_suites, i, (void**)&suite);
        _cutil_testing_suite_destroy(suite);
		free(suite);
	}
	cutil_vector_destroy(&test_system->_suites);
}

_cutil_test_suite *_cutil_testing_suite_create(const char *name) {
	_cutil_test_suite * suite = malloc(sizeof(_cutil_test_suite));
	suite->name = _str_cpy(name);
	cutil_vector_initp(&suite->_tests);

	return suite;
}
void _cutil_testing_suite_destroy(_cutil_test_suite *test_suite) {
	if (test_suite->name) {
		free(test_suite->name);
	}

	_cutil_test_entry *test_entry;
	for (unsigned int i = 0; i < test_suite->_tests.size; i++) {
		cutil_vector_getp(&test_suite->_tests, i, (void**)&test_entry);
        _cutil_testing_entry_destroy(test_entry);
        free(test_entry);
	}
}

_cutil_test_entry *_cutil_testing_entry_create(const char *test_name, cutil_test_function test_func) {
	_cutil_test_entry *test_entry = malloc(sizeof(_cutil_test_entry));

	test_entry->test_func = test_func;
	test_entry->test_name = _str_cpy(test_name);
	test_entry->test_result = 0;
	test_entry->test_message = NULL;

	return test_entry;
}

void _cutil_testing_entry_destroy(_cutil_test_entry *test_entry) {
	if (test_entry->test_name) {
		free(test_entry->test_name);
	}

	if (test_entry->test_message) {
		free(test_entry->test_message);
	}
}

void cutil_testing_init() {
	if (!test_system) {
		test_system = malloc(sizeof(_cutil_test_system));
		_cutil_testing_system_init();
	}
}

void cutil_testing_destroy() {
	if (test_system) {
        _cutil_testing_system_destroy();
		free(test_system);
		test_system = NULL;
	}
}

void cutil_testing_suite(const char *name) {
    cutil_testing_init();

	_cutil_test_suite * new_suite = _cutil_testing_suite_create(name);
	cutil_vector_pushp(&test_system->_suites, new_suite);

	test_system->_current_suite = new_suite;
}

void cutil_testing_add(const char *test_name, cutil_test_function test_func) {
    cutil_testing_init();

	if (!test_system->_current_suite) {
		cutil_testing_suite("Default");
	}

	_cutil_test_entry *test_entry = _cutil_testing_entry_create(test_name, test_func);
	cutil_vector_pushp(&test_system->_current_suite->_tests, test_entry);
}

int cutil_testing_run_suites(const char* suite_list) {
	cutil_vector suites;
	cutil_vector_initp(&suites);

	size_t len = strlen(suite_list);
	size_t start = 0;
	char suite_name[256];
	

	for (size_t i = 0; i < len; i++) {
		if (suite_list[i] == ';') {
			size_t str_size = min(i - start, 255);

			memset(suite_name, 0, 256);
			strncpy(suite_name, suite_list + start, str_size);

			start = i + 1;

			_cutil_test_suite *test_suite = _cutil_get_test_suite(suite_name);

			if (test_suite) {
				cutil_vector_pushp(&suites, test_suite);
			}
		}
	}

	if (start < len) {
		size_t str_size = min(len - start, 255);

		memset(suite_name, 0, 256);
		strncpy(suite_name, suite_list + start, str_size);

		_cutil_test_suite *test_suite = _cutil_get_test_suite(suite_name);

		if (test_suite) {
			cutil_vector_pushp(&suites, test_suite);
		}
	}


	int result = _cutil_run_test_suites(&suites);

	cutil_vector_destroy(&suites);

	return result;
}

int _cutil_testing_process_suite(_cutil_test_suite *current_suite, int* out_pass_count, int* out_fail_count) {
	int test_pass_count = 0;
	int test_fail_count = 0;
	_cutil_test_entry *current_test;

	printf("Test Suite: %s\n", current_suite->name);
	printf("-----------------------------------\n");

	for (unsigned int t = 0; t < current_suite->_tests.size; t++) {
		cutil_vector_getp(&current_suite->_tests, t, (void**)&current_test);
		test_system->_current_test = current_test;

		printf("Test: %s\n", current_test->test_name);
		current_test->test_func();

		if (current_test->test_result == 0) {
			printf("Result: PASSED\n");
			test_pass_count += 1;
		}
		else {
			printf("Result: FAILED\n");
			test_fail_count += 1;
		}
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

int _cutil_run_test_suites(cutil_vector *test_suites) {
	int total_pass_count = 0;
	int total_fail_count = 0;
	_cutil_test_suite *current_suite;

	for (unsigned int i = 0; i < test_suites->size; i++) {
		cutil_vector_getp(test_suites, i, (void**)&current_suite);

		int suite_pass_count = 0;
		int suite_fail_count = 0;

		_cutil_testing_process_suite(current_suite, &suite_pass_count, &suite_fail_count);

		total_pass_count += suite_pass_count;
		total_fail_count += suite_fail_count;
	}

	_cutil_print_results(total_pass_count, total_fail_count);

	if (total_fail_count > 0)
		return 1;
	else
		return 0;
}

int cutil_testing_run_all() {
	return _cutil_run_test_suites(&test_system->_suites);
}

_cutil_test_suite *_cutil_get_test_suite(const char *name) {
	_cutil_test_suite *test_suite = NULL;

	for (unsigned int i = 0; i < test_system->_suites.size; i++) {
		cutil_vector_getp(&test_system->_suites, i, (void**)&test_suite);

		if (strcmp(test_suite->name, name) == 0) {
			return test_suite;
		}
	}

	return NULL;
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

char *_str_cpy(const char *src) {
	size_t len = strlen(src) + 1;
	char *s = malloc(len);
	if (s == NULL)
		return NULL;
	return (char *)memcpy(s, src, len);
}
