#ifndef CUTIL_TESTING_H
#define CUTIL_TESTING_H

typedef void(*cutil_test_function)();

void cutil_testing_init();

void cutil_testing_suite(const char *name);
int cutil_testing_suite_before_each(cutil_test_function func);
int cutil_testing_suite_after_each(cutil_test_function func);
void cutil_testing_set_filter(const char* filter_str);
int cutil_testing_run();
void cutil_testing_destroy();

void _cutil_testing_add(const char *test_name, cutil_test_function test_func);

int _cutil_testing_assert_true(const char *exppression_str, int result);
int _cutil_testing_assert_false(const char *exppression_str, int result);
int _cutil_testing_assert_int_eq(const char *exppression_str, int expected, int result);
int _cutil_testing_assert_int_gt(const char *exppression_str, int expected, int result);
int _cutil_testing_assert_int_lt(const char *exppression_str, int expected, int result);
int _cutil_testing_assert_float_eq(const char *exppression_str, float expected, float result);
int _cutil_testing_assert_ptr_eq(const char *exppression_str, void* expected, void* result);
int _cutil_testing_assert_ptr_not_eq(const char *ptr1_str, void* ptr1, void* ptr2);
int _cutil_testing_assert_ptr_null(const char *exppression_str, void* ptr);
int _cutil_testing_assert_ptr_not_null(const char *exppression_str, void* ptr);

#define CUTIL_TESTING_ADD(FUNC) \
	_cutil_testing_add(#FUNC, &FUNC);

#define CUTIL_TESTING_EXPECT_TRUE(EXP) \
	_cutil_testing_assert_true(#EXP, (EXP));

#define CUTIL_TESTING_ASSERT_TRUE(EXP) \
	if (_cutil_testing_assert_true(#EXP, (EXP))) return;

#define CUTIL_TESTING_EXPECT_FALSE(EXP) \
	_cutil_testing_assert_false(#EXP, (EXP));

#define CUTIL_TESTING_ASSERT_FALSE(EXP) \
	if (_cutil_testing_assert_false(#EXP, (EXP))) return;

#define CUTIL_TESTING_EXPECT_INT_EQ(EXP, VAL) \
	_cutil_testing_assert_int_eq(#EXP, (int)VAL, (int)(EXP));

#define CUTIL_TESTING_ASSERT_INT_EQ(EXP, VAL) \
	if (_cutil_testing_assert_int_eq(#EXP, (int)VAL, (int)(EXP))) return;

#define CUTIL_TESTING_ASSERT_INT_LT(EXP, VAL) \
	if (_cutil_testing_assert_int_lt(#EXP, (int)VAL, (int)(EXP))) return;

#define CUTIL_TESTING_ASSERT_INT_GT(EXP, VAL) \
	if (_cutil_testing_assert_int_gt(#EXP, (int)VAL, (int)(EXP))) return;


#define CUTIL_TESTING_ASSERT_FLOAT_EQ(EXP, VAL) \
	if (_cutil_testing_assert_float_eq(#EXP, VAL, (EXP))) return;

#define CUTIL_TESTING_ASSERT_PTR_EQ(EXP, VAL) \
	if (_cutil_testing_assert_ptr_eq(#EXP, VAL, (EXP))) return;

#define CUTIL_TESTING_EXPECT_PTR_EQ(EXP, VAL) \
	_cutil_testing_assert_ptr_eq(#EXP, VAL, (EXP));

#define CUTIL_TESTING_ASSERT_PTR_NOT_EQ(PTR1, PTR2) \
	if (_cutil_testing_assert_ptr_not_eq(#PTR1, (PTR1), (PTR2))) return;

#define CUTIL_TESTING_EXPECT_PTR_NOT_EQ(PTR1, PTR2) \
	_cutil_testing_assert_ptr_not_eq(#PTR1, (PTR1), (PTR2));

#define CUTIL_TESTING_EXPECT_PTR_NULL(EXP) \
	_cutil_testing_assert_ptr_null(#EXP, (EXP))

#define CUTIL_TESTING_ASSERT_PTR_NULL(EXP) \
	if (_cutil_testing_assert_ptr_null(#EXP, (EXP))) return;

#define CUTIL_TESTING_EXPECT_PTR_NOT_NULL(EXP) \
	_cutil_testing_assert_ptr_not_null(#EXP, (EXP))

#define CUTIL_TESTING_ASSERT_PTR_NOT_NULL(EXP) \
	if (_cutil_testing_assert_ptr_not_null(#EXP, (EXP))) return;

#endif