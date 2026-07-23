cmake_minimum_required(VERSION 3.16)

add_executable(peer main.c)

target_link_libraries(peer
    PRIVATE
        packetizer
        udp_transport
        pthread
)
