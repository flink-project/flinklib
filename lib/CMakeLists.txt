include_directories(../include)

set(FLINK_SRCS base.c lowlevel.c error.c valid.c subdevtypes.c ain.c counter.c dio.c pwm.c wd.c)

add_library(flink SHARED ${FLINK_SRCS})
add_library(flink_static ${FLINK_SRCS})

INSTALL(TARGETS flink LIBRARY DESTINATION lib)
