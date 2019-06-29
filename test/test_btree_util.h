#ifndef CUTIL_TEST_BTREE_UTIL_H
#define CUTIL_TEST_BTREE_UTIL_H

#include "cutil/btree.h"

#include <stdio.h>

/* 
Note: snprintf is not available in MSVC prior to Visual Studio 2015.
In this case we will use the Microsoft extension of the function.
Although the behavior of this method is not exactly the same as the standardized version, it is acceptable behavior for our use cases here.
*/
#if defined(_MSC_VER) && _MSC_VER < 1900
	#define cutil_snprintf_func _snprintf
#else
	#define cutil_snprintf_func snprintf
#endif

typedef int (*cutil_btree_value_xform_func)(int val);


int read_btree_from_file(cutil_btree* btree, const char* test_data_name);
int insert_char_sequence(cutil_btree *btree, const char* sequence, cutil_btree_value_xform_func xform_func);
int forward_itr_char_sequence(cutil_btree* btree, const char* expected_sequence, int* error_index);
void dump_btree(cutil_btree* btree, const char* path);

int validate_btree(cutil_btree* btree);

#endif
