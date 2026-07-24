cmake_minimum_required(VERSION 3.16)

add_executable(unit_tests
    main.c
    test_crc16.c
    test_cobs.c
    test_packetizer.c
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/packetizer/include
        ${CMAKE_SOURCE_DIR}/lib/packetizer/src
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(unit_tests PRIVATE packetizer)

add_test(NAME unit_tests COMMAND unit_tests)
