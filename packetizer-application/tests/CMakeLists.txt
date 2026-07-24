cmake_minimum_required(VERSION 3.10)
project(packetizer_application C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(test_packetizer auto_tests.c)
target_include_directories(test_packetizer PRIVATE /home/davi/Desktop/c-c-plus-plus-developer-challenge/packetizer-application/inc)
target_link_libraries(test_packetizer PRIVATE pthread)
add_test(NAME packetizer_unit_tests COMMAND test_packetizer)