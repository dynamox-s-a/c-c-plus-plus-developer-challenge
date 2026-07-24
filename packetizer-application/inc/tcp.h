#ifndef __TRANSPORT_TCP_H
#define __TRANSPORT_TCP_H

/*
 * transport_tcp.h
 * ---------------
 * tcp transport layer for device-to-device communication.
 *
 * Used as an alternative transport layer, selected during compile with flag TRANSPORT_BACKEND=TCP.
 * Validates bonus #3:Support a second, different transport using the same packetizer core, demonstrating that the abstraction holds.
 */

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <netinet/in.h>

/// @brief Initializes the tcp transport:
///   1) creates the socket (AF_INET, SOCK_DGRAM)
///   2) binds the local port on INADDR_ANY (receive on any interface)
///   3) resolves/stores the remote address (send destination)
///
/// @param local_port  : local port to bind (receiving). Use 0 to let the OS
///               pick an ephemeral port (less common in this scenario,
///               since the remote peer needs to know the port).
/// @param remote_ip   : remote peer IP in dotted-decimal notation
///               (e.g. "127.0.0.1").
/// @param remote_port : remote peer port (send destination).
///
/// @returns 0 on success, -1 on error (message printed to stderr via
/// perror).

int tcp_init(uint16_t local_port,
             const char *remote_ip,
             uint16_t remote_port);

///
/// @brief Sends `len` bytes from `data` to the remote address configured in
/// tcp_init(). 
/// @param data pointer to the data buffer. Treated as uint8_t internally.
///
/// @param len Length of the data, in bytes.
/// @returns the number of bytes sent, or -1 on error.
///
ssize_t tcp_send(const void *data, size_t len);

///
/// @brief Waits for an incoming datagram for up to `timeout_ms` milliseconds
/// (implemented with select() over the socket).
///
/// If no data arrives within the timeout, returns 0 (not an error) -
/// this allows a receiving thread to periodically check a "stop" flag
/// without blocking forever on a recvfrom() call.
///
/// @param buf         : destination buffer
/// @param buf_len     : buffer size in bytes
/// @param from_addr   : if non-NULL, receives the source address of the datagram
/// @param timeout_ms  : maximum wait time in ms (0 = non-blocking poll)
///
/// @returns:
///   > 0 : bytes received
///     0 : timeout, no data available
///    -1 : error (message printed to stderr via perror)
///
ssize_t tcp_recv(void *buf,
                  size_t buf_len,
                  struct sockaddr_in *from_addr,
                  int timeout_ms);

///
/// @brief Closes the socket and marks the transport as closed.
/// Safe to call even if init failed partway through.
///
void tcp_close(void);


#endif /*__TRANSPORT_TCP_H*/