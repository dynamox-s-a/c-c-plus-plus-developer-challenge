cmake_minimum_required(VERSION 3.16)
project(packetizer-challenge C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------------------------------- #
# Compiler warnings
# --------------------------------------------------------------------------- #
add_compile_options(
    $<$<COMPILE_LANGUAGE:C>:-Wall>
    $<$<COMPILE_LANGUAGE:C>:-Wextra>
    $<$<COMPILE_LANGUAGE:C>:-Wpedantic>
    $<$<COMPILE_LANGUAGE:C>:-Wshadow>
    $<$<COMPILE_LANGUAGE:C>:-Wconversion>
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Wpedantic>
    $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
)

# --------------------------------------------------------------------------- #
# Sub-projects
# --------------------------------------------------------------------------- #
add_subdirectory(lib/packetizer)
add_subdirectory(transport/udp)
add_subdirectory(apps/peer)
add_subdirectory(tools/channel_sim)

# --------------------------------------------------------------------------- #
# Tests
# --------------------------------------------------------------------------- #
enable_testing()
add_subdirectory(tests)
