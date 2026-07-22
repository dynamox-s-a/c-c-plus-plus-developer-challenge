#ifndef PACKETIZER_H
#define PACKETIZER_H


#include <transport.h>
#include <fragmenter.h>
#include <reassembler.h>
#include <frame_parser.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct
{
    Transport*  transport;
    FrameParser parser;
    Reassembler reassembler;
    uint32_t    next_message_id;
    uint32_t    next_sequence;
} Packetizer;



void packetizer_init(Packetizer* packetizer, Transport* transport);
bool packetizer_send(Packetizer* packetizer, const uint8_t* data, size_t length);
bool packetizer_receive(Packetizer* packetizer, uint8_t* buffer, size_t* length);

#endif