set(cutil_sources
	../include/vector.h vector.c
	../include/list.h list.c
	../include/btree.h btree.c
)

add_library (cutil ${cutil_sources})

set(testing_sources
	../include/testing.h
    testing.c
)

add_library (cutil_testing ${testing_sources})
target_link_libraries(cutil_testing cutil)

if ( CMAKE_COMPILER_IS_GNUCC )
	if (COVERAGE)
		target_compile_options(cutil PRIVATE --coverage)
		target_link_libraries(cutil PRIVATE --coverage)
	endif()
endif()
