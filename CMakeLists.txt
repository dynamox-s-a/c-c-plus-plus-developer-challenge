cmake_minimum_required(VERSION 3.10)

project(App)

include_directories(Tools Operation)

add_executable(App main.cpp Operation/operation.cpp Operation/determinant.cpp Operation/factorial.cpp Tools/matrixParser.cpp Tools/logger.cpp)
