set(cutil_sources
    ../include/trait.h trait.c
	../include/vector.h vector.c
	../include/list.h list.c
	../include/btree.h btree_private.h btree.c
)

add_library (cutil ${cutil_sources})

if (NOT MSVC)
set(cutil_public_link_libs m)

if ( CMAKE_COMPILER_IS_GNUCC AND COVERAGE)
		target_compile_options(cutil PRIVATE --coverage)
        set(cutil_private_link_libs --coverage)
endif()

target_link_libraries(cutil 
    PUBLIC ${cutil_public_link_libs}
    PRIVATE ${cutil_private_link_libs}
)
endif()




set(testing_sources
	../include/testing.h
    testing.c
)

add_library (cutil_testing ${testing_sources})
target_link_libraries(cutil_testing cutil)
