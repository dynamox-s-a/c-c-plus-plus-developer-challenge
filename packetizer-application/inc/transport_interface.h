#ifndef __TRANSPORT_INTERFACE_H
#define __TRANSPORT_INTERFACE_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <netinet/in.h>

#if defined(TRANSPORT_USE_TCP) && TRANSPORT_USE_TCP
#include "tcp.h"
#else
#include "udp.h"
#endif

static inline int transport_init(uint16_t local_port,
                                 const char *remote_ip,
                                 uint16_t remote_port)
{
#if defined(TRANSPORT_USE_TCP) && TRANSPORT_USE_TCP
    return tcp_init(local_port, remote_ip, remote_port);
#else
    return udp_init(local_port, remote_ip, remote_port);
#endif
}

static inline ssize_t transport_send(const void *data, size_t len)
{
#if defined(TRANSPORT_USE_TCP) && TRANSPORT_USE_TCP
    return tcp_send(data, len);
#else
    return udp_send(data, len);
#endif
}

static inline ssize_t transport_recv(void *buf,
                                      size_t buf_len,
                                      struct sockaddr_in *from_addr,
                                      int timeout_ms)
{
#if defined(TRANSPORT_USE_TCP) && TRANSPORT_USE_TCP
    return tcp_recv(buf, buf_len, from_addr, timeout_ms);
#else
    return udp_recv(buf, buf_len, from_addr, timeout_ms);
#endif
}

static inline void transport_close(void)
{
#if defined(TRANSPORT_USE_TCP) && TRANSPORT_USE_TCP
    tcp_close();
#else
    udp_close();
#endif
}

#endif /* __TRANSPORT_INTERFACE_H */