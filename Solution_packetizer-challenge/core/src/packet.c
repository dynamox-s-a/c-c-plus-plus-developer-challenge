#include <packet.h>


void packet_destroy(Packet* packet)
{
    if (packet == NULL)
        return;

    free(packet->payload);
    packet->payload = NULL;
}