#pragma once
#include <stdbool.h>

#include "vector.h"

typedef void(*cutil_test_function)();

void cutil_testing_init();

void cutil_testing_add(const char *test_name, cutil_test_function test_func);
void cutil_testing_suite(const char *name);
int cutil_testing_run_all();
int cutil_testing_run_suites(const char* suite_list);
void cutil_testing_destroy();

int _cutil_testing_assert_int_eq(const char *exppression_str, int expected, int result);
int _cutil_testing_assert_ptr_eq(const char *exppression_str, void* expected, void* result);

#define CUTIL_TESTING_ASSERT_INT_EQ(EXP, VAL) \
	if (_cutil_testing_assert_int_eq(#EXP, VAL, (EXP))) return;

#define CUTIL_TESTING_ASSERT_PTR_EQ(EXP, VAL) \
	if (_cutil_testing_assert_ptr_eq(#EXP, VAL, (EXP))) return;
