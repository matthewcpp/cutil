set(cutil_sources
    ../include/cutil/allocator.h allocator_private.h allocator.c
    ../include/cutil/trait.h trait.c
    ../include/cutil/vector.h vector_private.h vector.c
    ../include/cutil/forward_list.h forward_list.c
    ../include/cutil/list.h list.c
    ../include/cutil/heap.h heap_private.h heap.c
    ../include/cutil/btree.h btree_private.h btree.c btree_itr.c
    defs_private.h
)

add_library (cutil ${cutil_sources})
set_compiler_options(cutil)
target_include_directories(cutil PUBLIC ../include)

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
