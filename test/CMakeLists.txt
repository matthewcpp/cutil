FetchContent_Declare(
		ctest
		GIT_REPOSITORY https://github.com/matthewcpp/ctest.git
		GIT_TAG        origin/master
)

# This is required to support CMake < 3.14
FetchContent_GetProperties(ctest)
if(NOT ctest_POPULATED)
	FetchContent_Populate(ctest)
	add_subdirectory(${ctest_SOURCE_DIR} ${ctest_BINARY_DIR})
endif()

include (CTest)

set(testing_sources
        test_main.c test_suites.h
		test_settings.h test_settings.c
		test_vector.c 
        test_heap.c test_heap_util.h test_heap_util.c 
		test_list.c test_list_itr.c
		test_btree_fixtures.h test_btree_fixtures.c
		test_btree.c test_btree_itr.c test_btree_util.h test_btree_util.c
        test_traits.c
        test_util/defs.h
		test_util/trait_tracker.h test_util/trait_tracker.c
	)

add_executable(cutil_test ${testing_sources})
set_compiler_options(cutil_test)

target_link_libraries(cutil_test cutil ctest)
target_include_directories(cutil_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(test_data_dir ${CMAKE_CURRENT_SOURCE_DIR}/btree_data)

add_test (NAME test_list COMMAND cutil_test "--cutil-test-filter" "list" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_vector COMMAND cutil_test "--cutil-test-filter" "vector" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_btree COMMAND cutil_test "--cutil-test-filter" "btree" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_heap COMMAND cutil_test "--cutil-test-filter" "heap" "--cutil-test-data-dir" ${test_data_dir})
add_test (NAME test_traits COMMAND cutil_test "--cutil-test-filter" "trait" "--cutil-test-data-dir" ${test_data_dir})

