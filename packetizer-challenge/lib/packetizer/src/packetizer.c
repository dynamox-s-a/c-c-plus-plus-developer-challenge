#include "../include/packetizer.h"
#include "packetizer_internal.h"
#include "cobs.h"
#include "crc16.h"

#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Internal structure                                                          */
/* -------------------------------------------------------------------------- */

struct packetizer_s {
    /* --- Configuration provided by the user --- */
    pkt_config_t cfg;

    /* --- TX state --- */
    tx_state_t   tx_state;
    uint8_t      tx_msg_id;      /* message counter, wraps to 0 after 255 */
    uint8_t      tx_frag_idx;    /* index of the fragment currently being sent */
    uint8_t      tx_frag_total;  /* total fragments for the current message */
    uint8_t      tx_retries;     /* number of retransmissions performed so far */
    uint32_t     tx_deadline;    /* deadline to receive the ACK (in ms) */
    uint8_t*     tx_msg_buf;     /* heap-allocated copy of the message */
    uint16_t     tx_msg_len;     /* message length in bytes */

    /* --- Byte-stream accumulator for received data (RX) --- */
    uint8_t      rx_raw[PKT_RX_BUF_SIZE];
    size_t       rx_raw_len;

    /* --- Reassembly slots for incoming messages (RX) --- */
    rx_slot_t    rx_slots[PKT_RX_SLOTS];

    /* Ring of recently completed MSG_IDs — allows ACKing late duplicates
     * without delivering the same message twice. */
    uint8_t      rx_done_ids[PKT_RX_SLOTS];
    size_t       rx_done_head;
    size_t       rx_done_count;
};

/* -------------------------------------------------------------------------- */
/* Internal helper functions                                                   */
/* -------------------------------------------------------------------------- */

/** Builds and transmits a DATA packet for the fragment at index frag_idx. */
static int tx_send_fragment(packetizer_t* pkt, uint8_t frag_idx)
{
    /* --- Calculate the byte range of the message this fragment carries --- */
    uint16_t max_pl  = pkt->cfg.max_payload;
    uint16_t msg_len = pkt->tx_msg_len;

    uint16_t offset = (uint16_t)(frag_idx * max_pl);   /* first byte in the message */
    uint16_t pl_len = (uint16_t)(msg_len - offset);    /* remaining bytes */
    if (pl_len > max_pl) pl_len = max_pl;               /* cap to maximum payload */

    /* --- Set fragment position flags within the message --- */
    uint8_t flags = 0;
    if (frag_idx == 0)              flags |= PKT_FLAG_FIRST; /* this is the first fragment */
    if (offset + pl_len >= msg_len) flags |= PKT_FLAG_LAST;  /* this is the last fragment */

    /* --- Build the packet header in the local buffer --- */
    uint8_t raw[PKT_RAW_MAX];
    raw[PKT_HDR_TYPE]        = PKT_TYPE_DATA;
    raw[PKT_HDR_MSG_ID]      = pkt->tx_msg_id;
    raw[PKT_HDR_FRAG_IDX]    = frag_idx;
    raw[PKT_HDR_FLAGS]       = flags;
    raw[PKT_HDR_MSG_LEN_HI]  = (frag_idx == 0) ? (uint8_t)(msg_len >> 8)   : 0;
    raw[PKT_HDR_MSG_LEN_LO]  = (frag_idx == 0) ? (uint8_t)(msg_len & 0xFF) : 0;
    raw[PKT_HDR_PAYLOAD_LEN] = (uint8_t)pl_len;

    /* --- Copy the fragment bytes immediately after the header --- */
    memcpy(&raw[PKT_HDR_SIZE], &pkt->tx_msg_buf[offset], pl_len);

    /* --- Compute and append the CRC-16 at the end of the packet --- */
    size_t raw_len   = PKT_HDR_SIZE + pl_len;
    uint16_t crc     = crc16_ccitt(raw, raw_len);
    raw[raw_len++]   = (uint8_t)(crc >> 8);    /* CRC high byte */
    raw[raw_len++]   = (uint8_t)(crc & 0xFF);  /* CRC low byte */

    /* --- COBS-encode: ensures 0x00 only appears as the frame delimiter --- */
    uint8_t encoded[COBS_ENCODED_MAX(PKT_RAW_MAX)];
    size_t enc_len = cobs_encode(encoded, raw, raw_len);

    /* --- Deliver the encoded bytes to the user-configured transport --- */
    return pkt->cfg.write(encoded, enc_len, pkt->cfg.user_ctx);
}

