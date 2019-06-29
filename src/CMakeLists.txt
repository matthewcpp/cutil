set(cutil_sources
    ../include/cutil/trait.h trait.c
	../include/cutil/vector.h vector.c
	../include/cutil/list.h list.c
	../include/cutil/btree.h btree_private.h btree.c btree_itr.c
)

add_library (cutil ${cutil_sources})
set_compiler_options(cutil)

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
	../include/ctest/ctest.h
    ctest.c
)

add_library (cutil_testing ${testing_sources})
set_compiler_options(cutil_testing)

target_link_libraries(cutil_testing cutil)

