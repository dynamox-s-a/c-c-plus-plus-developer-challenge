#ifndef PACKET_H
#define PACKET_H


#include <stdint.h>
#include <stdlib.h>


#define PACKET_START_VALUE (uint16_t) 0xAABB


typedef enum Packet_Type
{
    PACKET_DATA,
    PACKET_ACK,
} Packet_type;


typedef struct
{
    uint16_t    start;
    Packet_type type;
    uint16_t    length;
    uint8_t*    payload;
    uint32_t    sequence;
    uint16_t    fragment_index;
    uint16_t    total_fragments;
    uint32_t    crc;
} Packet;



void packet_destroy(Packet* packet);


#endif