/*
 * main.c
 * ------
 * Demo "peer" application for the UDP transport layer.
 *
 * The same application acts as sender and receiver at the same time
 * (full-duplex): a dedicated thread receives datagrams and prints
 * them, while the main thread reads lines from stdin and sends them
 * to the remote peer. To try it out, run two instances (or two
 * terminal windows) with crossed ports, e.g.:
 *
 *   Terminal A: ./peer 5000 127.0.0.1 6000
 *   Terminal B: ./peer 6000 127.0.0.1 5000
 *
 * Type a line in either terminal and press Enter to send it.
 * Ctrl+C shuts the application down cleanly.
 *
 * main() only orchestrates the UDP transport abstraction
 * (udp_init / udp_send / udp_recv / udp_close) plus the demo's own
 * threading and signal handling; it never touches sockets or
 * transport internals directly.
 *
 * NOTE: this application does not use the packetizer yet (out of
 * scope for this file). It exchanges raw UDP datagrams, serving as
 * the communication base the packetizer will be built on top of.
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>

#include "udp.h"
#include "log.h"
#include "packetizer.h"

#define RECV_BUF_SIZE   2048
#define RECV_TIMEOUT_MS 200   /* wake-up interval to check the stop flag */

/* Shared stop flag between the main thread and the receiver thread.
 * volatile sig_atomic_t is the appropriate type for variables touched
 * by a signal handler. */
static volatile sig_atomic_t g_stop = 0;

static void on_sigint(int signum)
{
    (void)signum;
    g_stop = 1;
}

static void *receiver_thread_fn(void *arg)
{
    (void)arg;

    uint8_t buf[RECV_BUF_SIZE];
    struct sockaddr_in from_addr;

    while (!g_stop) {
        ssize_t n = udp_recv(buf, sizeof(buf) - 1, &from_addr, RECV_TIMEOUT_MS);
        
        
        if (n < 0) {
            /* Real socket error: not much to do besides reporting it*/
            continue;
        }
        if (n == 0) {
            continue; /* timeout: just re-check g_stop */
        }
        packetizer_receive_data(buf, n);

        fflush(stdout);
    }

    return NULL;
}

int transport_send_fn(void * data, uint16_t data_len)
{
    if(udp_send(data, (uint16_t) data_len) <= 0) return 1;
    return 0;
}

void packetizer_rx_cb(packetizer_frame_t rx)
{

    char rx_data[PACKET_MTU] = {0};
    memcpy(rx_data, rx.payload, rx.payload_length);
    LOG_DEBUG("%u bytes rx: %s", rx.payload_length,(char*)rx_data);
}


int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr,
                "Usage: %s <local_port> <remote_ip> <remote_port>\n"
                "Example: %s 5000 127.0.0.1 6000\n",
                argv[0], argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t local_port   = (uint16_t)atoi(argv[1]);
    const char *remote_ip = argv[2];
    uint16_t remote_port  = (uint16_t)atoi(argv[3]);

    if (udp_init(local_port, remote_ip, remote_port) != 0) {
        fprintf(stderr, "Failed to initialize the UDP transport.\n");
        return EXIT_FAILURE;
    }

    packetizer_init(transport_send_fn, packetizer_rx_cb);

    /* Handle SIGINT (Ctrl+C) to allow a clean shutdown of the threads. */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sigaction(SIGINT, &sa, NULL);

    printf("Peer listening on port %u, sending to %s:%u\n",
           local_port, remote_ip, remote_port);
    printf("Type a message and press Enter to send it (Ctrl+C to quit).\n> ");
    fflush(stdout);

    /* Start the receiver thread: it runs in parallel with the send
     * loop below, giving true full-duplex operation. */
    pthread_t recv_tid;
    if (pthread_create(&recv_tid, NULL, receiver_thread_fn, NULL) != 0) {
        perror("pthread_create");
        udp_close();
        return EXIT_FAILURE;
    }

    /* Main thread: reads lines from stdin and sends each one as a UDP datagram.*/
    char line[RECV_BUF_SIZE];
    while (!g_stop) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break; /* EOF on stdin (e.g. closed pipe) */
        }

        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) {
            printf("\n\r> ");
            fflush(stdout);
            continue;
        }

        if (packetizer_send_data(line, len) < 0) {
            fprintf(stderr, "Failed to send message.\n");
        }
        printf("> ");
        fflush(stdout);
    }

    g_stop = 1;
    pthread_join(recv_tid, NULL);
    udp_close();

    printf("\nPeer shut down.\n");
    return EXIT_SUCCESS;
}