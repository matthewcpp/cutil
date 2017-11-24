cmake_minimum_required (VERSION 3.0)
project (cutil C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCUTIL_DEBUGGING")


include_directories(include)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_subdirectory (src)
add_subdirectory (test)
