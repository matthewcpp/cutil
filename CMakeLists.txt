cmake_minimum_required(VERSION 3.11)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
SET(COVERAGE OFF CACHE BOOL "Coverage")

include (${CMAKE_CURRENT_SOURCE_DIR}/cmake/util_functions.cmake)

project (cutil C)

if ( CMAKE_COMPILER_IS_GNUCC )
	if (COVERAGE)
		message(STATUS "Enabling support for code coverage.")
	endif()
endif()

include_directories(include)

add_subdirectory (src)

enable_testing()
add_subdirectory (test)
