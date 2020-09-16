cmake_minimum_required(VERSION 3.11)

SET(COVERAGE OFF CACHE BOOL "Enable Code Coverage")
SET(ENABLE_TESTING ON CACHE BOOL "Generate Test Projects")

include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/util_functions.cmake)
include(FetchContent)

project (cutil C)

if ( CMAKE_COMPILER_IS_GNUCC )
    if (COVERAGE)
        message(STATUS "Enabling support for code coverage.")
    endif()
endif()

add_subdirectory (src)

if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory (test)
endif()
