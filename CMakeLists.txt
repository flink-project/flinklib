cmake_minimum_required(VERSION 2.6)
project(flinklib)

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "-std=gnu99")
endif()

add_subdirectory(lib)
add_subdirectory(utils)
add_subdirectory(test)
