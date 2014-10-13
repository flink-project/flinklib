include_directories(../include)

add_executable(lsflink lsflink.c)
target_link_libraries(lsflink flink)
