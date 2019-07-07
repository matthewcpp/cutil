set(testing_sources
        test_main.c test_suites.h
		test_settings.h test_settings.c
		test_vector.c 
		test_list.c test_list_itr.c
		test_btree.c test_btree_itr.c test_btree_util.h test_btree_util.c
        test_traits.c
		test_util/trait_tracker.h test_util/trait_tracker.c
	)

add_executable(cutil_test ${testing_sources})
set_compiler_options(cutil_test)

target_link_libraries(cutil_test cutil_testing)
target_include_directories(cutil_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(test_data_dir ${CMAKE_CURRENT_SOURCE_DIR}/btree_data)

add_test (NAME test_list COMMAND cutil_test "--cutil-test-filter" "list;list_itr" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_vector COMMAND cutil_test "--cutil-test-filter" "vector;vectorp;vector-trait" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_btree COMMAND cutil_test "--cutil-test-filter" "btree_insert_odd_order;btree_insert_even_order;btree_get;btree_delete;btree_equals;btree_traits;btree_itr" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_traits COMMAND cutil_test "--cutil-test-filter" "traits" "--cutil-test-data-dir" ${test_data_dir})
