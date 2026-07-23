/**
 * @file packetizer.h
 * @brief Transport-agnostic message packetizer — public API.
 *
 * The packetizer sits between the application and a byte-oriented transport
 * (UART, BLE, UDP, named pipe, …).  It provides:
 *
 *   - Transparent fragmentation / reassembly of arbitrarily-large messages
 *     into fixed-size packets.
 *   - Per-packet framing via COBS (no special bytes in the data stream).
 *   - Per-packet integrity via CRC-16/CCITT.
 *   - Reliable delivery via stop-and-wait ARQ (ACK / NAK + retransmit on
 *     timeout).
 *   - Delivery status callbacks so the application knows whether each message
 *     was received or failed.
 *
 * The library is **single-threaded** and **transport-agnostic**: it never
 * calls sleep()/select()/read()/write() directly.  The caller drives it
 * through three entry points:
 *
 *   pkt_send()  — hand an application message to the library.
 *   pkt_feed()  — feed received bytes from the transport.
 *   pkt_tick()  — advance the retransmit timer (call periodically).
 *
 * In a multithreaded application the caller must protect each instance with
 * its own mutex.
 */

#ifndef PACKETIZER_H
#define PACKETIZER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------- */
/* Constants                                                                   */
/* -------------------------------------------------------------------------- */

/** Maximum bytes of application payload per packet on the wire. */
#define PKT_MAX_PAYLOAD  240U

/**
 * Maximum reassembled message size (bytes).
 * Larger messages are rejected at the sender.
 */
#define PKT_MAX_MSG_SIZE (64U * 1024U)

/** Number of simultaneous incoming-message reassembly slots. */
#define PKT_RX_SLOTS 4U

/* -------------------------------------------------------------------------- */
/* Types                                                                       */
/* -------------------------------------------------------------------------- */

/** Delivery status reported via the on_status callback. */
typedef enum {
    PKT_STATUS_OK    = 0, /**< Message delivered successfully. */
    PKT_STATUS_ERROR = 1, /**< Max retries exceeded; message lost. */
} pkt_status_t;

/**
 * Called by the library when it needs to write bytes to the transport.
 *
 * @param data    Bytes to transmit.
 * @param len     Number of bytes.
 * @param ctx     User context pointer from pkt_config_t.
 * @return        0 on success, -1 on failure.
 */
typedef int (*pkt_write_fn)(const uint8_t *data, size_t len, void *ctx);

/**
 * Called when a complete, integrity-verified message has been reassembled.
 *
 * @param data    Message bytes (valid only during the callback).
 * @param len     Message length in bytes.
 * @param ctx     User context pointer from pkt_config_t.
 */
typedef void (*pkt_recv_fn)(const uint8_t *data, size_t len, void *ctx);

/**
 * Called with the delivery status of a message previously submitted via
 * pkt_send().
 *
 * @param msg_id  Opaque identifier returned by pkt_send().
 * @param status  PKT_STATUS_OK or PKT_STATUS_ERROR.
 * @param ctx     User context pointer from pkt_config_t.
 */
typedef void (*pkt_status_fn)(uint8_t msg_id, pkt_status_t status, void *ctx);

/** Configuration passed to pkt_create(). */
typedef struct {
    pkt_write_fn   write;        /**< Transport write callback (required). */
    pkt_recv_fn    on_message;   /**< Message received callback (required). */
    pkt_status_fn  on_status;    /**< Delivery status callback (optional). */
    void          *user_ctx;     /**< Passed verbatim to every callback. */
    uint16_t       max_payload;  /**< Max bytes per packet payload (≤ PKT_MAX_PAYLOAD). */
    uint32_t       timeout_ms;   /**< ACK wait timeout in milliseconds. */
    uint8_t        max_retries;  /**< Max retransmit attempts before giving up. */
} pkt_config_t;

/** Opaque packetizer instance. */
typedef struct packetizer_s packetizer_t;

/* -------------------------------------------------------------------------- */
/* API                                                                         */
/* -------------------------------------------------------------------------- */

/**
 * Allocate and initialise a packetizer instance.
 *
 * @param cfg   Configuration. All callbacks are copied.
 * @return      Heap-allocated instance, or NULL on error.
 */
packetizer_t *pkt_create(const pkt_config_t *cfg);

/**
 * Destroy a packetizer instance and free all resources.
 *
 * @param pkt   Instance returned by pkt_create(). May be NULL (no-op).
 */
void pkt_destroy(packetizer_t *pkt);

/**
 * Queue an application message for reliable transmission.
 *
 * The call blocks internally only to fragment and enqueue; the actual
 * transmission happens through the write callback.
 *
 * Only one message can be in-flight at a time (stop-and-wait).  If a message
 * is already pending, this function returns -1.
 *
 * @param pkt     Packetizer instance.
 * @param data    Message bytes.
 * @param len     Message length (must be ≤ PKT_MAX_MSG_SIZE).
 * @param msg_id  Output: opaque identifier for tracking via on_status.
 * @return        0 on success, -1 if another message is pending or on error.
 */
int pkt_send(packetizer_t *pkt, const uint8_t *data, size_t len,
             uint8_t *msg_id);

/**
 * Feed raw bytes received from the transport into the packetizer.
 *
 * The library scans for COBS frame terminators (0x00), decodes each complete
 * frame, validates its CRC, and dispatches ACKs / reassembles data.
 *
 * @param pkt   Packetizer instance.
 * @param data  Received bytes.
 * @param len   Number of bytes.
 * @return      0 on success, -1 on internal error.
 */
int pkt_feed(packetizer_t *pkt, const uint8_t *data, size_t len);

/**
 * Advance the retransmit timer.
 *
 * Must be called regularly (e.g. every 10 ms) so that the library can detect
 * ACK timeouts and retransmit or fail pending messages.
 *
 * @param pkt       Packetizer instance.
 * @param now_ms    Current time in milliseconds (monotonic, arbitrary epoch).
 */
void pkt_tick(packetizer_t *pkt, uint32_t now_ms);

/**
 * Return non-zero if a message is currently in-flight (waiting for ACK).
 */
int pkt_is_busy(const packetizer_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* PACKETIZER_H */