/** Builds and sends an ACK or NAK packet for the given message and fragment. */
static int tx_send_ack(packetizer_t* pkt, uint8_t type, uint8_t msg_id, uint8_t frag_idx)
{
    /* --- Build the header: ACK/NAK carries no payload, only control fields --- */
    uint8_t raw[PKT_OVERHEAD];
    raw[PKT_HDR_TYPE]        = type;       /* PKT_TYPE_ACK or PKT_TYPE_NAK */
    raw[PKT_HDR_MSG_ID]      = msg_id;
    raw[PKT_HDR_FRAG_IDX]    = frag_idx;
    raw[PKT_HDR_FLAGS]       = 0;
    raw[PKT_HDR_MSG_LEN_HI]  = 0;
    raw[PKT_HDR_MSG_LEN_LO]  = 0;
    raw[PKT_HDR_PAYLOAD_LEN] = 0;

    /* --- Compute and append the CRC-16 at the end of the packet --- */
    size_t raw_len   = PKT_HDR_SIZE;
    uint16_t crc     = crc16_ccitt(raw, raw_len);
    raw[raw_len++]   = (uint8_t)(crc >> 8);    /* high byte */
    raw[raw_len++]   = (uint8_t)(crc & 0xFF);  /* low byte */

    /* --- COBS-encode and deliver to transport --- */
    uint8_t encoded[COBS_ENCODED_MAX(PKT_OVERHEAD)];
    size_t enc_len = cobs_encode(encoded, raw, raw_len);

    return pkt->cfg.write(encoded, enc_len, pkt->cfg.user_ctx);
}

/** Finds an active reassembly slot for msg_id, or allocates a free one. */
static rx_slot_t* rx_get_slot(packetizer_t* pkt, uint8_t msg_id)
{
    /* --- First pass: look for a slot already active for this msg_id --- */
    for (size_t i = 0; i < PKT_RX_SLOTS; i++) {
        if (pkt->rx_slots[i].active && pkt->rx_slots[i].msg_id == msg_id) {
            return &pkt->rx_slots[i];
        }
    }

    /* --- Second pass: allocate any free slot --- */
    for (size_t i = 0; i < PKT_RX_SLOTS; i++) {
        if (!pkt->rx_slots[i].active) {
            return &pkt->rx_slots[i];
        }
    }

    return NULL; /* all slots are in use */
}

/** Frees a reassembly slot and zeroes its memory. */
static void rx_free_slot(rx_slot_t* slot)
{
    /* --- Free the reassembly buffer and reset the slot (active back to 0) --- */
    free(slot->buf);
    memset(slot, 0, sizeof(*slot));
}

