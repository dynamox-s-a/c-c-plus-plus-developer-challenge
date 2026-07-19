#include "packetizer.h"
#include <errno.h>
#include <string.h>

#define PACKETIZER_SOF 0xAA

#define METADATA_SIZE (1 + 1 + 2 + 2 + 2 + 2 + 4)
#define PAYLOAD_MAX_SIZE (PACKET_MTU - METADATA_SIZE)

/*
 * Offsets of each field within a serialized (on-the-wire) frame.
 * Kept private to this file: callers never see the wire layout, only
 * the in-memory packetizer_frame_t.
 */
#define OFFSET_SOF              0
#define OFFSET_PACKET_TYPE      1
#define OFFSET_PAYLOAD_LENGTH   2
#define OFFSET_SEQUENCE_NUMBER  4
#define OFFSET_FRAGMENT_INDEX   6
#define OFFSET_FRAGMENT_COUNT   8
#define OFFSET_PAYLOAD          10

/// @brief Global state of the packetizer core: the callbacks registered
/// by packetizer_init() and the sequence number counter used to tag
/// outgoing messages. Kept private (static) to this translation unit,
/// as it is purely an implementation detail of the core, not something
/// callers should reach into directly.
typedef struct
{
    transport_send send_fn;                     /* registered transport send function */
    packetizer_message_received_cb on_message;  /* registered "message ready" callback */
    uint16_t next_sequence_number;               /* sequence number to assign to the next outgoing message */
    int is_initialized;
} packetizer_state_t;

static packetizer_state_t g_state;

/// @brief State used to track a fragmented message on the receive
/// side. Deliberately holds no message content: each fragment's bytes
/// are handed to the application as soon as they are validated (see
/// packetizer_message_received_cb), so the only thing that needs
/// tracking here is *which* fragment is expected next -- a handful of
/// integers, regardless of how large the overall message is. This
/// avoids the alternative of a whole-message buffer sized for the
/// worst case, which would be wasteful (or simply not fit) on a
/// memory-constrained device.
///
/// This implementation tracks a single in-flight (incomplete) message
/// at a time, which matches the point-to-point nature of the demo
/// applications built on top of this core. Supporting multiple
/// concurrent in-flight messages (e.g. multiple peers) would require
/// keying this state by sequence_number/peer instead of using one
/// static instance.
typedef struct
{
    uint16_t sequence_number;         /* sequence number of the message being tracked */
    uint16_t fragment_count;          /* total fragments expected for this message */
    uint16_t next_expected_fragment;  /* next fragment_index expected, in order */
    int in_progress;                  /* 0 = idle, 1 = tracking in progress */
} packetizer_reassembly_t;

static packetizer_reassembly_t g_reassembly;

