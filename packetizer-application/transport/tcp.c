#include "tcp.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>


typedef struct {
    int sockfd;                     /* UDP socket file descriptor        */
    struct sockaddr_in remote_addr; /* default destination for udp_send  */
    int is_open;                    /* 0 = closed, 1 = open              */
} tcp_handle_t;

/* Single shared instance of the TCP transport state. */
tcp_handle_t g_tcp_handle;

static int tcp_wait_for_readable(int timeout_ms)
{
    fd_set readfds;
    struct timeval tv;

    FD_ZERO(&readfds);
    FD_SET(g_tcp_handle.sockfd, &readfds);

    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ready = select(g_tcp_handle.sockfd + 1, &readfds, NULL, NULL, &tv);
    if (ready < 0) {
        if (errno == EINTR) {
            return 0;
        }
        perror("tcp_wait_for_readable: select");
        return -1;
    }

    return ready;
}

static ssize_t tcp_recv_exact(void *buf, size_t len, int timeout_ms)
{
    uint8_t *cursor = (uint8_t *)buf;
    size_t total = 0;

    while (total < len) {
        int ready = tcp_wait_for_readable(timeout_ms);
        if (ready < 0) {
            return -1;
        }
        if (ready == 0) {
            return 0;
        }

        ssize_t n = recv(g_tcp_handle.sockfd, cursor + total, len - total, 0);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("tcp_recv_exact: recv");
            return -1;
        }
        if (n == 0) {
            return total;
        }

        total += (size_t)n;
    }

    return (ssize_t)total;
}

static ssize_t tcp_send_exact(const void *data, size_t len)
{
    const uint8_t *cursor = (const uint8_t *)data;
    size_t total = 0;

    while (total < len) {
        ssize_t n = send(g_tcp_handle.sockfd, cursor + total, len - total, 0);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("tcp_send_exact: send");
            return -1;
        }
        if (n == 0) {
            break;
        }

        total += (size_t)n;
    }

    return (ssize_t)total;
}

int tcp_init(uint16_t local_port,
             const char *remote_ip,
             uint16_t remote_port)
{
    if (remote_ip == NULL) {
        errno = EINVAL;
        return -1;
    }

    memset(&g_tcp_handle, 0, sizeof(g_tcp_handle));

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(local_port);

    memset(&g_tcp_handle.remote_addr, 0, sizeof(g_tcp_handle.remote_addr));
    g_tcp_handle.remote_addr.sin_family = AF_INET;
    g_tcp_handle.remote_addr.sin_port = htons(remote_port);

    if (inet_pton(AF_INET, remote_ip, &g_tcp_handle.remote_addr.sin_addr) != 1) {
        fprintf(stderr, "tcp_init: invalid remote IP: %s\n", remote_ip);
        return -1;
    }

    int reuse = 1;

    if (local_port < remote_port) {
        int listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd < 0) {
            perror("tcp_init: listen socket");
            return -1;
        }

        if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                       &reuse, sizeof(reuse)) < 0) {
            perror("tcp_init: setsockopt(SO_REUSEADDR)");
        }

        if (bind(listenfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
            perror("tcp_init: listen bind");
            close(listenfd);
            return -1;
        }

        if (listen(listenfd, SOMAXCONN) < 0) {
            perror("tcp_init: listen");
            close(listenfd);
            return -1;
        }

        fd_set readfds;
        struct timeval tv;
        FD_ZERO(&readfds);
        FD_SET(listenfd, &readfds);
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        int ready = select(listenfd + 1, &readfds, NULL, NULL, &tv);
        if (ready <= 0) {
            fprintf(stderr, "tcp_init: timed out waiting for peer connection\n");
            close(listenfd);
            errno = ETIMEDOUT;
            return -1;
        }

        int accepted_fd = accept(listenfd, NULL, NULL);
        if (accepted_fd < 0) {
            perror("tcp_init: accept");
            close(listenfd);
            return -1;
        }

        close(listenfd);
        g_tcp_handle.sockfd = accepted_fd;
    } else {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("tcp_init: socket");
            return -1;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                       &reuse, sizeof(reuse)) < 0) {
            perror("tcp_init: setsockopt(SO_REUSEADDR)");
        }

        if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
            perror("tcp_init: bind");
            close(sockfd);
            return -1;
        }

        int connect_attempts = 0;
        while (connect(sockfd,
                       (struct sockaddr *)&g_tcp_handle.remote_addr,
                       sizeof(g_tcp_handle.remote_addr)) < 0) {
            if (errno != EINTR && errno != ECONNREFUSED && errno != ETIMEDOUT && errno != EAGAIN) {
                perror("tcp_init: connect");
                close(sockfd);
                return -1;
            }

            if (++connect_attempts >= 50) {
                fprintf(stderr, "tcp_init: connect timed out\n");
                close(sockfd);
                errno = ETIMEDOUT;
                return -1;
            }

            usleep(100000);
        }

        g_tcp_handle.sockfd = sockfd;
    }

    g_tcp_handle.is_open = 1;
    return 0;
}

ssize_t tcp_send(const void *data, size_t len)
{
    if (!g_tcp_handle.is_open || data == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (len > UINT16_MAX) {
        errno = EMSGSIZE;
        return -1;
    }

    uint16_t wire_len = htons((uint16_t)len);
    if (tcp_send_exact(&wire_len, sizeof(wire_len)) < 0) {
        return -1;
    }

    ssize_t payload_sent = tcp_send_exact(data, len);
    if (payload_sent < 0) {
        return -1;
    }
    if ((size_t)payload_sent != len) {
        errno = EIO;
        return -1;
    }

    return (ssize_t)len;
}

ssize_t tcp_recv(void *buf,
                 size_t buf_len,
                 struct sockaddr_in *from_addr,
                 int timeout_ms)
{
    if (!g_tcp_handle.is_open || buf == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (buf_len == 0) {
        errno = EINVAL;
        return -1;
    }

    uint16_t wire_len = 0;
    ssize_t prefix_read = tcp_recv_exact(&wire_len, sizeof(wire_len), timeout_ms);
    if (prefix_read < 0) {
        return -1;
    }
    if (prefix_read == 0) {
        return 0;
    }
    if ((size_t)prefix_read != sizeof(wire_len)) {
        errno = EIO;
        return -1;
    }

    uint16_t payload_len = ntohs(wire_len);
    if (payload_len > buf_len) {
        errno = EMSGSIZE;
        return -1;
    }

    ssize_t payload_read = tcp_recv_exact(buf, payload_len, timeout_ms);
    if (payload_read < 0) {
        return -1;
    }
    if (payload_read == 0) {
        return 0;
    }
    if ((size_t)payload_read != payload_len) {
        errno = EIO;
        return -1;
    }

    if (from_addr != NULL) {
        *from_addr = g_tcp_handle.remote_addr;
    }

    return payload_read;
}

void tcp_close(void)
{
    if (g_tcp_handle.is_open && g_tcp_handle.sockfd >= 0) {
        close(g_tcp_handle.sockfd);
    }

    g_tcp_handle.sockfd = -1;
    g_tcp_handle.is_open = 0;
}
