/**
 * @file udp_transport.h
 * @brief Thin UDP transport layer for the packetizer demo.
 *
 * Two processes share a pair of UDP sockets:
 *
 *   Process A listens on local_port, sends to remote_port (localhost).
 *   Process B listens on remote_port, sends to local_port (localhost).
 *
 * When a channel simulator is used it sits in between:
 *
 *   A → channel_sim (proxy_port) → B
 *   B → channel_sim (proxy_port+1) → A
 *
 * All sockets are non-blocking; udp_recv() returns 0 bytes when nothing
 * is available.
 */

#ifndef UDP_TRANSPORT_H
#define UDP_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct udp_transport_s udp_transport_t;

/**
 * Open a UDP transport.
 *
 * @param local_port   Port this process listens on.
 * @param remote_host  Destination hostname or IP (e.g. "127.0.0.1").
 * @param remote_port  Destination port.
 * @return             Opaque handle, or NULL on error.
 */
udp_transport_t *udp_open(uint16_t local_port,
                           const char *remote_host,
                           uint16_t remote_port);

/**
 * Close and free the transport.
 */
void udp_close(udp_transport_t *t);

/**
 * Send bytes (non-blocking).
 * @return 0 on success, -1 on error.
 */
int udp_send(udp_transport_t *t, const uint8_t *data, size_t len);

/**
 * Receive bytes (non-blocking).
 * @return Number of bytes received (0 if nothing available), -1 on error.
 */
ssize_t udp_recv(udp_transport_t *t, uint8_t *buf, size_t buf_len);

/**
 * File descriptor for use with select()/poll().
 */
int udp_fd(const udp_transport_t *t);

#ifdef __cplusplus
}
#endif

#endif /* UDP_TRANSPORT_H */
