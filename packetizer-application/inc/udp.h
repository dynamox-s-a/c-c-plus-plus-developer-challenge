#ifndef TRANSPORT_UDP_H
#define TRANSPORT_UDP_H

/*
 * transport_udp.h
 * ---------------
 * UDP transport layer for device-to-device communication.
 *
 * DESIGN NOTE: this module is intentionally simple and "dumb". It only
 * knows how to send/receive raw datagrams over a UDP socket. All the
 * protocol intelligence (packet framing, checksums, retransmission,
 * delivery control, message reassembly, etc.) belongs to the packetizer
 * layer, which must be transport-agnostic and see this module only
 * through an abstract "send bytes" / "receive bytes" interface. This
 * allows swapping UDP for another transport (TCP, serial, etc.) without
 * touching the packetizer core.
 *
 * API STYLE: the module keeps a single shared handle internally
 * (declared here, defined in transport_udp.c) so that callers -
 * including main() - never need to create, own, or pass around a
 * transport struct. Callers only invoke the abstraction functions below:
 * udp_init(), udp_send(), udp_recv() and udp_close().
 */

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <netinet/in.h>

/*
 * Internal state of the UDP transport.
 *
 * UDP has no notion of a "connection", so this struct just holds:
 *  - the local socket (used both for sending and receiving)
 *  - the remote peer address, used as the default destination of
 *    udp_send()
 *
 * Declared in the header and defined (as a single instance) in the
 * source file, so both sides share the same handle. Not intended to be
 * instantiated by callers - treat it as an implementation detail that
 * only exists to be shared between this header and transport_udp.c.
 */
typedef struct {
    int sockfd;                     /* UDP socket file descriptor        */
    struct sockaddr_in remote_addr; /* default destination for udp_send  */
    int is_open;                    /* 0 = closed, 1 = open              */
} udp_handle_t;

/* Single shared instance of the transport state. */
extern udp_handle_t g_udp_handle;

/*
 * Initializes the UDP transport:
 *   1) creates the socket (AF_INET, SOCK_DGRAM)
 *   2) binds the local port on INADDR_ANY (receive on any interface)
 *   3) resolves/stores the remote address (send destination)
 *
 * local_port  : local port to bind (receiving). Use 0 to let the OS
 *               pick an ephemeral port (less common in this scenario,
 *               since the remote peer needs to know the port).
 * remote_ip   : remote peer IP in dotted-decimal notation
 *               (e.g. "127.0.0.1").
 * remote_port : remote peer port (send destination).
 *
 * Returns 0 on success, -1 on error (message printed to stderr via
 * perror).
 */
int udp_init(uint16_t local_port,
             const char *remote_ip,
             uint16_t remote_port);

/*
 * Sends `len` bytes from `data` to the remote address configured in
 * udp_init(). UDP does not guarantee delivery, ordering, or absence of
 * duplication: these guarantees are the responsibility of the
 * packetizer layer built on top of this transport.
 *
 * Returns the number of bytes sent, or -1 on error.
 */
ssize_t udp_send(const void *data, size_t len);

/*
 * Waits for an incoming datagram for up to `timeout_ms` milliseconds
 * (implemented with select() over the socket).
 *
 * If no data arrives within the timeout, returns 0 (not an error) -
 * this allows a receiving thread to periodically check a "stop" flag
 * without blocking forever on a recvfrom() call.
 *
 * buf         : destination buffer
 * buf_len     : buffer size in bytes
 * from_addr   : if non-NULL, receives the source address of the datagram
 * timeout_ms  : maximum wait time in ms (0 = non-blocking poll)
 *
 * Returns:
 *   > 0 : bytes received
 *     0 : timeout, no data available
 *    -1 : error (message printed to stderr via perror)
 */
ssize_t udp_recv(void *buf,
                  size_t buf_len,
                  struct sockaddr_in *from_addr,
                  int timeout_ms);

/*
 * Closes the socket and marks the transport as closed.
 * Safe to call even if init failed partway through.
 */
void udp_close(void);

#endif /* TRANSPORT_UDP_H */