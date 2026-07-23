#ifndef SERIALIZER_H
#define SERIALIZER_H


#include <stdint.h>
#include <stddef.h>

#include <packet.h>


size_t serialize_packet(const Packet* packet, uint8_t* buffer);
int deserialize_packet(Packet* packet, const uint8_t* buffer, size_t buffer_size);


#endif