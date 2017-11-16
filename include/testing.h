#pragma once
#include <stdbool.h>

#include "vector.h"

typedef void(*cutil_test_function)(void);

void cutil_test_init();

void cutil_testing_add(const char *test_name, cutil_test_function test_func);
void cutil_testing_suite(const char *name);
int cutil_testing_run_all();
void cutil_testing_destroy();

int _cutil_testing_assert_ieq(const char *exppression_str, int expected, int result);

#define cutil_TEST_ASSERT_INT_EQ(EXP, VAL) \
	if (_cutil_test_assert_ieq(#EXP, VAL, (EXP))) return;
