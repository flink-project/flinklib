include_directories(../include ${CMAKE_BINARY_DIR}/flinkinterface/)

set(FLINK_SRCS base.c lowlevel.c error.c valid.c subdevtypes.c info.c ain.c aout.c counter.c dio.c pwm.c wd.c ppwa.c)



add_library(flink SHARED ${FLINK_SRCS})
add_library(flink_static ${FLINK_SRCS})

add_dependencies(flink subdevtypes)
add_dependencies(flink_static subdevtypes)

