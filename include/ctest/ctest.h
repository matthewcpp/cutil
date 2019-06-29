#ifndef CTEST_CTEST_H
#define CTEST_CTEST_H

typedef void(*cutil_test_function)();

void ctest_init();
void ctest_destroy();

void ctest_suite(const char *name);
int ctest_suite_before_each(cutil_test_function func);
int ctest_suite_after_each(cutil_test_function func);
void ctest_config_set_filter(const char *filter_str);
int ctest_run();

void _ctest_add_test(const char *test_name, cutil_test_function test_func);

int _ctest_predicate_true(const char *exppression_str, int result);
int _ctest_predicate_false(const char *exppression_str, int result);
int _ctest_predicate_int_eq(const char *exppression_str, int expected, int result);
int _ctest_predicate_int_gt(const char *exppression_str, int expected, int result);
int _ctest_predicate_int_lt(const char *exppression_str, int expected, int result);
int _ctest_predicate_float_eq(const char *exppression_str, float expected, float result);
int _ctest_predicate_ptr_eq(const char *exppression_str, void* expected, void* result);
int _ctest_predicate_ptr_neq(const char *ptr1_str, void* ptr1, void* ptr2);
int _ctest_predicate_ptr_null(const char *exppression_str, void* ptr);
int _ctest_predicate_ptr_not_null(const char *exppression_str, void* ptr);

#define CTEST_ADD_TEST(FUNC) \
	_ctest_add_test(#FUNC, &FUNC);

#define CTEST_EXPECT_TRUE(EXP) \
	_ctest_predicate_true(#EXP, (EXP));

#define CTEST_EXPECT_FALSE(EXP) \
	_ctest_predicate_false(#EXP, (EXP));

#define CTEST_EXPECT_INT_EQ(EXP, VAL) \
	_ctest_predicate_int_eq(#EXP, (int)VAL, (int)(EXP));

#define CTEST_EXPECT_PTR_EQ(EXP, VAL) \
	_ctest_predicate_ptr_eq(#EXP, VAL, (EXP));

#define CTEST_EXPECT_PTR_NEQ(PTR1, PTR2) \
	_ctest_predicate_ptr_neq(#PTR1, (PTR1), (PTR2));

#define CTEST_EXPECT_PTR_NULL(EXP) \
	_ctest_predicate_ptr_null(#EXP, (EXP))

#define CTEST_EXPECT_PTR_NOT_NULL(EXP) \
	_ctest_predicate_ptr_not_null(#EXP, (EXP))


#define CTEST_ASSERT_TRUE(EXP) \
	if (_ctest_predicate_true(#EXP, (EXP))) return;

#define CTEST_ASSERT_FALSE(EXP) \
	if (_ctest_predicate_false(#EXP, (EXP))) return;

#define CTEST_ASSERT_INT_EQ(EXP, VAL) \
	if (_ctest_predicate_int_eq(#EXP, (int)VAL, (int)(EXP))) return;

#define CTEST_ASSERT_INT_LT(EXP, VAL) \
	if (_ctest_predicate_int_lt(#EXP, (int)VAL, (int)(EXP))) return;

#define CTEST_ASSERT_INT_GT(EXP, VAL) \
	if (_ctest_predicate_int_gt(#EXP, (int)VAL, (int)(EXP))) return;

#define CTEST_ASSERT_FLOAT_EQ(EXP, VAL) \
	if (_ctest_predicate_float_eq(#EXP, VAL, (EXP))) return;

#define CTEST_ASSERT_PTR_EQ(EXP, VAL) \
	if (_ctest_predicate_ptr_eq(#EXP, VAL, (EXP))) return;

#define CTEST_ASSERT_PTR_NEQ(PTR1, PTR2) \
	if (_ctest_predicate_ptr_neq(#PTR1, (PTR1), (PTR2))) return;

#define CTEST_ASSERT_PTR_NULL(EXP) \
	if (_ctest_predicate_ptr_null(#EXP, (EXP))) return;

#define CTEST_ASSERT_PTR_NOT_NULL(EXP) \
	if (_ctest_predicate_ptr_not_null(#EXP, (EXP))) return;

#endif
