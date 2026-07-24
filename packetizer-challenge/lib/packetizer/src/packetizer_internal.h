/**
 * @file packetizer_internal.h
 * @brief Internal packet wire format and state definitions.
 *
 * Wire format (before COBS encoding / after COBS decoding):
 *
 *  Byte  0    : TYPE       — packet type (PKT_TYPE_*)
 *  Byte  1    : MSG_ID     — rolling message identifier (0–255)
 *  Byte  2    : FRAG_IDX  — fragment index within this message (0–255)
 *  Byte  3    : FLAGS      — PKT_FLAG_* bitmask
 *  Bytes 4–5  : MSG_LEN    — total message length in bytes (big-endian),
 *                            only meaningful in the FIRST fragment; 0 elsewhere
 *  Byte  6    : PAYLOAD_LEN — number of payload bytes in this packet (0–240)
 *  Bytes 7…   : PAYLOAD    — application data (PAYLOAD_LEN bytes)
 *  Last 2     : CRC16      — CRC-16/CCITT over bytes 0..(7+PAYLOAD_LEN-1),
 *                            big-endian
 *
 * Total per-packet overhead: 9 bytes (7 header + 2 CRC).
 *
 * ACK / NAK packets carry no payload (PAYLOAD_LEN = 0).
 * MSG_ID + FRAG_IDX identify the fragment being acknowledged.
 */

#ifndef PACKETIZER_INTERNAL_H
#define PACKETIZER_INTERNAL_H

#include <stddef.h>
#include <stdint.h>

/* ---- Packet types -------------------------------------------------------- */
#define PKT_TYPE_DATA  0x01U
#define PKT_TYPE_ACK   0x02U
#define PKT_TYPE_NAK   0x03U

/* ---- Flag bits ----------------------------------------------------------- */
#define PKT_FLAG_FIRST 0x01U   /* First fragment of a message */
#define PKT_FLAG_LAST  0x02U   /* Last fragment of a message  */

/* ---- Header layout ------------------------------------------------------- */
#define PKT_HDR_TYPE         0
#define PKT_HDR_MSG_ID       1
#define PKT_HDR_FRAG_IDX     2
#define PKT_HDR_FLAGS        3
#define PKT_HDR_MSG_LEN_HI   4
#define PKT_HDR_MSG_LEN_LO   5
#define PKT_HDR_PAYLOAD_LEN  6
#define PKT_HDR_SIZE         7   /* bytes before payload */
#define PKT_CRC_SIZE         2
#define PKT_OVERHEAD         (PKT_HDR_SIZE + PKT_CRC_SIZE)   /* 9 bytes */

/* Max raw (pre-COBS) packet size */
#define PKT_RAW_MAX          (PKT_HDR_SIZE + 240U + PKT_CRC_SIZE)  /* 249 */

/* RX accumulation buffer: enough for one COBS-encoded max packet */
/* COBS_ENCODED_MAX(249) = 249 + 249/254 + 2 = 252 */
#define PKT_RX_BUF_SIZE      300U

/* ---- TX state ------------------------------------------------------------ */
typedef enum {
    TX_IDLE = 0,
    TX_WAIT_ACK,
} tx_state_t;

/* ---- RX reassembly slot -------------------------------------------------- */
typedef struct {
    uint8_t  *buf;           /* heap-allocated reassembly buffer */
    uint16_t  msg_len;       /* total expected message length */
    uint16_t  received;      /* bytes assembled so far */
    uint8_t   msg_id;        /* message this slot belongs to */
    uint8_t   next_frag;     /* next expected fragment index */
    uint8_t   active;        /* 1 if slot is in use */
} rx_slot_t;

#endif /* PACKETIZER_INTERNAL_H */
