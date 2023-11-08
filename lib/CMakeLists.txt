include_directories(../include ${CMAKE_BINARY_DIR}/flinkinterface/)

set(FLINK_SRCS base.c lowlevel.c error.c valid.c subdevtypes.c info.c ain.c aout.c counter.c dio.c pwm.c wd.c ppwa.c reflectiveSensoren.c interrupt.c)

add_library(flink SHARED ${FLINK_SRCS})
add_library(flink_static ${FLINK_SRCS})

set_target_properties(flink PROPERTIES VERSION ${LIBRARY_VERSION})

INSTALL(TARGETS flink flink_static LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_dependencies(flink subdevtypes flinkioctl_cmd flink_funcid)
add_dependencies(flink_static subdevtypes flinkioctl_cmd flink_funcid)

