#ifndef __PACKETIZER_H
#define __PACKETIZER_H

#include <stdint.h>

//Packetizer's frame type identifiers
#define PACKET_TYPE_INVALID  0
#define PACKET_TYPE_DATA  1
#define PACKET_TYPE_ACK  2
#define PACKET_TYPE_NACK 3

/// @brief Maximum size, in bytes, of a single packetizer frame on the wire
/// (metadata + payload + CRC). Chosen to represent a typical MTU used in 
/// low-power wireless protocols, such as BLE.
#define PACKET_MTU 512u

/// @brief Maximum time, in milliseconds, packetizer_send_data() waits for
/// the ACK of a given DATA fragment before retransmitting it.
#define PACKETIZER_ACK_TIMEOUT_MS 300u

/// @brief Maximum number of retransmission attempts for a single DATA
/// fragment before giving up on it and reporting delivery failure to the
/// caller of packetizer_send_data().
#define PACKETIZER_ACK_MAX_RETRIES 5u

#define PACKETIZER_NACK_MAX_RETRIES 5u

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

/// @brief Pointer to the transport layer's data send function
/// @param void* pointer to the data to be transmitted
/// @param uint16_t length, in bytes, of the data to be transmitted
/// @returns Error code
typedef int (*transport_send_t)(void *,uint16_t);

/// @brief Callback invoked by the packetizer for every validated,
/// in-order fragment of an incoming message (or once, for an
/// unfragmented message). The packetizer core never buffers a whole
/// message itself -- doing so would require an internal buffer sized
/// for the largest possible message, which is unaffordable on a
/// memory-constrained device. Instead, each fragment's bytes are
/// handed to the application as soon as they are validated, and it is
/// the application's responsibility to store/reassemble them.
/// @param rx_info frame containing the received data
typedef void (*packetizer_message_received_cb)(packetizer_frame_t rx_info);


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
int packetizer_init(transport_send_t send_fn, packetizer_message_received_cb on_message);

/// @brief Packs (and fragments, if needed) an application message into one
/// or more packetizer frames and hands each one to the registered
/// transport_send_t function for transmission.
/// @param data_in Pointer to the application message to send.
/// @param data_len Length, in bytes, of the application message.
/// @returns 0 on success (every fragment was acknowledged); ETIMEDOUT if
/// a fragment's ACK was never received after exhausting all retries;
/// another error code for other failures (invalid arguments, transport
/// error, etc).
int packetizer_send_data(void * data_in, uint32_t data_len);

/// @brief Feeds a raw block of bytes received from the transport layer
/// into the packetizer, which validates it, discards it if corrupted, and delivers DATA-frame fragments to the
/// application in order via the registered packetizer_message_received_cb.
/// Only after that callback returns -- i.e. only after the application
/// has handled and stored the fragment's data -- an ACK frame
/// acknowledging it is sent back through the registered transport_send_t.
///
/// ACK frames (PACKET_TYPE_ACK) received here are matched against the
/// single fragment, if any, that a concurrent call to
/// packetizer_send_data() is currently waiting on; a match unblocks that
/// wait immediately instead of it having to sit out the full timeout.
/// @param data_in Pointer to the raw bytes received from the transport.
/// @param data_len Length, in bytes, of data_in.
/// @returns Error code
int packetizer_receive_data(void * data_in, uint16_t data_len);

#endif /* __PACKETIZER_H */