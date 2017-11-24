#include "test_suites.h"

#include "testing.h"

int main(int argc, char** argv){
    cutil_testing_init();

    add_vector_tests();
    add_list_tests();
	add_list_itr_tests();
	add_btree_tests();
	add_btree_itr_tests();

	int result = 0;

	if (argc > 1) {
		result = cutil_testing_run_suites(argv[1]);
	}
	else {
		cutil_testing_run_all();
	}
    
    cutil_testing_destroy();

    return result;
}