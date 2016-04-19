include_directories(../include ${CMAKE_BINARY_DIR}/flinkinterface/)

set(FLINK_SRCS base.c lowlevel.c error.c valid.c subdevtypes.c info.c ain.c aout.c counter.c dio.c pwm.c wd.c ppwa.c)



add_library(flink SHARED ${FLINK_SRCS})
add_library(flink_static ${FLINK_SRCS})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/libflink.so ${CMAKE_CURRENT_BINARY_DIR}/libflink_static.a DESTINATION lib)

add_dependencies(flink subdevtypes flinkioctl_cmd flink_funcid)
add_dependencies(flink_static subdevtypes flinkioctl_cmd flink_funcid)

