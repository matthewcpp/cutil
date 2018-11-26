set(testing_sources
        test_main.c test_suites.h
		test_settings.h test_settings.c
		test_vector.c 
		test_list.c test_list_itr.c
		test_btree.c test_btree_itr.c test_btree_util.h test_btree_util.c
	)

add_executable(cutil_test ${testing_sources})
target_link_libraries(cutil_test cutil_testing)

if ( CMAKE_COMPILER_IS_GNUCC )
    target_link_libraries(cutil_test m)
endif()

set(test_data_dir ${CMAKE_CURRENT_SOURCE_DIR}/btree_data)

add_test (NAME test_list COMMAND cutil_test "--cutil-test-filter" "list;list_itr" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_vector COMMAND cutil_test "--cutil-test-filter" "vector" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_btree COMMAND cutil_test "--cutil-test-filter" "--test-filter btree;btree_itr;btree_even_order;btree_delete" "--cutil-test-data-dir" ${test_data_dir})
