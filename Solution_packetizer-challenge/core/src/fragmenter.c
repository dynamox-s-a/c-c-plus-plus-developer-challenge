#include <fragmenter.h>
#include <string.h>


void fragmenter_init(Fragmenter* f, uint32_t message_id, const uint8_t* message, size_t message_size)
{
    f->message_id       = message_id;
    f->message          = message;
    f->message_size     = message_size;
    f->total_fragments  = (message_size + MAX_PAYLOAD_SIZE - 1) / MAX_PAYLOAD_SIZE;
    f->current_fragment = 0;
}


bool fragmenter_next(Fragmenter* f, Fragment* fragment) 
{
    if (f->current_fragment >= f->total_fragments)
        return false;

    size_t offset    = f->current_fragment * MAX_PAYLOAD_SIZE;
    size_t remaining = f->message_size - offset;
    size_t chunk     = remaining > MAX_PAYLOAD_SIZE ? MAX_PAYLOAD_SIZE : remaining;

    fragment->message_id      = f->message_id;
    fragment->fragment_index  = f->current_fragment;
    fragment->total_fragments = f->total_fragments;
    fragment->payload_size    = chunk;

    memcpy(fragment->payload, f->message + offset, chunk);
    f->current_fragment++;

    return true;
}