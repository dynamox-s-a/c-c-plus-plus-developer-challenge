cmake_minimum_required(VERSION 3.10)
project(DynamoxCalculator C)

# C99 standard with warnings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# Optimization for embedded (can be changed to -Os for size)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

# Show memory usage warnings (embedded best practice)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstack-usage=1024")

message(STATUS "===========================================")
message(STATUS "Dynamox Calculator - Embedded System")
message(STATUS "===========================================")
message(STATUS "Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Core sources
set(CORE_SOURCES
    src/core/operation_manager.c
    src/core/logger.c
)

# HMI sources
set(HMI_SOURCES
    src/hmi/terminal_interface.c
    src/hmi/input_validator.c
)

# Operations sources
set(OPERATIONS_SOURCES
    src/operations/addition.c
    src/operations/mean.c
    src/operations/determinant.c
    src/operations/multiply.c
)

# Main executable
add_executable(dynamox_calculator
    src/main.c
    ${CORE_SOURCES}
    ${HMI_SOURCES}
    ${OPERATIONS_SOURCES}
)

# Link math library (for sqrt, etc)
target_link_libraries(dynamox_calculator m)

# Create logs directory at build time
add_custom_command(TARGET dynamox_calculator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/logs
    COMMAND ${CMAKE_COMMAND} -E echo "Build successful!"
    COMMAND ${CMAKE_COMMAND} -E echo "Run: ./dynamox_calculator"
)

# Print summary
message(STATUS "===========================================")
message(STATUS "Sources configured:")
message(STATUS "  - Core: ${CORE_SOURCES}")
message(STATUS "  - HMI: ${HMI_SOURCES}")
message(STATUS "  - Operations: ${OPERATIONS_SOURCES}")
message(STATUS "===========================================")