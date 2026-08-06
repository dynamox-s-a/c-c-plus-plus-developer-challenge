cmake_minimum_required(VERSION 3.10)
project(dynamox_challenger LANGUAGES CXX)

find_package(spdlog REQUIRED)

set(PROJECT_NAME matrix_application)

# Set version of C++
set(CMAKE_CXX_STANDARD 17)


file(GLOB PROJECT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.cc"
  "${CMAKE_SOURCE_DIR}/src/*.cxx"
  "${CMAKE_SOURCE_DIR}/src/*.c"
  "${CMAKE_SOURCE_DIR}/drv/*.cpp"
  "${CMAKE_SOURCE_DIR}/libs/*.cpp"
  "${CMAKE_SOURCE_DIR}/libs/lib_expression/*.cpp"
  "${CMAKE_SOURCE_DIR}/libs/lib_menu/*.cpp"
  "${CMAKE_SOURCE_DIR}/libs/lib_hmi/*.cpp"
)

file(GLOB PROJECT_HEADERS CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.h"
  "${CMAKE_SOURCE_DIR}/src/*.hpp"
  "${CMAKE_SOURCE_DIR}/drv/*.hpp"
  "${CMAKE_SOURCE_DIR}/config/*.hpp"
  "${CMAKE_SOURCE_DIR}/libs/*.h"
  "${CMAKE_SOURCE_DIR}/libs/lib_expression/*.h"
  "${CMAKE_SOURCE_DIR}/libs/lib_menu/*.h"
  "${CMAKE_SOURCE_DIR}/libs/lib_hmi/*.h"
  "${CMAKE_SOURCE_DIR}/libs/json/include/nlohmann/*.hpp"
)

if(NOT PROJECT_SOURCES)
  message(FATAL_ERROR "No source files found for target dynamox_challenger")
endif()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

set(HEADERS_INCLUD_DIRS 
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/drv
  ${CMAKE_SOURCE_DIR}/config
  ${CMAKE_SOURCE_DIR}/libs
  ${CMAKE_SOURCE_DIR}/libs/lib_expression
  ${CMAKE_SOURCE_DIR}/libs/lib_menu
  ${CMAKE_SOURCE_DIR}/libs/lib_hmi
  ${CMAKE_SOURCE_DIR}/libs/json/include/ 
)

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADERS_INCLUD_DIRS})

target_link_libraries(${PROJECT_NAME} PRIVATE stdc++ spdlog::spdlog)

