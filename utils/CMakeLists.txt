add_executable(lsflink lsflink.c)
target_link_libraries(lsflink PRIVATE ${PROJECT_NAME})

add_executable(flinkinfo flinkinfo.c)
target_link_libraries(flinkinfo PRIVATE ${PROJECT_NAME})

add_executable(flinkanaloginput flinkanaloginput.c)
target_link_libraries(flinkanaloginput PRIVATE ${PROJECT_NAME})

add_executable(flinkanalogoutput flinkanalogoutput.c)
target_link_libraries(flinkanalogoutput PRIVATE ${PROJECT_NAME})

add_executable(flinkdio flinkdio.c)
target_link_libraries(flinkdio PRIVATE ${PROJECT_NAME})

add_executable(flinkpwm flinkpwm.c)
target_link_libraries(flinkpwm PRIVATE ${PROJECT_NAME})

add_executable(flinkcounter flinkcounter.c)
target_link_libraries(flinkcounter PRIVATE ${PROJECT_NAME})

add_executable(flinkwd flinkwd.c)
target_link_libraries(flinkwd PRIVATE ${PROJECT_NAME})

add_executable(flinkppwa flinkppwa.c)
target_link_libraries(flinkppwa PRIVATE ${PROJECT_NAME})

add_executable(flinksteppermotor flinksteppermotor.c)
target_link_libraries(flinksteppermotor PRIVATE ${PROJECT_NAME} m)

add_executable(flinkreflectivesensoren flinkreflectivesensoren.c)
target_link_libraries(flinkreflectivesensoren PRIVATE ${PROJECT_NAME})

add_executable(flinkinterrupthandler flinkinterrupthandler.c)
target_link_libraries(flinkinterrupthandler PRIVATE ${PROJECT_NAME})

add_executable(flinkinterruptmultiplexer flinkinterruptmultiplexer.c)
target_link_libraries(flinkinterruptmultiplexer PRIVATE ${PROJECT_NAME})

cmake_path(RELATIVE_PATH CMAKE_CURRENT_LIST_DIR BASE_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE "relpath")
install(TARGETS
  lsflink flinkinfo flinkanaloginput flinkanalogoutput flinkdio flinkpwm flinkcounter
  flinkwd flinkppwa flinkreflectivesensoren flinksteppermotor flinkinterrupthandler flinkinterruptmultiplexer
  RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${relpath})
