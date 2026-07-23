#include <serializer.h>
#include <stdlib.h>
#include <string.h>


/* 
A serialização e deserialização, copia n quantidade de bytes e avança o ponterio
com a mesma n quantidade de bytes para chegar no próximo valor 
*/


size_t serialize_packet(const Packet* packet, uint8_t* buffer)
{
    uint8_t* ptr = buffer;

    memcpy(ptr, &packet->start, sizeof(packet->start));
    ptr += sizeof(packet->start);

    memcpy(ptr, &packet->type, sizeof(packet->type));
    ptr += sizeof(packet->type);

    memcpy(ptr, &packet->length, sizeof(packet->length));
    ptr += sizeof(packet->length);

    memcpy(ptr, &packet->sequence, sizeof(packet->sequence));
    ptr += sizeof(packet->sequence);

    memcpy(ptr, &packet->fragment_index, sizeof(packet->fragment_index));
    ptr += sizeof(packet->fragment_index);

    memcpy(ptr, &packet->total_fragments, sizeof(packet->total_fragments));
    ptr += sizeof(packet->total_fragments);

    memcpy(ptr, &packet->crc, sizeof(packet->crc));
    ptr += sizeof(packet->crc);

    memcpy(ptr, packet->payload, packet->length);
    ptr += packet->length;

    return (size_t)(ptr - buffer); // tamanho do pacote serializado. Ex ptr: 10, buffer: 18, = 8 bytes
}


int deserialize_packet(Packet* packet, const uint8_t* buffer, size_t buffer_size)
{
    const uint8_t* ptr = buffer;

    memcpy(&packet->start, ptr, sizeof(packet->start));
    ptr += sizeof(packet->start);

    memcpy(&packet->type, ptr, sizeof(packet->type));
    ptr += sizeof(packet->type);

    memcpy(&packet->length, ptr, sizeof(packet->length));
    ptr += sizeof(packet->length);

    memcpy(&packet->sequence, ptr, sizeof(packet->sequence));
    ptr += sizeof(packet->sequence);

    memcpy(&packet->fragment_index, ptr, sizeof(packet->fragment_index));
    ptr += sizeof(packet->fragment_index);

    memcpy(&packet->total_fragments, ptr, sizeof(packet->total_fragments));
    ptr += sizeof(packet->total_fragments);

    memcpy(&packet->crc, ptr, sizeof(packet->crc));
    ptr += sizeof(packet->crc);

    packet->payload = malloc(packet->length);

    if (packet->payload == NULL)
        return -1;

    memcpy(packet->payload, ptr, packet->length);

    return 0;
}