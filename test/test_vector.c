#include "test_suites.h"

#include "testing.h"
#include "vector.h"

// Initializing Vector sets size to 0
void vector_init_size_0(){
    cutil_vector vector;
    cutil_vector_init(&vector, sizeof(int));

    CUTIL_TESTING_ASSERT_INT_EQ(vector._size, 0);
}

void add_vector_tests(){
    cutil_testing_suite("Vector");

	CUTIL_TESTING_ADD(vector_init_size_0);
}