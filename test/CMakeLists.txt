include_directories(../include ${CMAKE_BINARY_DIR}/flinkinterface/)

add_executable(flink_test_open_close open_close.c)
target_link_libraries(flink_test_open_close flink)

add_executable(flink_test_read_write read_write.c)
target_link_libraries(flink_test_read_write flink)

add_executable(flink_test_base_devices base_device_test.c)
target_link_libraries(flink_test_base_devices flink)
