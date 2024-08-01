cmake_minimum_required(VERSION 3.29)

project(supernode
    VERSION 22.5.1
    LANGUAGES CXX
)

include(CTest)
include(cmake/CPM.cmake)

CPMAddPackage(
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 11.0.2
  OPTIONS "FMT_OS OFF" "FMT_UNICODE OFF"
)

set(build_zip_output node-darwin-arm64 node-darwin-x64 node-linux-arm64 node-linux-x64 node-win-arm64 node-win-x64 node-node_modules node-unixlike node-win)
add_custom_command(
  OUTPUT ${build_zip_output}
  COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=build-zip -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -DTASK_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake"
  DEPENDS node-darwin-arm64.tar.gz node-darwin-x64.tar.gz node-linux-arm64.tar.xz node-linux-x64.tar.xz node-win-arm64.zip node-win-x64.zip
  COMMENT "Building zip"
)
find_program(zip_command NAMES zip REQUIRED)

add_executable(node)
target_sources(node PRIVATE src/bin/node.cpp src/binlib.cpp)
target_compile_features(node PRIVATE cxx_std_23)
target_compile_options(node PRIVATE -mcosmo -fexceptions)
target_link_options(node PRIVATE $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-mdbg> $<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:-mtiny>)
target_link_libraries(node PRIVATE fmt::fmt)
target_compile_definitions(node PRIVATE PROJECT_NAME="${PROJECT_NAME}" PROJECT_VERSION="${PROJECT_VERSION}")
add_custom_command(TARGET node POST_BUILD COMMAND "${zip_command}" -qAr "$<TARGET_FILE:node>" ${build_zip_output})

if(BUILD_TESTING)
  add_custom_target(generate COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=generate -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake")
endif()