/** Processes a raw packet that has been COBS-decoded and CRC-validated. */
static void process_packet(packetizer_t* pkt, const uint8_t* raw, size_t len)
{
    /* --- Minimum sanity check: discard packets too short to hold a header --- */
    if (len < PKT_OVERHEAD) return;

    /* --- Extract header fields into readable local variables --- */
    uint8_t type     = raw[PKT_HDR_TYPE];
    uint8_t msg_id   = raw[PKT_HDR_MSG_ID];
    uint8_t frag_idx = raw[PKT_HDR_FRAG_IDX];
    uint8_t flags    = raw[PKT_HDR_FLAGS];
    uint16_t msg_len = (uint16_t)(((uint16_t)raw[PKT_HDR_MSG_LEN_HI] << 8) |
        raw[PKT_HDR_MSG_LEN_LO]);
    uint8_t pl_len   = raw[PKT_HDR_PAYLOAD_LEN];

    /* --- Verify that the total length matches what the header declares --- */
    size_t expected_raw_len = PKT_HDR_SIZE + pl_len + PKT_CRC_SIZE;
    if (len != expected_raw_len) return;

    const uint8_t* payload = &raw[PKT_HDR_SIZE];

    /* ------------------------------------------------------------------ */
    /* ACK — fragment delivery confirmation                               */
    /* ------------------------------------------------------------------ */
    if (type == PKT_TYPE_ACK) {
        if (pkt->tx_state == TX_WAIT_ACK &&
            msg_id == pkt->tx_msg_id &&
            frag_idx == pkt->tx_frag_idx) {

            uint8_t last_frag = (uint8_t)(pkt->tx_frag_total - 1u);

            if (frag_idx == last_frag) {
                /* Last fragment confirmed: message delivered successfully */
                pkt->tx_state = TX_IDLE;
                free(pkt->tx_msg_buf);
                pkt->tx_msg_buf = NULL;
                if (pkt->cfg.on_status) {
                    pkt->cfg.on_status(pkt->tx_msg_id,
                        PKT_STATUS_OK,
                        pkt->cfg.user_ctx);
                }
            }
            else {
                /* Partial ACK: advance to the next fragment */
                pkt->tx_frag_idx++;
                pkt->tx_retries = 0;
                tx_send_fragment(pkt, pkt->tx_frag_idx);
                pkt->tx_deadline = 0; /* timer re-armed on the next pkt_tick() call */
            }
        }
        return;
    }

    /* ------------------------------------------------------------------ */
    /* NAK — rejection: receiver requested retransmission                 */
    /* ------------------------------------------------------------------ */
    if (type == PKT_TYPE_NAK) {
        if (pkt->tx_state == TX_WAIT_ACK &&
            msg_id == pkt->tx_msg_id &&
            frag_idx == pkt->tx_frag_idx) {
            /* Treat as timeout: force immediate retransmission */
            pkt->tx_retries = pkt->cfg.max_retries; /* forces the retransmission code path */
            pkt->tx_deadline = 0;
        }
        return;
    }

    /* ------------------------------------------------------------------ */
    /* DATA — data fragment received                                       */
    /* ------------------------------------------------------------------ */
    if (type == PKT_TYPE_DATA) {
        /* Check whether this msg_id was already delivered (late duplicate) */

        for (size_t i = 0; i < pkt->rx_done_count; i++) {
            size_t idx = (pkt->rx_done_head + PKT_RX_SLOTS - pkt->rx_done_count + i)
                % PKT_RX_SLOTS;
            if (pkt->rx_done_ids[idx] == msg_id) {
                tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);
                return;
            }
        }

        rx_slot_t* slot = rx_get_slot(pkt, msg_id);
        if (!slot) {
            /* No free slot: send NAK so the sender can retry later */
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }

        /* --- Initialise the slot upon receiving the first fragment --- */
        if (flags & PKT_FLAG_FIRST) {
            if (slot->active && slot->msg_id == msg_id) {
                /* Duplicate first fragment (retransmission): ACK and ignore */
                tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);
                return;
            }
            if (msg_len == 0 || (uint32_t)msg_len > PKT_MAX_MSG_SIZE) {
                tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
                return;
            }
            slot->buf = (uint8_t*)malloc(msg_len);
            if (!slot->buf) {
                tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
                return;
            }
            slot->msg_len = msg_len;
            slot->received = 0;
            slot->msg_id = msg_id;
            slot->next_frag = 0;
            slot->active = 1;
        }

        if (!slot->active || slot->msg_id != msg_id) {
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }

        /* --- Detect duplicate or out-of-order fragment --- */
        if (frag_idx < slot->next_frag) {
            /* Already received this fragment: silently re-ACK */
            tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);
            return;
        }
        if (frag_idx != slot->next_frag) {
            /* Out-of-order fragment: request retransmission via NAK */
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }

        /* --- Copy the payload into the reassembly buffer --- */
        if ((uint32_t)slot->received + pl_len > slot->msg_len) {
            /* Overflow: payload exceeds the declared size — corrupted message */
            rx_free_slot(slot);
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }
        memcpy(&slot->buf[slot->received], payload, pl_len);
        slot->received = (uint16_t)(slot->received + pl_len);
        slot->next_frag = (uint8_t)(slot->next_frag + 1u);

        /* Acknowledge receipt of this fragment */
        tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);

        /* --- Deliver the complete message upon receiving the last fragment --- */
        if (flags & PKT_FLAG_LAST) {
            if (slot->received == slot->msg_len) {
                pkt->cfg.on_message(slot->buf, slot->received,
                    pkt->cfg.user_ctx);
            }
            /* Record msg_id as completed so late duplicates can be ACKed */
            pkt->rx_done_ids[pkt->rx_done_head] = msg_id;
            pkt->rx_done_head = (pkt->rx_done_head + 1u) % PKT_RX_SLOTS;
            if (pkt->rx_done_count < PKT_RX_SLOTS) pkt->rx_done_count++;
            rx_free_slot(slot);
        }
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                  */
/* -------------------------------------------------------------------------- */

