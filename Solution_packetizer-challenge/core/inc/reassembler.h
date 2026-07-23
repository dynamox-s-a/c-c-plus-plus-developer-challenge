#ifndef REASSEMBLER_H
#define REASSEMBLER_H


#include <fragmenter.h>


typedef struct
{
    uint32_t message_id;
    uint16_t total_fragments;
    uint16_t received_fragments;
    bool     received[MAX_FRAGMENTS];
    uint8_t  data[MAX_MESSAGE_SIZE];
    size_t   total_size;
} Reassembler;


void reassembler_init(Reassembler *r);
bool reassembler_add_fragment(Reassembler* r, const Fragment* fragment);
bool reassembler_is_complete(const Reassembler* r);
size_t reassembler_get_message(Reassembler* r, uint8_t* buffer);


#endif