/// @brief Computes a CRC-32 (IEEE 802.3 polynomial, 0xEDB88320) over a
/// block of bytes. Implemented locally (bit-by-bit, no lookup table) so
/// the packetizer core has no external dependencies, at the cost of
/// being slower than a table-driven implementation; acceptable given
/// the small frame sizes involved.
/// @param data Pointer to the bytes to checksum.
/// @param len Number of bytes to checksum.
/// @returns The computed CRC-32 value.
static uint32_t packetizer_crc32(const uint8_t * data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFu;

    for (uint32_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++)
        {
            if (crc & 1u)
            {
                crc = (crc >> 1) ^ 0xEDB88320u;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

/// @brief Writes a 16-bit value into a byte buffer in big-endian
/// (network) order. Implemented manually, instead of relying on
/// htons()/ntohs(), so the packetizer core has no dependency on
/// platform socket headers and stays fully transport-agnostic.
static void write_be16(uint8_t * buf, uint16_t value)
{
    buf[0] = (uint8_t)(value >> 8);
    buf[1] = (uint8_t)(value & 0xFFu);
}

/// @brief Reads a big-endian (network order) 16-bit value from a byte
/// buffer. Counterpart of write_be16().
static uint16_t read_be16(const uint8_t * buf)
{
    return (uint16_t)(((uint16_t)buf[0] << 8) | (uint16_t)buf[1]);
}

/// @brief Writes a 32-bit value into a byte buffer in big-endian
/// (network) order. See write_be16() for the rationale of not using
/// htonl().
static void write_be32(uint8_t * buf, uint32_t value)
{
    buf[0] = (uint8_t)(value >> 24);
    buf[1] = (uint8_t)(value >> 16);
    buf[2] = (uint8_t)(value >> 8);
    buf[3] = (uint8_t)(value & 0xFFu);
}

/// @brief Reads a big-endian (network order) 32-bit value from a byte
/// buffer. Counterpart of write_be32().
static uint32_t read_be32(const uint8_t * buf)
{
    return ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8)  | (uint32_t)buf[3];
}

/// @brief Fills in a packetizer_frame_t descriptor for a single data
/// fragment. This does not touch the wire/serialized representation:
/// it only builds the in-memory description consumed by
/// packetizer_serialize_frame().
/// @param data_in Pointer to this fragment's payload bytes (a slice of
/// the original application message).
/// @param data_len Length, in bytes, of this fragment's payload. Must
/// not exceed PAYLOAD_MAX_SIZE.
/// @param fragment_index Index of this fragment within the message.
/// @param fragment_count Total number of fragments in the message.
/// @param sequence_number Sequence number shared by all fragments of
/// this message.
/// @param frame_out Descriptor to be filled in.
/// @returns Error code
static int packetizer_pack_data(void * data_in, uint32_t data_len,
                                 uint16_t fragment_index, uint16_t fragment_count,
                                 uint16_t sequence_number,
                                 packetizer_frame_t * frame_out)
{
    if (data_in == NULL || data_len == 0 || frame_out == NULL) return EINVAL;
    if (data_len > PAYLOAD_MAX_SIZE) return EINVAL;

    frame_out->sof             = PACKETIZER_SOF;
    frame_out->packet_type     = PACKET_TYPE_DATA;
    frame_out->payload_length  = (uint16_t)data_len;
    frame_out->sequence_number = sequence_number;
    frame_out->fragment_index  = fragment_index;
    frame_out->fragment_count  = fragment_count;
    frame_out->payload         = (uint8_t *)data_in;
    frame_out->crc             = 0; /* computed at serialization time, over the wire bytes */

    return 0;
}

/// @brief Serializes a frame descriptor into its on-the-wire byte
/// representation, computing and appending the CRC-32. The CRC covers
/// every field from packet_type through the end of the payload; the
/// leading SOF byte is excluded, since it is a fixed sync marker and
/// carries no information worth protecting.
/// @param frame Frame descriptor to serialize.
/// @param out_buf Destination buffer. Must be at least
/// (METADATA_SIZE + frame->payload_length) bytes long -- callers use a
/// PACKET_MTU-sized buffer, which is always large enough.
/// @returns Total number of bytes written to out_buf.
static uint16_t packetizer_serialize_frame(const packetizer_frame_t * frame, uint8_t * out_buf)
{
    uint16_t offset = 0;

    out_buf[offset++] = frame->sof;
    out_buf[offset++] = frame->packet_type;

    write_be16(&out_buf[offset], frame->payload_length); offset += 2;
    write_be16(&out_buf[offset], frame->sequence_number); offset += 2;
    write_be16(&out_buf[offset], frame->fragment_index);  offset += 2;
    write_be16(&out_buf[offset], frame->fragment_count);  offset += 2;

    if (frame->payload_length > 0 && frame->payload != NULL)
    {
        memcpy(&out_buf[offset], frame->payload, frame->payload_length);
        offset += frame->payload_length;
    }

    uint32_t crc = packetizer_crc32(&out_buf[OFFSET_PACKET_TYPE], offset - OFFSET_PACKET_TYPE);
    write_be32(&out_buf[offset], crc);
    offset += 4;

    return offset;
}

/// @brief Advances the fragment-tracking state machine for one
/// validated DATA-frame fragment and hands its bytes straight to the
/// application via g_state.on_message -- the core never copies
/// fragment data into a message-sized buffer of its own.
/// @param sequence_number Sequence number carried by the frame.
/// @param fragment_index Fragment index carried by the frame.
/// @param fragment_count Fragment count carried by the frame.
/// @param payload Pointer to this fragment's payload bytes.
/// @param payload_length Length, in bytes, of this fragment's payload.
/// @returns Error code
static int packetizer_reassemble_fragment(uint16_t sequence_number,
                                           uint16_t fragment_index,
                                           uint16_t fragment_count,
                                           uint8_t * payload,
                                           uint16_t payload_length)
{
    /* Unfragmented message: nothing to track, deliver right away. */
    if (fragment_count <= 1)
    {
        if (g_state.on_message != NULL)
        {
            g_state.on_message(sequence_number, 0, 1, payload, payload_length);
        }
        return 0;
    }

    /* Start tracking a new message whenever we are idle, or whenever
     * the incoming fragment belongs to a different message than the
     * one currently being tracked (e.g. the previous message was
     * abandoned after losing a fragment). */
    if (!g_reassembly.in_progress || g_reassembly.sequence_number != sequence_number)
    {
        if (fragment_index != 0)
        {
            /* Joined the stream mid-message (fragment 0 was missed or
             * already consumed): there is no valid message to append
             * this fragment to, so discard it. */
            return EBADMSG;
        }

        g_reassembly.in_progress            = 1;
        g_reassembly.sequence_number        = sequence_number;
        g_reassembly.fragment_count         = fragment_count;
        g_reassembly.next_expected_fragment = 0;
    }

    if (fragment_index != g_reassembly.next_expected_fragment)
    {
        /* Out-of-order, duplicated, or gapped fragment: this simple
         * tracker only accepts fragments in order, so the in-progress
         * message is abandoned. Recovering from this (e.g. via
         * retransmission) is left to future work described in the
         * protocol documentation. */
        g_reassembly.in_progress = 0;
        return EBADMSG;
    }

    /* Hand this fragment straight to the application. Reassembling the
     * full message, if the application needs it whole, is now its own
     * responsibility (e.g. writing each fragment directly to a file at
     * the right offset) -- far cheaper on a memory-constrained device
     * than holding the entire message in the packetizer core. */
    if (g_state.on_message != NULL)
    {
        g_state.on_message(sequence_number, fragment_index, fragment_count,
                            payload, payload_length);
    }

    g_reassembly.next_expected_fragment++;

    if (g_reassembly.next_expected_fragment == g_reassembly.fragment_count)
    {
        g_reassembly.in_progress = 0;
    }

    return 0;
}

/// @brief Builds and transmits a PACKET_TYPE_ACK frame acknowledging one
/// received fragment. Callers must only invoke this after the
/// fragment's data has already been handed to, and handled/stored by,
/// the application (see packetizer_message_received_cb) -- never
/// before, since the ACK is the receiver's promise that the data is
/// safe. ACK frames carry no payload; the acknowledged fragment is
/// identified solely by sequence_number/fragment_index/fragment_count,
/// so a single MTU-sized stack buffer (the same one already used for
/// serializing DATA frames) is enough to build and send it -- no
/// additional, larger buffer is needed.
/// @param sequence_number Sequence number of the acknowledged message.
/// @param fragment_index Index of the acknowledged fragment.
/// @param fragment_count Total fragment count of the acknowledged message.
/// @returns Error code
static int packetizer_send_ack(uint16_t sequence_number,
                                uint16_t fragment_index,
                                uint16_t fragment_count)
{
    packetizer_frame_t ack_frame = {0};
    ack_frame.sof             = PACKETIZER_SOF;
    ack_frame.packet_type     = PACKET_TYPE_ACK;
    ack_frame.payload_length  = 0;
    ack_frame.sequence_number = sequence_number;
    ack_frame.fragment_index  = fragment_index;
    ack_frame.fragment_count  = fragment_count;
    ack_frame.payload         = NULL;

    uint8_t ack_wire_buf[PACKET_MTU];
    uint16_t frame_len = packetizer_serialize_frame(&ack_frame, ack_wire_buf);

    if (g_state.send_fn(ack_wire_buf, frame_len) != 0)
    {
        return EIO;
    }

    return 0;
}

int packetizer_init(transport_send send_fn, packetizer_message_received_cb on_message)
{
    if (send_fn == NULL) return EINVAL;

    memset(&g_state, 0, sizeof(g_state));
    g_state.send_fn       = send_fn;
    g_state.on_message    = on_message;
    g_state.is_initialized = 1;

    memset(&g_reassembly, 0, sizeof(g_reassembly));

    return 0;
}

int packetizer_send_data(void * data_in, uint32_t data_len)
{
    if (data_in == NULL || data_len == 0) return EINVAL;
    if (!g_state.is_initialized) return ENODEV;

    uint8_t * bdata = (uint8_t *) data_in;

    /* Number of PAYLOAD_MAX_SIZE-sized fragments needed to carry the
     * whole message, rounding up. Computed in 32 bits first so an
     * oversized message is rejected with a clear error instead of
     * silently wrapping around the 16-bit fragment_count wire field. */
    uint32_t needed_fragments = (data_len + PAYLOAD_MAX_SIZE - 1) / PAYLOAD_MAX_SIZE;
    if (needed_fragments == 0) needed_fragments = 1;
    if (needed_fragments > UINT16_MAX) return EMSGSIZE;

    uint16_t fragment_count = (uint16_t)needed_fragments;

    /* All fragments of this message share the same sequence number;
     * it is what lets the receiver associate them with each other. */
    uint16_t sequence_number = g_state.next_sequence_number++;

    uint8_t wire_buf[PACKET_MTU];

    for (uint16_t fragment_index = 0; fragment_index < fragment_count; fragment_index++)
    {
        uint32_t offset    = (uint32_t)fragment_index * PAYLOAD_MAX_SIZE;
        uint32_t remaining = data_len - offset;
        uint32_t frag_len  = (remaining > PAYLOAD_MAX_SIZE) ? PAYLOAD_MAX_SIZE : remaining;

        packetizer_frame_t frame = {0};
        int rc = packetizer_pack_data(&bdata[offset], frag_len,
                                       fragment_index, fragment_count,
                                       sequence_number, &frame);
        if (rc != 0) return rc;

        uint16_t frame_len = packetizer_serialize_frame(&frame, wire_buf);

        if (g_state.send_fn(wire_buf, frame_len) != 0)
        {
            /* Stop on the first transport failure rather than sending
             * a partial set of fragments the receiver could never
             * reassemble anyway. */
            return EIO;
        }
    }

    return 0;
}

int packetizer_receive_data(void * data_in, uint16_t data_len)
{
    if (data_in == NULL || data_len < METADATA_SIZE) return EINVAL;
    if (!g_state.is_initialized) return ENODEV;

    uint8_t * bytes = (uint8_t *) data_in;

    if (bytes[OFFSET_SOF] != PACKETIZER_SOF)
    {
        /* Does not even look like one of our frames: likely noise
         * from a misbehaving channel. Never hand this to the CRC
         * check or the application. */
        return EBADMSG;
    }

    uint8_t  packet_type     = bytes[OFFSET_PACKET_TYPE];
    uint16_t payload_length  = read_be16(&bytes[OFFSET_PAYLOAD_LENGTH]);
    uint16_t sequence_number = read_be16(&bytes[OFFSET_SEQUENCE_NUMBER]);
    uint16_t fragment_index  = read_be16(&bytes[OFFSET_FRAGMENT_INDEX]);
    uint16_t fragment_count  = read_be16(&bytes[OFFSET_FRAGMENT_COUNT]);

    /* The declared payload length must exactly account for the rest
     * of the datagram; otherwise the frame is malformed or truncated. */
    if ((uint32_t)METADATA_SIZE + payload_length != (uint32_t)data_len)
    {
        return EBADMSG;
    }

    uint8_t * payload      = &bytes[OFFSET_PAYLOAD];
    uint32_t received_crc  = read_be32(&bytes[OFFSET_PAYLOAD + payload_length]);
    uint32_t computed_crc  = packetizer_crc32(&bytes[OFFSET_PACKET_TYPE],
                                               (uint32_t)(OFFSET_PAYLOAD + payload_length - OFFSET_PACKET_TYPE));

    if (received_crc != computed_crc)
    {
        /* Corrupted in transit: per the user stories, this must never
         * be delivered to the application as valid data. */
        return EBADMSG;
    }

    switch (packet_type)
    {
        case PACKET_TYPE_DATA:
        {
            int rc = packetizer_reassemble_fragment(sequence_number, fragment_index,
                                                     fragment_count, payload, payload_length);
            if (rc == 0)
            {
                /* The application has already handled and stored the
                 * fragment's data inside packetizer_reassemble_fragment
                 * (its on_message callback already returned by this
                 * point), so it is now safe to acknowledge it. */
                packetizer_send_ack(sequence_number, fragment_index, fragment_count);
            }
            return rc;
        }

        case PACKET_TYPE_ACK:
            /* TODO: delivery confirmation and retransmission handling
             * on the sender side (user story #2) is reserved for a
             * future iteration. */
            return 0;

        default:
            return EBADMSG;
    }
}