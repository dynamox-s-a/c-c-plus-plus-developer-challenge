cmake_minimum_required(VERSION 3.10)
project(packetizer_application C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(TRANSPORT_BACKEND "UDP" CACHE STRING "Transport backend to build: UDP or TCP")
set_property(CACHE TRANSPORT_BACKEND PROPERTY STRINGS "UDP" "TCP")

add_compile_options(-Wall -Wextra -Wpedantic)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

file(GLOB_RECURSE SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/app/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/transport/*.c"
)

add_executable(peer ${SOURCES})
target_include_directories(peer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

if(TRANSPORT_BACKEND STREQUAL "TCP")
	target_compile_definitions(peer PRIVATE TRANSPORT_USE_TCP=1)
elseif(TRANSPORT_BACKEND STREQUAL "UDP")
	target_compile_definitions(peer PRIVATE TRANSPORT_USE_TCP=0)
endif()

message(STATUS "Configured packetizer-application with sources: ${SOURCES}")

