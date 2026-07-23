#include "udp_transport.h"

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct udp_transport_s {
    int                 fd;
    struct sockaddr_in  remote;
};

udp_transport_t *udp_open(uint16_t local_port,
                           const char *remote_host,
                           uint16_t remote_port)
{
    udp_transport_t *t = calloc(1, sizeof(*t));
    if (!t) return NULL;

    t->fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (t->fd < 0) {
        perror("socket");
        free(t);
        return NULL;
    }

    /* Allow address reuse so we can restart quickly */
    int opt = 1;
    setsockopt(t->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Non-blocking */
    int flags = fcntl(t->fd, F_GETFL, 0);
    fcntl(t->fd, F_SETFL, flags | O_NONBLOCK);

    /* Bind local port */
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family      = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port        = htons(local_port);

    if (bind(t->fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
        perror("bind");
        close(t->fd);
        free(t);
        return NULL;
    }

    /* Resolve remote address */
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%u", (unsigned)remote_port);
    if (getaddrinfo(remote_host, port_str, &hints, &res) != 0) {
        fprintf(stderr, "getaddrinfo: cannot resolve %s\n", remote_host);
        close(t->fd);
        free(t);
        return NULL;
    }
    memcpy(&t->remote, res->ai_addr, sizeof(t->remote));
    freeaddrinfo(res);

    return t;
}

void udp_close(udp_transport_t *t)
{
    if (!t) return;
    close(t->fd);
    free(t);
}

int udp_send(udp_transport_t *t, const uint8_t *data, size_t len)
{
    ssize_t sent = sendto(t->fd, data, len, 0,
                          (struct sockaddr *)&t->remote, sizeof(t->remote));
    if (sent < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("sendto");
        return -1;
    }
    return 0;
}

ssize_t udp_recv(udp_transport_t *t, uint8_t *buf, size_t buf_len)
{
    ssize_t n = recv(t->fd, buf, buf_len, 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) return 0;
        perror("recv");
        return -1;
    }
    return n;
}

int udp_fd(const udp_transport_t *t)
{
    return t ? t->fd : -1;
}
