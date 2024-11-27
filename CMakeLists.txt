cmake_minimum_required(VERSION 3.14)

include(cmake/git.cmake)

version_from_git(GIT_VERSION)
message("git version: ${GIT_VERSION}")

project(flink LANGUAGES C VERSION ${GIT_VERSION})

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(${PROJECT_NAME} SHARED)
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${GIT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR} OUTPUT_NAME ${PROJECT_NAME} EXPORT_NAME ${PROJECT_NAME})

add_subdirectory(flinkinterface)
add_subdirectory(lib)
add_subdirectory(utils)
add_subdirectory(test)

target_include_directories(${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
  PUBLIC $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/flinkinterface> $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_14)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(DIRECTORY ${CMAKE_BINARY_DIR}/flinkinterface/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} FILES_MATCHING PATTERN "*.h")

install(TARGETS "${PROJECT_NAME}"
  EXPORT ${PROJECT_NAME}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(EXPORT ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME})


## Doxygen documentation target (make doc)
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM )
endif(DOXYGEN_FOUND)

