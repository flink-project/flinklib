include_directories(../include ${CMAKE_BINARY_DIR}/flinkinterface/)

add_executable(lsflink lsflink.c)
target_link_libraries(lsflink flink)

add_executable(flinkinfo flinkinfo.c)
target_link_libraries(flinkinfo flink)

add_executable(flinkanaloginput flinkanaloginput.c)
target_link_libraries(flinkanaloginput flink)

add_executable(flinkanalogoutput flinkanalogoutput.c)
target_link_libraries(flinkanalogoutput flink)

add_executable(flinkdio flinkdio.c)
target_link_libraries(flinkdio flink)

add_executable(flinkpwm flinkpwm.c)
target_link_libraries(flinkpwm flink)

add_executable(flinkcounter flinkcounter.c)
target_link_libraries(flinkcounter flink)

add_executable(flinkwd flinkwd.c)
target_link_libraries(flinkwd flink)

add_executable(flinkppwa flinkppwa.c)
target_link_libraries(flinkppwa flink)

add_executable(flinksteppermotor flinksteppermotor.c)
target_link_libraries(flinksteppermotor flink m)

add_executable(flinkreflectivesensoren flinkreflectivesensoren.c)
target_link_libraries(flinkreflectivesensoren flink)

add_executable(flinkinterrupthandler flinkinterrupthandler.c)
target_link_libraries(flinkinterrupthandler flink)

add_executable(flinkinterruptmultiplexer flinkinterruptmultiplexer.c)
target_link_libraries(flinkinterruptmultiplexer flink)

INSTALL(
	TARGETS
		lsflink
		flinkinfo
		flinkanaloginput
		flinkanalogoutput
		flinkdio
		flinkpwm
		flinkcounter
		flinkwd
		flinkppwa
		flinkreflectivesensoren
		flinksteppermotor
		flinkinterrupthandler
		flinkinterruptmultiplexer
	RUNTIME
	DESTINATION bin
)