packetizer_t* pkt_create(const pkt_config_t* cfg)
{
    /* --- Validate mandatory callbacks before allocating any resource --- */
    if (!cfg || !cfg->write || !cfg->on_message) return NULL;

    /* --- Allocate and initialise the structure on the heap (calloc zeroes all fields) --- */
    packetizer_t* pkt = (packetizer_t*)calloc(1, sizeof(*pkt));
    if (!pkt) return NULL;
    pkt->cfg = *cfg;

    /* --- Apply default values for fields left unset by the caller --- */
    if (pkt->cfg.max_payload == 0 || pkt->cfg.max_payload > PKT_MAX_PAYLOAD) {
        pkt->cfg.max_payload = PKT_MAX_PAYLOAD;
    }
    if (pkt->cfg.timeout_ms == 0) {
        pkt->cfg.timeout_ms = 500;
    }
    if (pkt->cfg.max_retries == 0) {
        pkt->cfg.max_retries = 5;
    }

    return pkt;
}

void pkt_destroy(packetizer_t* pkt)
{
    if (!pkt) return;

    /* --- Free the TX buffer if a message is currently in flight --- */
    free(pkt->tx_msg_buf);

    /* --- Free the reassembly buffers for all RX slots --- */
    for (size_t i = 0; i < PKT_RX_SLOTS; i++) {
        free(pkt->rx_slots[i].buf);
    }

    /* --- Free the main structure --- */
    free(pkt);
}

int pkt_send(packetizer_t* pkt, const uint8_t* data, size_t len, uint8_t* msg_id)
{
    /* --- Validate parameters and prevent concurrent sends --- */
    if (!pkt || !data || len == 0 || len > PKT_MAX_MSG_SIZE) return -1;
    if (pkt->tx_state != TX_IDLE) return -1;

    /* --- Copy the message to an internal buffer (library owns the memory) --- */
    pkt->tx_msg_buf = (uint8_t*)malloc(len);
    if (!pkt->tx_msg_buf) return -1;
    memcpy(pkt->tx_msg_buf, data, len);
    pkt->tx_msg_len = (uint16_t)len;

    /* --- Initialise the TX state for this message --- */
    pkt->tx_msg_id++;           /* 0-255 counter, wraps to zero naturally */
    pkt->tx_frag_idx  = 0;      /* next fragment to send */
    pkt->tx_retries   = 0;      /* retransmission counter */
    pkt->tx_deadline  = 0;      /* timer is armed on the first pkt_tick() call */

    /* --- Calculate how many fragments will be needed --- */
    uint16_t max_pl = pkt->cfg.max_payload;
    pkt->tx_frag_total = (uint8_t)(((uint16_t)len + max_pl - 1u) / max_pl);

    /* --- Mark as in-flight and return the message ID to the caller --- */
    pkt->tx_state = TX_WAIT_ACK;
    if (msg_id) *msg_id = pkt->tx_msg_id;

    /* --- Send the first fragment; remaining ones are sent as ACKs arrive --- */
    return tx_send_fragment(pkt, 0);
}

