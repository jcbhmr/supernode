cmake_minimum_required(VERSION 3.29)

project(supernode
    VERSION 22.5.1
    LANGUAGES C CXX
)

include(cmake/CPM.cmake)

CPMAddPackage(
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG 11.0.2
  OPTIONS "FMT_OS OFF" "FMT_UNICODE OFF"
)

add_library(incbin INTERFACE)
target_include_directories(incbin INTERFACE incbin)

# TODO: Investigate using 7zip for smaller bundles
configure_file(node-darwin-arm64.zip node-darwin-arm64.zip COPYONLY)
configure_file(node-darwin-x64.zip node-darwin-x64.zip COPYONLY)
configure_file(node-linux-arm64.zip node-linux-arm64.zip COPYONLY)
configure_file(node-linux-x64.zip node-linux-x64.zip COPYONLY)
configure_file(node-node_modules.zip node-node_modules.zip COPYONLY)
configure_file(node-unixlike.zip node-unixlike.zip COPYONLY)
configure_file(node-win-arm64.zip node-win-arm64.zip COPYONLY)
configure_file(node-win-x64.zip node-win-x64.zip COPYONLY)
configure_file(node-win.zip node-win.zip COPYONLY)

add_executable(node src/main.cpp)
target_compile_features(node PRIVATE cxx_std_23 c_std_23)
target_compile_options(node PRIVATE -mcosmo -fexceptions)
target_link_options(node PRIVATE $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-mdbg> $<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:-mtiny>)
add_custom_command(
  TARGET node
  POST_BUILD
  COMMAND zip -0Ar $<TARGET_FILE:node> node-darwin-arm64.zip
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/zip"
)
target_link_libraries(node fmt::fmt incbin)
