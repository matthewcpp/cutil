set(cutil_sources
    vector.c
)

add_library (cutil ${cutil_sources})

set(testing_sources
    testing.c
)

add_library (cutil_testing ${testing_sources})
target_link_libraries(cutil_testing cutil)
