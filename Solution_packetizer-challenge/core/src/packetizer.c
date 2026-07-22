#include <packetizer.h>
#include <packet.h>
#include <serializer.h>
#include <crc32.h>
#include <frame_parser.h>



void packetizer_init(Packetizer* packetizer, Transport* transport)
{
    packetizer->transport       = transport;
    packetizer->next_message_id = 1;
    packetizer->next_sequence   = 1;

    frame_parser_init(&packetizer->parser);
    reassembler_init(&packetizer->reassembler);
}


bool packetizer_send(Packetizer* packetizer, const uint8_t* data, size_t length)
{
    Fragmenter fragmenter;
    Fragment fragment;

    fragmenter_init(&fragmenter, packetizer->next_message_id++, data, length);

    while (fragmenter_next(&fragmenter, &fragment))
    {
        Packet packet;

        packet.start           = PACKET_START_VALUE;
        packet.type            = PACKET_DATA;
        packet.sequence        = packetizer->next_sequence++;
        packet.fragment_index  = fragment.fragment_index;
        packet.total_fragments = fragment.total_fragments;
        packet.length          = sizeof(Fragment);
        packet.payload         = (uint8_t*) &fragment;
        packet.crc             = crc32(packet.payload, packet.length);

        uint8_t tx_buffer[1024];
        size_t size = serialize_packet(&packet, tx_buffer);

        if (transport_send(packetizer->transport, tx_buffer, size) <= 0)
            return false;
    }

    return true;
}


bool packetizer_receive(Packetizer* packetizer, uint8_t* buffer, size_t* length)
{
    uint8_t rx_buffer[1024];
    int received = transport_receive(packetizer->transport, rx_buffer, sizeof(rx_buffer));

    if (received <= 0)
        return false;

    if (!frame_parser_append(&packetizer->parser, rx_buffer, received))
        return false;

    uint8_t frame[1024];
    size_t frame_size;

    while (frame_parser_next(&packetizer->parser, frame, &frame_size))
    {
        Packet packet;

        if (deserialize_packet(&packet, frame, frame_size) != 0)
            continue;

        if (packet.type == PACKET_ACK)
        {
            packet_destroy(&packet);
            continue;
        }

        if (crc32(packet.payload, packet.length) != packet.crc)
        {
            packet_destroy(&packet);
            continue;
        }

        Fragment* fragment = (Fragment*) packet.payload;

        if (!reassembler_add_fragment(&packetizer->reassembler, fragment))
        {
            packet_destroy(&packet);
            continue;
        }

        if (!reassembler_is_complete(&packetizer->reassembler))
        {
            packet_destroy(&packet);
            continue;
        }

        *length = reassembler_get_message(&packetizer->reassembler, buffer);

        reassembler_init(&packetizer->reassembler);
        packet_destroy(&packet);
        return true;
    }

    return false;
}