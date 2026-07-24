cmake_minimum_required(VERSION 3.16)

add_library(udp_transport STATIC
    udp_transport.c
)

target_include_directories(udp_transport PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
