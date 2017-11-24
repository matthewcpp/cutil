set(testing_sources
        test_main.c test_suites.h 
		test_vector.c 
		test_list.c test_list_itr.c
		test_btree.c test_btree_itr.c test_btree_util.h test_btree_util.c
	)

add_executable(cutil_test ${testing_sources})
target_link_libraries(cutil_test cutil_testing)
