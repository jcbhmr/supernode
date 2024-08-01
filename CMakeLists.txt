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

add_executable(node)
target_sources(node PRIVATE src/bin/node.cpp src/binlib.cpp)
target_compile_features(node PRIVATE cxx_std_23)
target_compile_options(node PRIVATE -mcosmo -fexceptions)
target_link_options(node PRIVATE $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-mdbg> $<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:-mtiny>)
target_link_libraries(node PUBLIC fmt::fmt)
target_compile_definitions(node PRIVATE PROJECT_NAME="${PROJECT_NAME}" PROJECT_VERSION="${PROJECT_VERSION}")
add_custom_command(TARGET node POST_BUILD COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=postbuild -DTARGET_FILE="$<TARGET_FILE:node>" -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -DTASK_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake")

add_executable(npm)
target_sources(npm PRIVATE src/bin/npm.cpp src/binlib.cpp)
target_compile_features(npm PRIVATE cxx_std_23)
target_compile_options(npm PRIVATE -mcosmo -fexceptions)
target_link_options(npm PRIVATE $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-mdbg> $<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:-mtiny>)
target_link_libraries(npm PUBLIC fmt::fmt)
target_compile_definitions(npm PRIVATE PROJECT_NAME="${PROJECT_NAME}" PROJECT_VERSION="${PROJECT_VERSION}")
add_custom_command(TARGET npm POST_BUILD COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=postbuild -DTARGET_FILE="$<TARGET_FILE:npm>" -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -DTASK_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake")

add_executable(npx)
target_sources(npx PRIVATE src/bin/npx.cpp src/binlib.cpp)
target_compile_features(npx PRIVATE cxx_std_23)
target_compile_options(npx PRIVATE -mcosmo -fexceptions)
target_link_options(npx PRIVATE $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-mdbg> $<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:-mtiny>)
target_link_libraries(npx PUBLIC fmt::fmt)
target_compile_definitions(npx PRIVATE PROJECT_NAME="${PROJECT_NAME}" PROJECT_VERSION="${PROJECT_VERSION}")
add_custom_command(TARGET npx POST_BUILD COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=postbuild -DTARGET_FILE="$<TARGET_FILE:npx>" -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -DTASK_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake")

add_executable(corepack)
target_sources(corepack PRIVATE src/bin/corepack.cpp src/binlib.cpp)
target_compile_features(corepack PRIVATE cxx_std_23)
target_compile_options(corepack PRIVATE -mcosmo -fexceptions)
target_link_options(corepack PRIVATE $<$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>:-mdbg> $<$<STREQUAL:${CMAKE_BUILD_TYPE},Release>:-mtiny>)
target_link_libraries(corepack PUBLIC fmt::fmt)
target_compile_definitions(corepack PRIVATE PROJECT_NAME="${PROJECT_NAME}" PROJECT_VERSION="${PROJECT_VERSION}")
add_custom_command(TARGET corepack POST_BUILD COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=postbuild -DTARGET_FILE="$<TARGET_FILE:corepack>" -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -DTASK_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake")

if(BUILD_TESTING)
  add_custom_target(generate COMMAND "${CMAKE_COMMAND}" -DTASK_NAME=generate -DTASK_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/task.cmake")
endif()
