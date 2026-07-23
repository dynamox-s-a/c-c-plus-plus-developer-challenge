/* Packetizer integration unit tests — exercises the full stack in-process */
#include "test_framework.h"
#include "../lib/packetizer/include/packetizer.h"

#include <string.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */
/* Test harness                                                                */
/* -------------------------------------------------------------------------- */

/** A loopback channel between two packetizer instances. */
typedef struct {
    packetizer_t *a;          /* sender side */
    packetizer_t *b;          /* receiver side */

    /* Last message received by B */
    uint8_t  b_msg[PKT_MAX_MSG_SIZE];
    size_t   b_msg_len;
    int      b_recv_count;

    /* Delivery status received by A */
    uint8_t  a_last_status_id;
    int      a_status_ok;
    int      a_status_count;

    /* Injection controls */
    int      drop_next;     /* drop the next write() call */
    int      drop_all;      /* drop ALL A→B writes (simulates total link loss) */
    int      corrupt_next;  /* corrupt the next write() call */
    int      dup_next;      /* duplicate the next write() call */

    /* Time for tick() */
    uint32_t now_ms;
} harness_t;

static harness_t *g_h __attribute__((unused)); /* global for callbacks */

static int write_a_to_b(const uint8_t *data, size_t len, void *ctx)
{
    harness_t *h = (harness_t *)ctx;
    if (h->drop_all)         { return 0; }
    if (h->drop_next)        { h->drop_next = 0; return 0; }
    if (h->corrupt_next) {
        h->corrupt_next = 0;
        uint8_t *copy = malloc(len);
        memcpy(copy, data, len);
        copy[len / 2] ^= 0xFF;  /* corrupt middle byte */
        pkt_feed(h->b, copy, len);
        free(copy);
        return 0;
    }
    if (h->dup_next) {
        h->dup_next = 0;
        pkt_feed(h->b, data, len);  /* first copy */
    }
    return pkt_feed(h->b, data, len);
}

static int write_b_to_a(const uint8_t *data, size_t len, void *ctx)
{
    harness_t *h = (harness_t *)ctx;
    return pkt_feed(h->a, data, len);
}

static void on_msg_b(const uint8_t *data, size_t len, void *ctx)
{
    harness_t *h = (harness_t *)ctx;
    memcpy(h->b_msg, data, len);
    h->b_msg_len = len;
    h->b_recv_count++;
}

static void on_status_a(uint8_t msg_id, pkt_status_t status, void *ctx)
{
    harness_t *h = (harness_t *)ctx;
    h->a_last_status_id = msg_id;
    h->a_status_ok      = (status == PKT_STATUS_OK);
    h->a_status_count++;
}

static void on_msg_noop(const uint8_t *data, size_t len, void *ctx)
{
    (void)data; (void)len; (void)ctx;
}

static harness_t *harness_new(uint16_t max_payload,
                               uint32_t timeout_ms,
                               uint8_t  max_retries)
{
    harness_t *h = calloc(1, sizeof(*h));

    pkt_config_t cfg_a = {
        .write       = write_a_to_b,
        .on_message  = on_msg_noop,  /* A is sender in these tests, but API requires it */
        .on_status   = on_status_a,
        .user_ctx    = h,
        .max_payload = max_payload,
        .timeout_ms  = timeout_ms,
        .max_retries = max_retries,
    };
    pkt_config_t cfg_b = {
        .write       = write_b_to_a,
        .on_message  = on_msg_b,
        .on_status   = NULL,
        .user_ctx    = h,
        .max_payload = max_payload,
        .timeout_ms  = timeout_ms,
        .max_retries = max_retries,
    };

    h->a = pkt_create(&cfg_a);
    h->b = pkt_create(&cfg_b);
    return h;
}

static void harness_free(harness_t *h)
{
    pkt_destroy(h->a);
    pkt_destroy(h->b);
    free(h);
}

/** Advance simulated time and call tick() until packetizer is idle. */
static void harness_settle(harness_t *h, int max_ticks)
{
    for (int i = 0; i < max_ticks && pkt_is_busy(h->a); i++) {
        h->now_ms += 50;
        pkt_tick(h->a, h->now_ms);
        pkt_tick(h->b, h->now_ms);
    }
}

/* -------------------------------------------------------------------------- */
/* Tests                                                                       */
/* -------------------------------------------------------------------------- */

TEST(send_short_message)
{
    harness_t *h = harness_new(240, 500, 5);
    const char *msg = "Hello, Dynamox!";
    uint8_t id;
    int rc = pkt_send(h->a, (const uint8_t *)msg, strlen(msg), &id);
    ASSERT_EQ(rc, 0);

    harness_settle(h, 20);

    ASSERT_EQ(h->b_recv_count, 1);
    ASSERT_EQ(h->b_msg_len, strlen(msg));
    ASSERT_MEM_EQ(h->b_msg, msg, strlen(msg));
    ASSERT_EQ(h->a_status_ok, 1);

    harness_free(h);
}

