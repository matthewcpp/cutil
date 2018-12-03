#include "test_suites.h"

#include "testing.h"
#include "test_settings.h"

int main(int argc, char** argv) {
	cutil_test_settings_init();
	if (!cutil_test_settings_parse(argc, (const char**)argv)) {
		cutil_test_settings_destroy();
		return 1;
	}

    cutil_testing_init();

    add_vector_tests();
    add_list_tests();
	add_list_itr_tests();
	add_btree_tests();
	add_btree_itr_tests();

	int result = 0;
	const char* filter_string = cutil_test_get_filter_string();

	if (filter_string) {
		cutil_testing_set_filter(filter_string);
	}

	result = cutil_testing_run();

    
    cutil_testing_destroy();
	cutil_test_settings_destroy();

    return result;
}
