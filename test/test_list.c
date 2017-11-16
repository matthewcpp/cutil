#include "test_suites.h"

#include "testing.h"
#include "list.h"

void list_init_size_0(){
    cutil_list list;
    cutil_list_init(&list, sizeof(int));

    CUTIL_TESTING_ASSERT_INT_EQ(list.size, 0);
}

void add_list_tests(){
    cutil_testing_suite("List");

    cutil_testing_add("Initializing List sets size to 0", &list_init_size_0);
}