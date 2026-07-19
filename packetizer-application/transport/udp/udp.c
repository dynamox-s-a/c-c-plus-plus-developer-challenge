#include "udp.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>

/*
 * Single shared instance of the transport state, declared as extern in
 * transport_udp.h. All functions in this file operate on this instance,
 * so callers never need to allocate or pass a handle themselves.
 */
udp_handle_t g_udp_handle;

int udp_init(uint16_t local_port,
             const char *remote_ip,
             uint16_t remote_port)
{
    if (remote_ip == NULL) {
        errno = EINVAL;
        return -1;
    }

    memset(&g_udp_handle, 0, sizeof(g_udp_handle));

    /* 1) Create the UDP (datagram, connectionless) socket. */
    g_udp_handle.sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (g_udp_handle.sockfd < 0) {
        perror("udp_init: socket");
        return -1;
    }

    /*
     * Allows quickly reusing the port after restarting the process
     * (avoids "Address already in use" during repeated test runs).
     */
    int reuse = 1;
    if (setsockopt(g_udp_handle.sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &reuse, sizeof(reuse)) < 0) {
        perror("udp_init: setsockopt(SO_REUSEADDR)");
        /* not fatal: continue anyway */
    }

    /* 2) Bind the local port on any interface (INADDR_ANY). */
    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family      = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port        = htons(local_port);

    if (bind(g_udp_handle.sockfd, (struct sockaddr *)&local_addr,
             sizeof(local_addr)) < 0) {
        perror("udp_init: bind");
        close(g_udp_handle.sockfd);
        g_udp_handle.sockfd = -1;
        return -1;
    }

    /* 3) Resolve/store the remote address (send destination). */
    memset(&g_udp_handle.remote_addr, 0, sizeof(g_udp_handle.remote_addr));
    g_udp_handle.remote_addr.sin_family = AF_INET;
    g_udp_handle.remote_addr.sin_port   = htons(remote_port);

    if (inet_pton(AF_INET, remote_ip, &g_udp_handle.remote_addr.sin_addr) != 1) {
        fprintf(stderr, "udp_init: invalid remote IP: %s\n", remote_ip);
        close(g_udp_handle.sockfd);
        g_udp_handle.sockfd = -1;
        return -1;
    }

    g_udp_handle.is_open = 1;
    return 0;
}

ssize_t udp_send(const void *data, size_t len)
{
    if (!g_udp_handle.is_open || data == NULL) {
        errno = EINVAL;
        return -1;
    }

    ssize_t sent = sendto(g_udp_handle.sockfd, data, len, 0,
                           (struct sockaddr *)&g_udp_handle.remote_addr,
                           sizeof(g_udp_handle.remote_addr));
    if (sent < 0) {
        perror("udp_send: sendto");
        return -1;
    }

    return sent;
}

ssize_t udp_recv(void *buf,
                  size_t buf_len,
                  struct sockaddr_in *from_addr,
                  int timeout_ms)
{
    if (!g_udp_handle.is_open || buf == NULL) {
        errno = EINVAL;
        return -1;
    }

    /*
     * select() with a timeout allows the calling thread to "wake up"
     * periodically even if no data arrives, so it can check a stop
     * flag (essential for a clean shutdown of threads blocked on
     * recvfrom).
     */
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(g_udp_handle.sockfd, &readfds);

    struct timeval tv;
    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ready = select(g_udp_handle.sockfd + 1, &readfds, NULL, NULL, &tv);
    if (ready < 0) {
        if (errno == EINTR) {
            /* Interrupted by a signal (e.g. SIGINT): treat as a
             * timeout and let the caller decide what to do next. */
            return 0;
        }
        perror("udp_recv: select");
        return -1;
    }
    if (ready == 0) {
        return 0; /* timeout, no data */
    }

    struct sockaddr_in src_addr;
    socklen_t src_len = sizeof(src_addr);

    ssize_t n = recvfrom(g_udp_handle.sockfd, buf, buf_len, 0,
                          (struct sockaddr *)&src_addr, &src_len);
    if (n < 0) {
        perror("udp_recv: recvfrom");
        return -1;
    }

    if (from_addr != NULL) {
        *from_addr = src_addr;
    }

    return n;
}

void udp_close(void)
{
    if (g_udp_handle.is_open && g_udp_handle.sockfd >= 0) {
        close(g_udp_handle.sockfd);
    }
    g_udp_handle.sockfd  = -1;
    g_udp_handle.is_open = 0;
}