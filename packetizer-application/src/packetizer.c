/* Needed under -std=c11 to expose clock_gettime(), CLOCK_MONOTONIC and
 * nanosleep() from <time.h>, used by the ACK wait/timeout logic below. */
#define _POSIX_C_SOURCE 200809L

#include "packetizer.h"
#include <errno.h>
#include <string.h>
#include <time.h>
#include "log.h"

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


#define packetizer_send_ack(seq,frag,count) \
    packetizer_send_control(PACKET_TYPE_ACK, seq, frag, count)

#define packetizer_send_nack(seq,frag,count,reason) \
    packetizer_send_control(PACKET_TYPE_NACK, seq, frag, count)


/// @brief Global state of the packetizer core: the callbacks registered
/// by packetizer_init() and the sequence number counter used to tag
/// outgoing messages. Kept private (static) to this translation unit,
/// as it is purely an implementation detail of the core, not something
/// callers should reach into directly.
typedef struct
{
    transport_send_t send_fn;                     /* registered transport send function */
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

/// @brief State used to track the single DATA fragment, if any, that
/// packetizer_send_data() is currently waiting on an ACK for. Only one
/// fragment is ever in flight awaiting acknowledgment at a time (the
/// stop-and-wait scheme documented on packetizer_send_data()), so this
/// is a handful of fields rather than a table sized for many
/// concurrent outstanding fragments -- consistent with the rest of
/// this core's resource-constrained design.
///
/// `acked` is written by packetizer_receive_data() (typically called
/// from a different thread, e.g. a dedicated receiver thread) and
/// polled by packetizer_send_data() while it waits; `volatile` is
/// enough to keep the compiler from caching it in a register across
/// that busy-wait loop. This is a deliberately simple mechanism: it
/// assumes a single message is ever being sent at a time, matching the
/// point-to-point demo applications built on this core. A fully
/// concurrent, multi-message-in-flight sender would need proper
/// synchronization (e.g. a mutex or C11 atomics) around this state.
typedef struct
{
    uint16_t sequence_number;  /* sequence number of the fragment awaiting ACK */
    uint16_t fragment_index;   /* index of the fragment awaiting ACK */
    int waiting;                /* 1 while packetizer_send_data() is actively waiting */
    volatile int acked;         /* set to 1 once the matching ACK is received */
    uint8_t nack_count;
} packetizer_pending_ack_t;

static packetizer_pending_ack_t g_pending_ack;

/// @brief Computes a CRC-32 (IEEE 802.3 polynomial, 0xEDB88320) over a
/// block of bytes.
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
/// @param received Frame received by the transport layer
/// @returns Error code
static int packetizer_reassemble_fragment(packetizer_frame_t received)
{
    /* Unfragmented message: nothing to track, deliver right away. */
    if (received.fragment_count <= 1)
    {
        if (g_state.on_message != NULL)
        {
            if(received.sequence_number == g_reassembly.sequence_number)
            {
                LOG_ERROR("Repeated message detected, ignoring");
                return 0;
            }

            /* What sequence_number should legitimately come right
             * after g_reassembly.sequence_number, accounting for
             * wraparound. packetizer_send_data() skips 0 when its
             * counter wraps (0 is reserved as this tracker's initial
             * "nothing seen yet" sentinel), so the expected successor
             * of 65535 is 1, not 0 or 65536. Computing this in a
             * wider-than-16-bit type first, instead of evaluating
             * g_reassembly.sequence_number + 1 directly in uint16_t,
             * avoids that addition itself silently overflowing back
             * to 0 right at the boundary -- which previously made the
             * very next, perfectly in-order message look like a
             * "lost messages" jump every time the counter wrapped. */
            uint32_t expected_next = (uint32_t)g_reassembly.sequence_number + 1u;
            if (expected_next > UINT16_MAX) expected_next = 1u;

            if(received.sequence_number != (uint16_t)expected_next)
            {
                LOG_ERROR("Sender was on message n %u, this device was on %u", received.sequence_number, g_reassembly.sequence_number);
            }

            /* Always advance the tracker to the message just accepted,
             * not only inside the jump-detection branch above.
             * Otherwise, on every normal in-order arrival the tracker
             * is left one message stale, which makes the *next*
             * arrival look like a false 2-apart "jump" even though
             * nothing was actually lost. */
            g_reassembly.sequence_number = received.sequence_number;
            
            received.fragment_index = 0;
            received.fragment_count = 1;
            g_state.on_message(received);
        }
        return 0;
    }

    /* Start tracking a new message whenever we are idle, or whenever
     * the incoming fragment belongs to a different message than the
     * one currently being tracked (e.g. the previous message was
     * abandoned after losing a fragment). */
    if (!g_reassembly.in_progress || g_reassembly.sequence_number != received.sequence_number)
    {
        if (received.fragment_index != 0)
        {
            /* Joined the stream mid-message (fragment 0 was missed or
             * already consumed): there is no valid message to append
             * this fragment to, so discard it. */
            LOG_ERROR("Invalid fragment index");
            return EBADMSG;
        }

        g_reassembly.in_progress            = 1;
        g_reassembly.sequence_number        = received.sequence_number;
        g_reassembly.fragment_count         = received.fragment_count;
        g_reassembly.next_expected_fragment = 0;
    }

    if (received.fragment_index != g_reassembly.next_expected_fragment)
    {
        /* Out-of-order, duplicated, or gapped fragment: this simple
         * tracker only accepts fragments in order, so the in-progress
         * message is abandoned. Recovering from this (e.g. via
         * retransmission) is left to future work described in the
         * protocol documentation. */
        LOG_ERROR("Invalid fragment index. Expected %u, got %u", g_reassembly.next_expected_fragment, received.fragment_index);
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
        LOG_DEBUG("Passing fragment %u to application", received.fragment_index);
        g_state.on_message(received);
    }

    g_reassembly.next_expected_fragment++;

    if (g_reassembly.next_expected_fragment == g_reassembly.fragment_count)
    {
        LOG_DEBUG("Finished fragment parsing at %u fragments", received.fragment_index);
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
static int packetizer_send_control(
    uint8_t type,
    uint16_t sequence_number,
    uint16_t fragment_index,
    uint16_t fragment_count)
{
    packetizer_frame_t frame = {0};

    frame.sof             = PACKETIZER_SOF;
    frame.packet_type     = type;
    frame.sequence_number = sequence_number;
    frame.fragment_index  = fragment_index;
    frame.fragment_count  = fragment_count;

    uint8_t wire_buf[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire_buf);

    if(g_state.send_fn(wire_buf, len) != 0)
        return EIO;

    return 0;
}

/// @brief Returns a monotonically increasing timestamp in milliseconds.
/// Used solely to measure elapsed time while waiting for an ACK; kept
/// in its own function so porting this core to a platform without
/// clock_gettime()/CLOCK_MONOTONIC only requires changing this one place.
/// @returns Current monotonic time, in milliseconds.
static uint64_t packetizer_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000u + (uint64_t)(ts.tv_nsec / 1000000);
}

/// @brief Blocks until the ACK for one specific DATA fragment arrives,
/// retransmitting it on timeout, up to PACKETIZER_ACK_MAX_RETRIES times.
/// Implements the stop-and-wait scheme documented on packetizer_send_data().
///
/// The wait itself does not spin at full CPU: it sleeps in small
/// increments, re-checking the (volatile) g_pending_ack.acked flag that
/// packetizer_receive_data() sets when the matching ACK comes in.
/// @param sequence_number Sequence number of the fragment sent.
/// @param fragment_index Index of the fragment sent.
/// @param wire_buf Serialized bytes of the fragment, kept around so it
/// can be retransmitted verbatim without re-serializing it.
/// @param frame_len Length, in bytes, of wire_buf.
/// @returns 0 if the ACK was received in time; ETIMEDOUT if every retry
/// was exhausted without one; EIO if a retransmit itself failed to send.
static int packetizer_wait_for_ack(uint16_t sequence_number, uint16_t fragment_index,
                                    uint8_t * wire_buf, uint16_t frame_len)
{
    g_pending_ack.sequence_number = sequence_number;
    g_pending_ack.fragment_index  = fragment_index;
    g_pending_ack.acked           = 0;
    g_pending_ack.nack_count = 0;
    g_pending_ack.waiting         = 1;

    /* Attempt 0 is the transmission packetizer_send_data() already did
     * before calling this function; the remaining attempts are
     * retransmissions after a timeout. */
    for (uint32_t attempt = 0; attempt <= PACKETIZER_ACK_MAX_RETRIES; attempt++)
    {
        uint64_t deadline = packetizer_now_ms() + PACKETIZER_ACK_TIMEOUT_MS;

        while (packetizer_now_ms() < deadline)
        {
            if (g_pending_ack.acked == 1)
            {
                g_pending_ack.waiting = 0;
                return 0;
            }

            if(g_pending_ack.acked == -1)
            {
                g_pending_ack.nack_count++;

                LOG_ERROR("NACK received (%u/%u)",
                        g_pending_ack.nack_count,
                        PACKETIZER_NACK_MAX_RETRIES);

                if (g_pending_ack.nack_count >= PACKETIZER_NACK_MAX_RETRIES)
                {
                    LOG_ERROR("Fragment rejected too many times. Aborting transmission.");

                    g_pending_ack.waiting = 0;
                    return EBADMSG;
                }

                if (g_state.send_fn(wire_buf, frame_len) != 0)
                {
                    g_pending_ack.waiting = 0;
                    return EIO;
                }

                /* Give this retransmission its own fresh wait: reset
                 * the flag so the next poll iteration only reacts to a
                 * genuinely new response (ACK or NACK), instead of
                 * immediately re-processing this same NACK again on
                 * the next 1ms tick and burning through the entire
                 * retry budget in a handful of milliseconds. */
                g_pending_ack.acked = 0;
                deadline = packetizer_now_ms() + PACKETIZER_ACK_TIMEOUT_MS;
            }


            /* Sleep briefly */
            struct timespec poll_interval = { .tv_sec = 0, .tv_nsec = 1000000 }; /* 1 ms */
            nanosleep(&poll_interval, NULL);
        }

        if (attempt < PACKETIZER_ACK_MAX_RETRIES)
        {
            LOG_ERROR("ACK timeout for seq %u frag %u, retransmitting (attempt %u/%u)",
                       sequence_number, fragment_index, attempt + 1, PACKETIZER_ACK_MAX_RETRIES);

            if (g_state.send_fn(wire_buf, frame_len) != 0)
            {
                LOG_ERROR("Retransmission failed for seq %u frag %u", sequence_number, fragment_index);
                g_pending_ack.waiting = 0;
                return EIO;
            }
        }
    }

    LOG_ERROR("Giving up on seq %u frag %u after %u attempts",
               sequence_number, fragment_index, PACKETIZER_ACK_MAX_RETRIES);
    g_pending_ack.waiting = 0;
    return ETIMEDOUT;
}


int packetizer_init(transport_send_t send_fn, packetizer_message_received_cb on_message)
{
    if (send_fn == NULL || on_message == NULL) return EINVAL;

    memset(&g_state, 0, sizeof(g_state));
    g_state.send_fn       = send_fn;
    g_state.on_message    = on_message;
    g_state.is_initialized = 1;

    memset(&g_reassembly, 0, sizeof(g_reassembly));
    memset(&g_pending_ack, 0, sizeof(g_pending_ack));
    LOG_DEBUG("Packetizer initialized");
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
    if(g_state.next_sequence_number == 0) g_state.next_sequence_number = 1;
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
        LOG_DEBUG("Sending %u bytes. Frag %u/%u", frame.payload_length, frame.fragment_index, frame.fragment_count);
        if (g_state.send_fn(wire_buf, frame_len) != 0)
        {
            /* Stop on the first transport failure rather than sending
             * a partial set of fragments the receiver could never
             * reassemble anyway. */
            LOG_ERROR("Transport layer send failed");
            return EIO;
        }

        /* Stop-and-wait: don't move on to the next fragment until this
         * one is confirmed delivered (or we've exhausted every retry). */
        int ack_rc = packetizer_wait_for_ack(sequence_number, fragment_index, wire_buf, frame_len);
        if (ack_rc != 0)
        {
            return ack_rc;
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
        LOG_ERROR("No SOF");
        return EBADMSG;
    }

    packetizer_frame_t received = {0};
    received.packet_type     = bytes[OFFSET_PACKET_TYPE];
    received.payload_length  = read_be16(&bytes[OFFSET_PAYLOAD_LENGTH]);
    received.sequence_number = read_be16(&bytes[OFFSET_SEQUENCE_NUMBER]);
    received.fragment_index  = read_be16(&bytes[OFFSET_FRAGMENT_INDEX]);
    received.fragment_count  = read_be16(&bytes[OFFSET_FRAGMENT_COUNT]);

    /* The declared payload length must exactly account for the rest
     * of the datagram; otherwise the frame is malformed or truncated. */
    if ((uint32_t)METADATA_SIZE + received.payload_length != (uint32_t)data_len)
    {
        LOG_ERROR("Invalid message size: %u", data_len);
        return EBADMSG;
    }

    

    received.payload = &bytes[OFFSET_PAYLOAD];
    received.crc  = read_be32(&bytes[OFFSET_PAYLOAD + received.payload_length]);
    uint32_t computed_crc  = packetizer_crc32(&bytes[OFFSET_PACKET_TYPE],
                                               (uint32_t)(OFFSET_PAYLOAD + received.payload_length - OFFSET_PACKET_TYPE));

    if (received.crc != computed_crc)
    {
        /* Corrupted in transit: per the user stories, this must never
         * be delivered to the application as valid data. */
        LOG_ERROR("Invalid CRC --- Calced:%x --- Received:%x", computed_crc, received.crc);
        return EBADMSG;
    }

    switch (received.packet_type)
    {
        case PACKET_TYPE_DATA:
        {
            LOG_DEBUG("[RX DATA %u]", received.sequence_number);
            int rc = packetizer_reassemble_fragment(received);
            if (rc == 0)
            {
                /* The application has already handled and stored the
                 * fragment's data inside packetizer_reassemble_fragment
                 * (its on_message callback already returned by this
                 * point), so it is now safe to acknowledge it. */
                packetizer_send_ack(received.sequence_number, received.fragment_index, received.fragment_count);
            }
            return rc;
        }

        case PACKET_TYPE_ACK:
            if (g_pending_ack.waiting &&
                g_pending_ack.sequence_number == received.sequence_number &&
                g_pending_ack.fragment_index == received.fragment_index)
            {
                LOG_DEBUG("ACK received for seq %u frag %u", received.sequence_number, received.fragment_index);
                g_pending_ack.acked = 1;
            }
            else
            {
                /* Stray, late (arrived after we already gave up or
                 * moved on), or duplicated ACK: nothing to unblock. */
                LOG_DEBUG("Ignoring unmatched ACK for seq %u frag %u", received.sequence_number, received.fragment_index);
            }
            return 0;

        case PACKET_TYPE_NACK:
            
            LOG_ERROR("NACK received.");

            if(g_pending_ack.waiting &&
            g_pending_ack.sequence_number == received.sequence_number &&
            g_pending_ack.fragment_index == received.fragment_index)
            {
                g_pending_ack.acked = -1;
            }

        return 0;

        default:
            return EBADMSG;
    }
}