TEST(send_binary_message_with_zeros)
{
    harness_t *h = harness_new(240, 500, 5);
    uint8_t msg[64];
    for (size_t i = 0; i < sizeof(msg); i++) msg[i] = (uint8_t)(i % 16);
    /* inject several zero bytes */
    msg[0] = 0; msg[10] = 0; msg[30] = 0; msg[63] = 0;

    uint8_t id;
    ASSERT_EQ(pkt_send(h->a, msg, sizeof(msg), &id), 0);
    harness_settle(h, 20);

    ASSERT_EQ(h->b_recv_count, 1);
    ASSERT_EQ(h->b_msg_len, sizeof(msg));
    ASSERT_MEM_EQ(h->b_msg, msg, sizeof(msg));
    harness_free(h);
}

TEST(fragmented_message)
{
    /* Use tiny max_payload (10) to force fragmentation */
    harness_t *h = harness_new(10, 500, 5);
    uint8_t msg[100];
    for (size_t i = 0; i < sizeof(msg); i++) msg[i] = (uint8_t)i;

    uint8_t id;
    ASSERT_EQ(pkt_send(h->a, msg, sizeof(msg), &id), 0);
    harness_settle(h, 200);

    ASSERT_EQ(h->b_recv_count, 1);
    ASSERT_EQ(h->b_msg_len, sizeof(msg));
    ASSERT_MEM_EQ(h->b_msg, msg, sizeof(msg));
    ASSERT_EQ(h->a_status_ok, 1);
    harness_free(h);
}

TEST(corrupt_packet_is_rejected_and_retransmitted)
{
    harness_t *h = harness_new(240, 100, 5);
    h->corrupt_next = 1;  /* corrupt the first DATA packet */

    const char *msg = "survive corruption";
    uint8_t id;
    ASSERT_EQ(pkt_send(h->a, (const uint8_t *)msg, strlen(msg), &id), 0);
    harness_settle(h, 100);

    /* Despite the corruption the message must arrive (retransmit kicked in) */
    ASSERT_EQ(h->b_recv_count, 1);
    ASSERT_MEM_EQ(h->b_msg, msg, strlen(msg));
    ASSERT_EQ(h->a_status_ok, 1);
    harness_free(h);
}

TEST(dropped_packet_is_retransmitted)
{
    harness_t *h = harness_new(240, 100, 5);
    h->drop_next = 1;  /* drop the first DATA packet */

    const char *msg = "survive loss";
    uint8_t id;
    ASSERT_EQ(pkt_send(h->a, (const uint8_t *)msg, strlen(msg), &id), 0);
    harness_settle(h, 100);

    ASSERT_EQ(h->b_recv_count, 1);
    ASSERT_EQ(h->a_status_ok, 1);
    harness_free(h);
}

TEST(duplicated_packet_delivered_once)
{
    harness_t *h = harness_new(240, 500, 5);
    h->dup_next = 1;  /* duplicate the first DATA packet */

    const char *msg = "no double delivery";
    uint8_t id;
    ASSERT_EQ(pkt_send(h->a, (const uint8_t *)msg, strlen(msg), &id), 0);
    harness_settle(h, 50);

    /* Message must be delivered exactly once */
    ASSERT_EQ(h->b_recv_count, 1);
    harness_free(h);
}

TEST(max_retries_exceeded_reports_error)
{
    /* max_retries=2, link is totally dead → should fail after 3 attempts */
    harness_t *h = harness_new(240, 50, 2);
    h->drop_all = 1;  /* A→B link is dead; B never ACKs */

    const char *msg = "this will fail";
    uint8_t id;
    ASSERT_EQ(pkt_send(h->a, (const uint8_t *)msg, strlen(msg), &id), 0);

    /* Advance time well past retries */
    for (int i = 0; i < 200; i++) {
        h->now_ms += 50;
        pkt_tick(h->a, h->now_ms);
    }

    ASSERT_EQ(h->a_status_count, 1);
    ASSERT_EQ(h->a_status_ok, 0);   /* must report failure */
    ASSERT_EQ(h->b_recv_count, 0);  /* B received nothing */
    harness_free(h);
}

TEST(busy_returns_error_when_inflight)
{
    harness_t *h = harness_new(240, 500, 5);
    h->drop_all = 1;  /* prevent synchronous ACK so packetizer stays busy */

    /* Start a send — B won't ACK so A remains in TX_WAIT_ACK */
    const char *msg = "first";
    uint8_t id1, id2;
    ASSERT_EQ(pkt_send(h->a, (const uint8_t *)msg, strlen(msg), &id1), 0);
    ASSERT(pkt_is_busy(h->a));

    /* A second send while first is in-flight must fail */
    int rc = pkt_send(h->a, (const uint8_t *)"second", 6, &id2);
    ASSERT_EQ(rc, -1);

    /* Allow delivery now */
    h->drop_all = 0;
    harness_settle(h, 20);
    ASSERT_EQ(h->a_status_ok, 1);
    harness_free(h);
}