int pkt_feed(packetizer_t* pkt, const uint8_t* data, size_t len)
{
    if (!pkt || !data) return -1;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        /* --- Byte 0x00: COBS frame delimiter — process the accumulated frame --- */
        if (byte == 0x00) {
            if (pkt->rx_raw_len > 0) {
                /* --- COBS-decode; result is always smaller than the input --- */
                uint8_t decoded[PKT_RX_BUF_SIZE];
                size_t dec_len = cobs_decode(decoded,
                    pkt->rx_raw,
                    pkt->rx_raw_len);

                if (dec_len >= PKT_OVERHEAD) {
                    /* --- Validate the received CRC against the recomputed CRC --- */
                    size_t payload_end = dec_len - PKT_CRC_SIZE;
                    uint16_t received_crc =
                        (uint16_t)(((uint16_t)decoded[payload_end] << 8) |
                            decoded[payload_end + 1]);
                    uint16_t computed_crc = crc16_ccitt(decoded, payload_end);

                    if (received_crc == computed_crc) {
                        process_packet(pkt, decoded, dec_len);
                    }
                    /* Corrupted packet: silently discard.
                     * The sender will detect the timeout and retransmit. */
                }
                pkt->rx_raw_len = 0;
            }
        }
        /* --- Data byte: accumulate in the buffer until the next delimiter --- */
        else {
            if (pkt->rx_raw_len < PKT_RX_BUF_SIZE) {
                pkt->rx_raw[pkt->rx_raw_len++] = byte;
            }
            else {
                /* Buffer overflow: reset and wait for the next frame */
                pkt->rx_raw_len = 0;
            }
        }
    }

    return 0;
}

void pkt_tick(packetizer_t* pkt, uint32_t now_ms)
{
    /* --- Only work to do if a message is waiting for an ACK --- */
    if (!pkt || pkt->tx_state != TX_WAIT_ACK) return;

    /* --- First call after sending: arm the timer by setting the deadline --- */
    if (pkt->tx_deadline == 0) {
        pkt->tx_deadline = now_ms + pkt->cfg.timeout_ms;
        return;
    }

    /* --- Deadline has not elapsed yet: nothing to do --- */
    if ((int32_t)(now_ms - pkt->tx_deadline) < 0) {
        return;
    }

    /* --- Timeout elapsed: ACK did not arrive in time --- */
    if (pkt->tx_retries >= pkt->cfg.max_retries) {
        /* --- Retries exhausted: cancel transmission and notify the application --- */
        pkt->tx_state = TX_IDLE;
        free(pkt->tx_msg_buf);
        pkt->tx_msg_buf = NULL;
        if (pkt->cfg.on_status) {
            pkt->cfg.on_status(pkt->tx_msg_id,
                PKT_STATUS_ERROR,
                pkt->cfg.user_ctx);
        }
        return;
    }

    /* --- Retransmit the fragment and restart the timer for the next attempt --- */
    pkt->tx_retries++;
    pkt->tx_deadline = now_ms + pkt->cfg.timeout_ms;
    tx_send_fragment(pkt, pkt->tx_frag_idx);
}

int pkt_is_busy(const packetizer_t* pkt)
{
    /* --- Return 1 if a message is in-flight waiting for ACK, 0 otherwise --- */
    return (pkt && pkt->tx_state != TX_IDLE) ? 1 : 0;
}
