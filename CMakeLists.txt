cmake_minimum_required(VERSION 2.6)
project(flinklib)

## Compiler flags
if(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_C_FLAGS "-std=gnu99 -O1")
endif()

## Set library version
include(cmake/versionCheckHelpers.cmake)
setLibraryVersion()

## Doxygen documentation target (make doc)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif(DOXYGEN_FOUND)


## Install CMake configuration file
set(INCLUDE_INSTALL_DIR include/)
set(LIB_INSTALL_DIR lib/)

include(CMakePackageConfigHelpers)

configure_package_config_file(
	cmake/flinkConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/flinkConfig.cmake
	INSTALL_DESTINATION ${LIB_INSTALL_DIR}/flink/cmake
	PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)

write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/flinkConfigVersion.cmake
	VERSION 0
	COMPATIBILITY SameMajorVersion)

install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/flinkConfig.cmake
		${CMAKE_CURRENT_BINARY_DIR}/flinkConfigVersion.cmake
	DESTINATION
		${LIB_INSTALL_DIR}/flink/cmake)


add_subdirectory(flinkinterface)
add_subdirectory(lib)
add_subdirectory(utils)
add_subdirectory(test)

INSTALL(FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/include/flinklib.h 
	${CMAKE_CURRENT_SOURCE_DIR}/include/flinkioctl.h 
	${CMAKE_CURRENT_SOURCE_DIR}/include/flink_funcid.h 
	${CMAKE_CURRENT_SOURCE_DIR}/lib/types.h 
	DESTINATION include)
