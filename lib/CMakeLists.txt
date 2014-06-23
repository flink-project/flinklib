## Compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "-std=c99")
endif()

include_directories(../include)

set(FLINK_SRCS flink.c error.c)

add_library(flink SHARED ${FLINK_SRCS})
add_library(flink_static ${FLINK_SRCS})