#include "test_suites.h"

#include "testing.h"
#include "vector.h"

void vector_init_size_0(){
    cutil_vector vector;
    cutil_vector_init(&vector, sizeof(int));

    CUTIL_TESTING_ASSERT_INT_EQ(vector.size, 1);
}

void add_vector_tests(){
    cutil_testing_add("Initializing Vector sets size to 0", &vector_init_size_0);
}