#include "test_suites.h"

#include "ctest/ctest.h"
#include "test_settings.h"

int main(int argc, char** argv) {
    int result = 0;
    const char* filter_string;

    cutil_test_settings_init();
    if (!cutil_test_settings_parse(argc, (const char**)argv)) {
        cutil_test_settings_destroy();
        return 1;
    }

    ctest_init();

    add_vector_tests();
    add_list_tests();
    add_list_itr_tests();
    add_btree_tests();
    add_btree_itr_tests();
    add_trait_tests();
    add_heap_tests();
    add_default_allocator_tests();

    filter_string = cutil_test_get_filter_string();

    if (filter_string) {
        ctest_config_set_filter(filter_string);
    }

    result = ctest_run();

    ctest_destroy();
    cutil_test_settings_destroy();

    return result;
}
