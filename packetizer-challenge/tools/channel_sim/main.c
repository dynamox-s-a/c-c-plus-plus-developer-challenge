/**
 * channel_sim — UDP channel simulator for hostile-channel testing.
 *
 * Acts as a transparent UDP proxy between two peers while randomly injecting:
 *   - Packet loss
 *   - Packet corruption (bit flips)
 *   - Packet duplication
 *
 * Usage:
 *   channel_sim [options]
 *
 *   --a-port  <N>     Port that peer A sends to (sim listens here)
 *   --b-port  <N>     Port that peer B sends to (sim listens here)
 *   --a-dest  <N>     Port to forward A→B traffic to (peer B's listen port)
 *   --b-dest  <N>     Port to forward B→A traffic to (peer A's listen port)
 *   --loss    <0-100> Packet loss probability % (default 10)
 *   --corrupt <0-100> Packet corruption probability % (default 5)
 *   --dup     <0-100> Packet duplication probability % (default 5)
 *   --seed    <N>     Random seed (default: time-based)
 *
 * Example (peer A on 5000↔5001, channel_sim on 5002↔5003):
 *
 *   channel_sim --a-port 5002 --b-port 5003 \
 *               --a-dest 5001 --b-dest 5000  \
 *               --loss 15 --corrupt 5 --dup 5
 *
 *   peer --local-port 5000 --remote-port 5002   # A → sim
 *   peer --local-port 5001 --remote-port 5003   # B → sim
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

/* -------------------------------------------------------------------------- */
/* Helpers                                                                     */
/* -------------------------------------------------------------------------- */

static int open_udp(uint16_t port)
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) { perror("socket"); return -1; }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return -1;
    }
    return fd;
}

static int send_to(int fd, const uint8_t *buf, size_t len, uint16_t port)
{
    struct sockaddr_in dst;
    memset(&dst, 0, sizeof(dst));
    dst.sin_family      = AF_INET;
    dst.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dst.sin_port        = htons(port);
    return (int)sendto(fd, buf, len, 0, (struct sockaddr *)&dst, sizeof(dst));
}

/** Returns 1 with probability p/100. */
static int chance(int p) { return (rand() % 100) < p; }

/** Flip a random bit in buf[0..len-1]. */
static void corrupt(uint8_t *buf, size_t len)
{
    if (len == 0) return;
    size_t byte_idx = (size_t)rand() % len;
    uint8_t bit     = (uint8_t)(1u << (rand() % 8));
    buf[byte_idx] ^= bit;
}

/* -------------------------------------------------------------------------- */
/* main                                                                        */
/* -------------------------------------------------------------------------- */

typedef struct {
    uint16_t a_port, b_port;   /* ports sim listens on */
    uint16_t a_dest, b_dest;   /* ports sim forwards to */
    int loss_pct, corrupt_pct, dup_pct;
} config_t;

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s --a-port <N> --b-port <N> --a-dest <N> --b-dest <N>\n"
        "          [--loss <0-100>] [--corrupt <0-100>] [--dup <0-100>]\n"
        "          [--seed <N>]\n",
        prog);
}

int main(int argc, char *argv[])
{
    config_t cfg = {
        .a_port     = 0,
        .b_port     = 0,
        .a_dest     = 0,
        .b_dest     = 0,
        .loss_pct   = 10,
        .corrupt_pct = 5,
        .dup_pct    = 5,
    };
    unsigned seed = (unsigned)time(NULL);

    for (int i = 1; i < argc; i++) {
#define ARG(name) (strcmp(argv[i], name) == 0 && i + 1 < argc)
        if      (ARG("--a-port"))   cfg.a_port      = (uint16_t)atoi(argv[++i]);
        else if (ARG("--b-port"))   cfg.b_port      = (uint16_t)atoi(argv[++i]);
        else if (ARG("--a-dest"))   cfg.a_dest      = (uint16_t)atoi(argv[++i]);
        else if (ARG("--b-dest"))   cfg.b_dest      = (uint16_t)atoi(argv[++i]);
        else if (ARG("--loss"))     cfg.loss_pct    = atoi(argv[++i]);
        else if (ARG("--corrupt"))  cfg.corrupt_pct = atoi(argv[++i]);
        else if (ARG("--dup"))      cfg.dup_pct     = atoi(argv[++i]);
        else if (ARG("--seed"))     seed             = (unsigned)atoi(argv[++i]);
        else { usage(argv[0]); return 1; }
#undef ARG
    }

    if (!cfg.a_port || !cfg.b_port || !cfg.a_dest || !cfg.b_dest) {
        usage(argv[0]);
        return 1;
    }

    srand(seed);

    int fd_a = open_udp(cfg.a_port);  /* receives from peer A */
    int fd_b = open_udp(cfg.b_port);  /* receives from peer B */
    if (fd_a < 0 || fd_b < 0) return 1;

    printf("Channel simulator started (seed=%u)\n", seed);
    printf("  A→B: :%u → :%u   B→A: :%u → :%u\n",
           cfg.a_port, cfg.a_dest, cfg.b_port, cfg.b_dest);
    printf("  Loss=%d%%  Corrupt=%d%%  Dup=%d%%\n",
           cfg.loss_pct, cfg.corrupt_pct, cfg.dup_pct);

    unsigned long total = 0, lost = 0, corrupted = 0, duped = 0;

    uint8_t buf[4096];
    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd_a, &fds);
        FD_SET(fd_b, &fds);
        int maxfd = (fd_a > fd_b ? fd_a : fd_b) + 1;

        struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };
        int r = select(maxfd, &fds, NULL, NULL, &tv);
        if (r < 0 && errno != EINTR) { perror("select"); break; }

        /* --- A → B direction --- */
        if (FD_ISSET(fd_a, &fds)) {
            ssize_t n = recv(fd_a, buf, sizeof(buf), 0);
            if (n > 0) {
                total++;
                if (chance(cfg.loss_pct)) {
                    lost++;
                    printf("[A→B] DROP  pkt #%lu\n", total);
                } else {
                    if (chance(cfg.corrupt_pct)) {
                        corrupt(buf, (size_t)n);
                        corrupted++;
                        printf("[A→B] CORRUPT pkt #%lu\n", total);
                    }
                    send_to(fd_a, buf, (size_t)n, cfg.a_dest);
                    if (chance(cfg.dup_pct)) {
                        send_to(fd_a, buf, (size_t)n, cfg.a_dest);
                        duped++;
                        printf("[A→B] DUP  pkt #%lu\n", total);
                    }
                }
            }
        }

        /* --- B → A direction --- */
        if (FD_ISSET(fd_b, &fds)) {
            ssize_t n = recv(fd_b, buf, sizeof(buf), 0);
            if (n > 0) {
                total++;
                if (chance(cfg.loss_pct)) {
                    lost++;
                    printf("[B→A] DROP  pkt #%lu\n", total);
                } else {
                    if (chance(cfg.corrupt_pct)) {
                        corrupt(buf, (size_t)n);
                        corrupted++;
                        printf("[B→A] CORRUPT pkt #%lu\n", total);
                    }
                    send_to(fd_b, buf, (size_t)n, cfg.b_dest);
                    if (chance(cfg.dup_pct)) {
                        send_to(fd_b, buf, (size_t)n, cfg.b_dest);
                        duped++;
                        printf("[B→A] DUP  pkt #%lu\n", total);
                    }
                }
            }
        }

        fflush(stdout);
    }

    printf("\nStats: total=%lu lost=%lu corrupt=%lu dup=%lu\n",
           total, lost, corrupted, duped);
    close(fd_a);
    close(fd_b);
    return 0;
}
