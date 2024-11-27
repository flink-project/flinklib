add_executable(flink_test_open_close open_close.c)
target_link_libraries(flink_test_open_close PRIVATE ${PROJECT_NAME})

add_executable(flink_test_read_write read_write.c)
target_link_libraries(flink_test_read_write PRIVATE ${PROJECT_NAME})

add_executable(flink_test_base_devices base_device_test.c)
target_link_libraries(flink_test_base_devices PRIVATE ${PROJECT_NAME})

cmake_path(RELATIVE_PATH CMAKE_CURRENT_LIST_DIR BASE_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE "relpath")
install(TARGETS flink_test_open_close RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${relpath})
install(TARGETS flink_test_read_write RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${relpath})
install(TARGETS flink_test_base_devices RUNTIME DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}/${relpath})
