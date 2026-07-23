#ifndef FRAME_PARSER_H
#define FRAME_PARSER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FRAME_BUFFER_SIZE 1024


typedef struct
{
    uint8_t buffer[FRAME_BUFFER_SIZE];
    size_t length;
} FrameParser;


void frame_parser_init(FrameParser* parser);
bool frame_parser_append(FrameParser* parser, const uint8_t* data, size_t size);
bool frame_parser_next(FrameParser* parser, uint8_t* frame, size_t* frame_size);


#endif