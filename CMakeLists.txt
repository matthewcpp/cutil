cmake_minimum_required (VERSION 3.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
SET(COVERAGE OFF CACHE BOOL "Coverage")

project (cutil C)

if ( CMAKE_COMPILER_IS_GNUCC )
	if (COVERAGE)
		message(STATUS "Enabling support for code coverage.")
	endif()
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCUTIL_DEBUGGING")

if ( MSVC )
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /W4 /WX")
	set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} /W4 /WX")
else()
	set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Werror -pedantic")
	set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror -pedantic")
endif()

enable_testing()

include_directories(include)

add_subdirectory (src)
add_subdirectory (test)
