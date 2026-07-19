#ifndef __PACKETIZER_H
#define __PACKETIZER_H

#include <stdint.h>

//Packetizer's frame type identifiers
#define PACKET_TYPE_INVALID  0
#define PACKET_TYPE_DATA  1
#define PACKET_TYPE_ACK  2

/// @brief Maximum size, in bytes, of a single packetizer frame on the wire
/// (metadata + payload + CRC). Chosen conservatively so a frame fits inside
/// a single UDP datagram without triggering IP-level fragmentation on most
/// networks. Messages larger than the resulting payload capacity are
/// automatically split into multiple fragments by packetizer_send_data().
#define PACKET_MTU 512

/// @brief Pointer to the transport layer's data send function
/// @param void* pointer to the data to be transmitted
/// @param uint16_t length, in bytes, of the data to be transmitted
/// @returns Error code
typedef int (*transport_send)(void *,uint16_t);

/// @brief Callback invoked by the packetizer for every validated,
/// in-order fragment of an incoming message (or once, for an
/// unfragmented message). The packetizer core never buffers a whole
/// message itself -- doing so would require an internal buffer sized
/// for the largest possible message, which is unaffordable on a
/// memory-constrained device. Instead, each fragment's bytes are
/// handed to the application as soon as they are validated, and it is
/// the application's responsibility to store/reassemble them (e.g. by
/// writing each fragment straight to a file at the right offset).
/// An ACK for this fragment is only sent back to the sender after this
/// callback returns, i.e. once the application has actually handled
/// and stored the data -- never before.
/// @param sequence_number Sequence number of the message this fragment
/// belongs to. Constant across all fragments of the same message.
/// @param fragment_index Index of this fragment within the message
/// (0-based).
/// @param fragment_count Total number of fragments in this message;
/// fragment_index == fragment_count - 1 marks the last one.
/// @param data Pointer to this fragment's payload bytes. Only valid for
/// the duration of the callback; copy it if it needs to outlive the call.
/// @param len Length, in bytes, of this fragment's payload.
typedef void (*packetizer_message_received_cb)(uint16_t sequence_number,
                                                uint16_t fragment_index,
                                                uint16_t fragment_count,
                                                uint8_t * data,
                                                uint16_t len);

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

/// @brief Initializes the packetizer core, registering the callbacks it
/// uses to reach the transport layer (to send bytes) and the application
/// layer (to deliver validated fragments). Must be called once before
/// packetizer_send_data() or packetizer_receive_data().
/// @param send_fn Transport-layer function used to transmit serialized
/// frames. Must not be NULL.
/// @param on_message Callback invoked whenever a validated, in-order
/// fragment (or unfragmented message) has been received. May be NULL
/// if the receive path is not used.
/// @returns Error code
int packetizer_init(transport_send send_fn, packetizer_message_received_cb on_message);

/// @brief Packs (and fragments, if needed) an application message into one
/// or more packetizer frames and hands each one to the registered
/// transport_send function for transmission.
/// @param data_in Pointer to the application message to send.
/// @param data_len Length, in bytes, of the application message.
/// @returns Error code
int packetizer_send_data(void * data_in, uint32_t data_len);

/// @brief Feeds a raw block of bytes received from the transport layer
/// (one full datagram/frame) into the packetizer, which validates it,
/// discards it if corrupted, and delivers DATA-frame fragments to the
/// application in order via the registered packetizer_message_received_cb.
/// Only after that callback returns -- i.e. only after the application
/// has handled and stored the fragment's data -- an ACK frame
/// acknowledging it is sent back through the registered transport_send.
/// @param data_in Pointer to the raw bytes received from the transport.
/// @param data_len Length, in bytes, of data_in.
/// @returns Error code
int packetizer_receive_data(void * data_in, uint16_t data_len);

#endif /* __PACKETIZER_H */