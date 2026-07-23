#include <reassembler.h>

#include <string.h>

void reassembler_init(Reassembler* r)
{
    memset(r, 0, sizeof(*r));
}


bool reassembler_add_fragment(Reassembler* r, const Fragment* fragment)
{
    if (fragment->fragment_index >= MAX_FRAGMENTS)
        return false;

    if (r->received_fragments == 0)
    {
        r->message_id = fragment->message_id;
        r->total_fragments = fragment->total_fragments;
    }

    if (fragment->message_id != r->message_id)
        return false;

    if (r->received[fragment->fragment_index])
        return true;

    size_t offset = fragment->fragment_index * MAX_PAYLOAD_SIZE;
    memcpy(r->data + offset, fragment->payload, fragment->payload_size);

    r->received[fragment->fragment_index] = true;
    r->received_fragments++;

    if (fragment->fragment_index == fragment->total_fragments - 1)
    {
        r->total_size = offset + fragment->payload_size;
    }

    return true;
}


bool reassembler_is_complete(const Reassembler* r)
{
    return r->received_fragments == r->total_fragments;
}


size_t reassembler_get_message(Reassembler* r, uint8_t* buffer)
{
    memcpy(buffer, r->data, r->total_size);
    return r->total_size;
}