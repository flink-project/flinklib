include_directories(../include)

add_executable(flink_test_open_close open_close.c)
target_link_libraries(flink_test_open_close flink)

add_executable(flink_test_read_write read_write.c)
target_link_libraries(flink_test_read_write flink)

add_executable(flink_test_pwm pwm.c)
target_link_libraries(flink_test_pwm flink)

add_executable(flink_test_gpio gpio.c)
target_link_libraries(flink_test_gpio flink)

add_executable(flink_test_counter counter.c)
target_link_libraries(flink_test_counter flink)