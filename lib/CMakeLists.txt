target_sources(${PROJECT_NAME} PRIVATE
  base.c lowlevel.c error.c valid.c subdevtypes.c info.c ain.c aout.c
  counter.c dio.c pwm.c wd.c ppwa.c stepperMotor.c reflectiveSensor.c interrupt.c)

add_dependencies(flink subdevtypes flinkioctl_cmd flink_funcid)
