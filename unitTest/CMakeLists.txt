cmake_minimum_required(VERSION 3.12)
project(dynamox_test_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

set(GOOGLE_TEST_PATH /usr/src/googletest)
add_subdirectory(${GOOGLE_TEST_PATH} googletest_build)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.cpp"
    "${CMAKE_SOURCE_DIR}/../*.cpp"
    "${CMAKE_SOURCE_DIR}/../libs/*.cpp"
    "${CMAKE_SOURCE_DIR}/../libs/lib_menu/*.cpp"
    "${CMAKE_SOURCE_DIR}/../libs/lib_expression/*.cpp"
)

file(GLOB PROJECT_HEADERS CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/../config/*.hpp"
    "${CMAKE_SOURCE_DIR}/../libs/*.h"
    "${CMAKE_SOURCE_DIR}/../libs/*.hpp"
    "${CMAKE_SOURCE_DIR}/../libs/lib_menu/*.h"
    "${CMAKE_SOURCE_DIR}/../libs/lib_expression/*.h"
)

add_executable(mytests
    ${TEST_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(mytests PRIVATE
    ${GOOGLE_TEST_PATH}/googletest/include
    ${CMAKE_SOURCE_DIR}/../config
    ${CMAKE_SOURCE_DIR}/../libs
    ${CMAKE_SOURCE_DIR}/../libs/lib_menu
    ${CMAKE_SOURCE_DIR}/../libs/lib_hmi
    ${CMAKE_SOURCE_DIR}/../libs/lib_expression
    ${CMAKE_SOURCE_DIR}/../libs/json/include 
)

target_link_libraries(mytests PRIVATE gtest_main)

include(GoogleTest)
gtest_discover_tests(mytests)
