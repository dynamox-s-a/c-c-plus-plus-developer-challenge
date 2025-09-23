cmake_minimum_required(VERSION 3.10)
project(dynamox_challenge VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_library(hmi src/hmi.cpp)
add_library(operations src/operations.cpp)
add_library(log src/log.cpp)

add_executable(dynamox main.cpp)
target_link_libraries(dynamox PRIVATE hmi operations log)
