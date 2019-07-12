#ifndef CUTIL_TEST_DEFS_H
#define CUTIL_TEST_DEFS_H

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

#endif
