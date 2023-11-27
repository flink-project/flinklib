cmake_minimum_required(VERSION 3.10)

project(flinklib)

set(CMAKE_CXX_STANDARD 14)


## Load CMake modules library at specified version
set(LOAD_CMAKE_MODULES_LIB TRUE CACHE BOOL "TRUE if the cmake-modules library should be loaded.")

if(LOAD_CMAKE_MODULES_LIB)
  include(cmake/CMakeModulesLibLoader.cmake)
  load_cmake_modules_lib(https://github.com/eeros-project/cmake-modules.git)
endif()

include(CMakeModulesLib)

if(CMAKE_BUILD_TYPE MATCHES "Debug|DEBUG")
  logger_on()
endif()

if(LOAD_CMAKE_MODULES_LIB)
  checkout_cmake_modules_lib_version(8fcde84)
endif()


## Fetch the version information from git tag
include(VersioningHelpers)
setLibraryVersion()


## Create the package files
createPackageFiles(flinkConfig.cmake.in include/ lib/)


add_subdirectory(flinkinterface)
add_subdirectory(lib)
add_subdirectory(utils)
add_subdirectory(test)

INSTALL(FILES 
	${CMAKE_CURRENT_SOURCE_DIR}/include/flinklib.h 
	${CMAKE_CURRENT_SOURCE_DIR}/include/flinkioctl.h 
	${CMAKE_CURRENT_BINARY_DIR}/flinkinterface/flink_funcid.h
	${CMAKE_CURRENT_BINARY_DIR}/flinkinterface/flinkioctl_cmd.h
	${CMAKE_CURRENT_SOURCE_DIR}/lib/types.h 
	DESTINATION include)


## Doxygen documentation target (make doc)
find_package(Doxygen)
if(DOXYGEN_FOUND)
	configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
	add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif(DOXYGEN_FOUND)

