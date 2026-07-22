#ifndef FRAGMENTER_H
#define FRAGMENTER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_PAYLOAD_SIZE       32
#define MAX_FRAGMENTS          16
#define MAX_MESSAGE_SIZE       (MAX_PAYLOAD_SIZE * MAX_FRAGMENTS)

typedef struct
{
    uint32_t message_id;
    uint16_t fragment_index;
    uint16_t total_fragments;
    uint16_t payload_size;
    uint8_t payload[MAX_PAYLOAD_SIZE];
} Fragment;


typedef struct
{
    uint32_t message_id;
    const uint8_t* message;
    size_t message_size;
    uint16_t total_fragments;
    uint16_t current_fragment;
} Fragmenter;


void fragmenter_init(Fragmenter* f, uint32_t message_id, const uint8_t* message, size_t message_size);
bool fragmenter_next(Fragmenter* f, Fragment* fragment);

#endif