cmake_minimum_required (VERSION 3.0)
project (cutil C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCUTIL_DEBUGGING")

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Werror")
	set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} -Wall -Werror")
endif()

if ( MSVC )
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} /W4 /WX")
	set(CMAKE_C_FLAGS_DEBUG  "${CMAKE_C_FLAGS_DEBUG} /W4 /WX")
endif()

include_directories(include)

add_subdirectory (src)
add_subdirectory (test)
