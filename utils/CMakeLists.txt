include_directories(../include)

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
	RUNTIME
	DESTINATION bin
)

