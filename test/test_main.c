#include "test_suites.h"

#include "testing.h"

int main(int argc, char** argv){
    cutil_testing_init();

    add_vector_tests();
    add_list_tests();

    int result = cutil_testing_run_all();

    cutil_testing_destroy();

    return result;
}