cmake_minimum_required(VERSION 3.16)

add_library(packetizer STATIC
    src/crc16.c
    src/cobs.c
    src/packetizer.c
)

target_include_directories(packetizer
    PUBLIC  include
    PRIVATE src
)


