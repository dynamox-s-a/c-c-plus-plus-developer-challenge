#ifndef __PACKETIZER_H
#define __PACKETIZER_H

#include <stdint.h>

//Packetizer's frame type identifiers
#define PACKET_TYPE_INVALID  0
#define PACKET_TYPE_DATA  1
#define PACKET_TYPE_ACK  2

/// @brief Pointer to the transport layer's data send function
/// @param uint16_t length, in bytes, of the data to be transmitted
/// @param void* pointer to the data to be transmitted
/// @returns Error code
typedef int (*transport_send)(void *,uint16_t);


/// @brief Packetizer frame format
typedef struct
{
    uint8_t sof; //Start of frame

    uint8_t packet_type; //Identifier of what type of data is being transmitted. See PACKET_TYPE_ macros.
    uint16_t payload_length; //Size (in bytes) of the data pointed by the frame's payload pointer
    uint16_t sequence_number; //Counter assigned to each ongoing packet
    uint16_t fragment_index; //Index of the current message fragment. Used when transmitting large data blobs in smaller fragments
    uint16_t fragment_count; //Total fragment count. Used when transmitting large data blobs in smaller fragments.

    uint8_t * payload; //Pointer to the data to be transmitted.

    uint32_t crc;   //Cyclic redundance check of the data between SOF and EOF

}packetizer_frame_t;

#endif /* __PACKETIZER_H */