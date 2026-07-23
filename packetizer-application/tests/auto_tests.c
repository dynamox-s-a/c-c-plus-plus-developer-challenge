/*
 * test_packetizer.c
 * ------------------
 * Automated test suite for the packetizer core.
 *
 * WHITE-BOX TECHNIQUE: this file includes packetizer.c directly
 * (rather than linking against a separately-compiled packetizer.o).
 * That is what lets the tests below reach static internals --
 * g_state, g_reassembly, g_pending_ack, packetizer_crc32(),
 * read_be16()/write_be32(), the OFFSET_* macros, etc. -- which are
 * required to:
 *   - force the sequence_number counter right up to its wraparound
 *     boundary without actually sending 65535 messages first;
 *   - deliberately corrupt a serialized frame's CRC/SOF/fields to
 *     check that packetizer_receive_data() rejects it;
 *   - synthesize ACK/NACK frames from a mock transport.
 *
 * BUILD NOTE: because this file #includes packetizer.c's source
 * directly, it must be built as ITS OWN executable and must NOT also
 * link against a separately-compiled packetizer.o/.a -- doing both
 * would define every packetizer_* symbol twice and fail to link. See
 * the CMakeLists.txt snippet provided alongside this file.
 *
 * This still needs the project's real "log.h" (whatever LOG_DEBUG/
 * LOG_ERROR resolve to in the main app) on the include path, exactly
 * like packetizer.c does when built for the real application.
 *
 * Test framework: no external dependency (no Unity/CMocka) --
 * a minimal CHECK()/RUN_TEST() pair. main() returns the number of
 * failed checks (0 = success), which is what CTest/CMake's add_test()
 * needs to treat this binary as a single pass/fail test.
 */

#include "../src/packetizer.c"

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Minimal test framework                                              */
/* ------------------------------------------------------------------ */

static int g_checks_run    = 0;
static int g_checks_failed = 0;

#define CHECK(cond) \
    do { \
        g_checks_run++; \
        if (!(cond)) { \
            g_checks_failed++; \
            printf("  FAIL (%s:%d): %s\n", __FILE__, __LINE__, #cond); \
        } \
    } while (0)

#define RUN_TEST(fn) \
    do { \
        printf("-- %s --\n", #fn); \
        fn(); \
    } while (0)

/* ------------------------------------------------------------------ */
/* Shared test doubles (mock transport + mock application callback)    */
/* ------------------------------------------------------------------ */

/* Records of what the mock application callback observed. */
static int      g_delivered_count = 0;
static uint16_t g_last_delivered_seq = 0;

static void test_on_message(packetizer_frame_t rx)
{
    g_delivered_count++;
    g_last_delivered_seq = rx.sequence_number;
}

/* Records of what the mock transport observed for the most recent
 * outgoing DATA frame, plus a running count of every DATA frame send
 * attempt (initial transmissions AND retransmissions alike). */
static int      g_data_send_count = 0;
static uint8_t  g_last_wire[PACKET_MTU];
static uint16_t g_last_wire_len = 0;

/* A transport double that "loses" everything: used for the ACK-timeout
 * test, where no response of any kind should ever come back. */
static int transport_black_hole(void * data, uint16_t len)
{
    (void)data; (void)len;
    return 0; /* "sent" successfully, but nobody ever answers */
}

/* A transport double that captures the last DATA frame sent (for
 * inspection) and swallows everything else. Does not answer with an
 * ACK/NACK itself -- individual tests that need a response spawn their
 * own short-lived thread to deliver one, simulating it arriving
 * asynchronously from a receiver thread, same as the real application. */
static int transport_capture_only(void * data, uint16_t len)
{
    uint8_t type = ((uint8_t *)data)[OFFSET_PACKET_TYPE];
    if (type == PACKET_TYPE_DATA)
    {
        g_data_send_count++;
        memcpy(g_last_wire, data, len);
        g_last_wire_len = len;
    }
    return 0;
}

/* Thread body: waits a few milliseconds (simulating network + peer
 * processing latency), then feeds an ACK for the last captured DATA
 * frame back into the packetizer, exactly as a receiver thread would
 * after calling udp_recv(). The delay matters: packetizer_send_data()
 * only starts actually waiting (packetizer_wait_for_ack) a moment
 * *after* the transport_send call returns, so an ACK delivered with
 * zero delay, from the very same call stack, would arrive before
 * anyone is listening for it and be silently ignored. */
static void * deliver_ack_after_delay(void * arg)
{
    (void)arg;
    struct timespec delay = { .tv_sec = 0, .tv_nsec = 5000000 }; /* 5ms */
    nanosleep(&delay, NULL);

    uint8_t copy[PACKET_MTU];
    memcpy(copy, g_last_wire, g_last_wire_len);
    packetizer_receive_data(copy, g_last_wire_len);
    return NULL;
}

/* Same idea, but patches the captured DATA frame into a NACK (keeping
 * its sequence_number/fragment_index/fragment_count, and recomputing
 * the CRC so it still passes validation) before delivering it. */
static void * deliver_nack_after_delay(void * arg)
{
    (void)arg;
    struct timespec delay = { .tv_sec = 0, .tv_nsec = 5000000 }; /* 5ms */
    nanosleep(&delay, NULL);

    uint8_t copy[PACKET_MTU];
    memcpy(copy, g_last_wire, g_last_wire_len);
    copy[OFFSET_PACKET_TYPE] = PACKET_TYPE_NACK;
    uint32_t crc = packetizer_crc32(&copy[OFFSET_PACKET_TYPE],
                                     (uint32_t)(g_last_wire_len - OFFSET_PACKET_TYPE - 4));
    write_be32(&copy[g_last_wire_len - 4], crc);

    packetizer_receive_data(copy, g_last_wire_len);
    return NULL;
}

/* A transport double that behaves like transport_capture_only for
 * DATA frames (spawning a delayed ACK responder each time), while also
 * feeding any ACK/NACK the core sends back into itself -- lets a
 * single-process test exercise the full send -> receive -> ACK send
 * -> ACK receive loop realistically. */
static int transport_auto_ack(void * data, uint16_t len)
{
    uint8_t type = ((uint8_t *)data)[OFFSET_PACKET_TYPE];

    if (type == PACKET_TYPE_DATA)
    {
        g_data_send_count++;
        memcpy(g_last_wire, data, len);
        g_last_wire_len = len;

        pthread_t tid;
        pthread_create(&tid, NULL, deliver_ack_after_delay, NULL);
        pthread_detach(tid);
        return 0;
    }

    if (type == PACKET_TYPE_ACK)
    {
        uint8_t copy[PACKET_MTU];
        memcpy(copy, data, len);
        return packetizer_receive_data(copy, len);
    }

    return 0;
}

/* Resets every piece of packetizer state between tests (re-running
 * packetizer_init conveniently zeroes g_state, g_reassembly and
 * g_pending_ack all at once) and resets the test doubles' own
 * bookkeeping. */
static void reset_packetizer(transport_send send_fn)
{
    packetizer_init(send_fn, test_on_message);
    g_delivered_count    = 0;
    g_last_delivered_seq = 0;
    g_data_send_count    = 0;
    g_last_wire_len      = 0;
}

/* ------------------------------------------------------------------ */
/* CRC validation                                                       */
/* ------------------------------------------------------------------ */

static void test_crc_valid_frame_is_accepted(void)
{
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    uint8_t payload[] = "crc ok";
    CHECK(packetizer_pack_data(payload, (uint32_t)(sizeof(payload) - 1), 0, 1, 7, &frame) == 0);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);

    /* Untouched, freshly-serialized bytes must be accepted as-is. */
    CHECK(packetizer_receive_data(wire, len) == 0);
    CHECK(g_delivered_count == 1);
}

static void test_crc_corrupted_payload_is_rejected(void)
{
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    uint8_t payload[] = "crc corrupt";
    packetizer_pack_data(payload, (uint32_t)(sizeof(payload) - 1), 0, 1, 8, &frame);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);
    wire[OFFSET_PAYLOAD] ^= 0xFF; /* flip a payload byte, CRC field left untouched */

    int rc = packetizer_receive_data(wire, len);
    CHECK(rc == EBADMSG);
    CHECK(g_delivered_count == 0); /* never handed to the application */
}

static void test_crc_corrupted_crc_field_is_rejected(void)
{
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    uint8_t payload[] = "crc field corrupt";
    packetizer_pack_data(payload, (uint32_t)(sizeof(payload) - 1), 0, 1, 9, &frame);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);
    wire[len - 1] ^= 0xFF; /* flip a byte inside the trailing CRC itself */

    int rc = packetizer_receive_data(wire, len);
    CHECK(rc == EBADMSG);
    CHECK(g_delivered_count == 0);
}

static void test_crc_zero_length_payload_frame_round_trips(void)
{
    /* Control frames (ACK/NACK) carry a zero-length payload; the CRC
     * math must still be correct for that edge case (offset math with
     * payload_length == 0). packetizer_send_control() is exercised
     * indirectly through the ACK path elsewhere, so here we build one
     * directly and confirm it validates. */
    packetizer_frame_t frame = {0};
    frame.sof             = PACKETIZER_SOF;
    frame.packet_type     = PACKET_TYPE_ACK;
    frame.sequence_number = 42;
    frame.fragment_index  = 0;
    frame.fragment_count  = 1;

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);
    CHECK(len == METADATA_SIZE);

    reset_packetizer(transport_capture_only);
    CHECK(packetizer_receive_data(wire, len) == 0);
}

/* ------------------------------------------------------------------ */
/* Invalid frame / parameter handling                                   */
/* ------------------------------------------------------------------ */

static void test_receive_rejects_null_or_undersized_input(void)
{
    reset_packetizer(transport_capture_only);

    CHECK(packetizer_receive_data(NULL, METADATA_SIZE) == EINVAL);

    uint8_t tiny[METADATA_SIZE - 1];
    memset(tiny, 0, sizeof(tiny));
    CHECK(packetizer_receive_data(tiny, sizeof(tiny)) == EINVAL);
}

static void test_receive_rejects_wrong_sof(void)
{
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    uint8_t payload[] = "sof test";
    packetizer_pack_data(payload, (uint32_t)(sizeof(payload) - 1), 0, 1, 2, &frame);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);
    wire[OFFSET_SOF] = 0x00; /* not PACKETIZER_SOF */

    CHECK(packetizer_receive_data(wire, len) == EBADMSG);
    CHECK(g_delivered_count == 0);
}

static void test_receive_rejects_inconsistent_payload_length(void)
{
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    uint8_t payload[] = "length test";
    packetizer_pack_data(payload, (uint32_t)(sizeof(payload) - 1), 0, 1, 3, &frame);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);

    /* Truncate the buffer by one byte without adjusting the
     * payload_length field baked into it: now inconsistent with the
     * data_len actually passed to packetizer_receive_data(). */
    CHECK(packetizer_receive_data(wire, (uint16_t)(len - 1)) == EBADMSG);
    CHECK(g_delivered_count == 0);
}

static void test_receive_rejects_unknown_packet_type(void)
{
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    frame.sof             = PACKETIZER_SOF;
    frame.packet_type     = 0xEE; /* not DATA, ACK, or NACK */
    frame.sequence_number = 1;
    frame.fragment_count  = 1;

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);

    CHECK(packetizer_receive_data(wire, len) == EBADMSG);
}

static void test_receive_rejects_fragment_joining_mid_stream(void)
{
    /* A fragment with index > 0 arriving for a sequence_number the
     * receiver has never seen fragment 0 of (e.g. that datagram was
     * lost) cannot be reassembled and must be rejected, not silently
     * accepted into a bogus partial message. */
    reset_packetizer(transport_capture_only);

    packetizer_frame_t frame = {0};
    frame.sof             = PACKETIZER_SOF;
    frame.packet_type     = PACKET_TYPE_DATA;
    frame.sequence_number = 99;
    frame.fragment_index  = 2; /* not 0 */
    frame.fragment_count  = 5;
    uint8_t payload[4] = { 1, 2, 3, 4 };
    frame.payload         = payload;
    frame.payload_length  = sizeof(payload);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);

    CHECK(packetizer_receive_data(wire, len) == EBADMSG);
    CHECK(g_delivered_count == 0);
}

static void test_send_rejects_invalid_arguments(void)
{
    reset_packetizer(transport_capture_only);

    CHECK(packetizer_send_data(NULL, 10) == EINVAL);

    char dummy = 0;
    CHECK(packetizer_send_data(&dummy, 0) == EINVAL);
}

static void test_send_rejects_message_too_large_to_fragment(void)
{
    reset_packetizer(transport_capture_only);

    /* The EMSGSIZE check happens purely on data_len, before any of the
     * message's actual bytes are read, so a real multi-gigabyte buffer
     * is not needed to exercise this path safely. */
    uint8_t dummy = 0;
    uint32_t oversized_len = (uint32_t)PAYLOAD_MAX_SIZE * (UINT16_MAX + 1u);
    CHECK(packetizer_send_data(&dummy, oversized_len) == EMSGSIZE);
}

static void test_init_rejects_null_send_fn(void)
{
    CHECK(packetizer_init(NULL, test_on_message) == EINVAL);
}

static void test_init_rejects_null_on_message(void)
{
    /* NOTE: packetizer.h's doxygen for packetizer_init() says
     * on_message "may be NULL if the receive path is not used", but
     * the current implementation rejects NULL unconditionally. This
     * test documents the actual (stricter) behavior; worth reconciling
     * the comment and the code one way or the other. */
    CHECK(packetizer_init(transport_capture_only, NULL) == EINVAL);
}

/* ------------------------------------------------------------------ */
/* ACK handling                                                        */
/* ------------------------------------------------------------------ */

static void test_ack_received_in_time_completes_send(void)
{
    reset_packetizer(transport_auto_ack);

    const char * msg = "ack ok";
    int rc = packetizer_send_data((void *)msg, (uint32_t)strlen(msg));

    CHECK(rc == 0);
    CHECK(g_data_send_count == 1); /* no retransmission needed */
}

static void test_ack_timeout_retransmits_then_fails(void)
{
    reset_packetizer(transport_black_hole);

    const char * msg = "nobody home";
    int rc = packetizer_send_data((void *)msg, (uint32_t)strlen(msg));

    CHECK(rc == ETIMEDOUT);
    /* One initial transmission plus PACKETIZER_ACK_MAX_RETRIES
     * retransmissions before giving up. transport_black_hole doesn't
     * track sends itself, so this is asserted indirectly via timing
     * instead: PACKETIZER_ACK_MAX_RETRIES full PACKETIZER_ACK_TIMEOUT_MS
     * windows must have elapsed. That timing assertion is inherently
     * a bit loose under CI load, so here we only assert the outcome
     * (ETIMEDOUT); test_ack_received_in_time_completes_send() and the
     * NACK tests below already cover send-count bookkeeping precisely
     * where the timing is deterministic (sub-10ms, not multi-second).
     */
}

/* ------------------------------------------------------------------ */
/* NACK handling                                                       */
/* ------------------------------------------------------------------ */

static int transport_always_nack(void * data, uint16_t len)
{
    uint8_t type = ((uint8_t *)data)[OFFSET_PACKET_TYPE];
    if (type == PACKET_TYPE_DATA)
    {
        g_data_send_count++;
        memcpy(g_last_wire, data, len);
        g_last_wire_len = len;

        pthread_t tid;
        pthread_create(&tid, NULL, deliver_nack_after_delay, NULL);
        pthread_detach(tid);
    }
    return 0;
}

static void test_nack_exhausting_retries_aborts_send(void)
{
    reset_packetizer(transport_always_nack);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);
    const char * msg = "rejected";
    int rc = packetizer_send_data((void *)msg, (uint32_t)strlen(msg));
    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed_ms = (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) / 1e6;

    CHECK(rc == EBADMSG);
    /* One send per NACK actually received, up to the retry limit --
     * packetizer_wait_for_ack() resets g_pending_ack.acked back to 0
     * after handling each NACK, so every retry gets its own genuine
     * wait for a fresh response instead of the stale flag immediately
     * re-triggering on the next poll tick. */
    CHECK(g_data_send_count == (int)PACKETIZER_NACK_MAX_RETRIES);
    /* Each NACK in this test is delivered by its own thread after a
     * real ~5ms delay (deliver_nack_after_delay), so exhausting
     * PACKETIZER_NACK_MAX_RETRIES of them genuinely takes multiple
     * such round trips -- confirms the retries are no longer an
     * instant, same-flag cascade (which previously finished in ~1ms
     * per bogus "retry" regardless of the delay). */
    CHECK(elapsed_ms >= 4.0 * ((double)PACKETIZER_NACK_MAX_RETRIES - 1.0));
}

/* ------------------------------------------------------------------ */
/* sequence_number wraparound (2^16 -> 0)                              */
/* ------------------------------------------------------------------ */

static void test_sequence_number_wraps_from_65535_to_1(void)
{
    reset_packetizer(transport_auto_ack);

    /* White-box: jump the internal counter right to the boundary
     * instead of actually sending 65535 messages first. */
    g_state.next_sequence_number = 65535;

    int rc1 = packetizer_send_data("a", 1);
    uint16_t seq1 = read_be16(&g_last_wire[OFFSET_SEQUENCE_NUMBER]);

    int rc2 = packetizer_send_data("b", 1);
    uint16_t seq2 = read_be16(&g_last_wire[OFFSET_SEQUENCE_NUMBER]);

    CHECK(rc1 == 0);
    CHECK(rc2 == 0);
    CHECK(seq1 == 65535);
    /* sequence_number 0 is reserved as the reassembly tracker's "no
     * message seen yet" sentinel (see g_reassembly.sequence_number's
     * initial value), so packetizer_send_data() is expected to skip
     * it on wraparound and resume at 1, not 0. */
    CHECK(seq2 == 1);
}

static void test_messages_still_delivered_across_wraparound(void)
{
    reset_packetizer(transport_auto_ack);
    g_state.next_sequence_number = 65535;

    packetizer_send_data("a", 1); /* seq 65535 */
    packetizer_send_data("b", 1); /* seq 1 (wrapped) */

    CHECK(g_delivered_count == 2);
    CHECK(g_last_delivered_seq == 1);

    /* packetizer_reassemble_fragment()'s jump-detection now computes
     * the expected next sequence_number with wraparound in mind
     * (65535's successor is 1, not 0), instead of evaluating
     * g_reassembly.sequence_number + 1 directly in uint16_t -- which
     * used to overflow to 0 right at this same boundary and made this
     * perfectly in-order message look like a lost-message jump. The
     * tracker itself should reflect the wrapped value cleanly. */
    CHECK(g_reassembly.sequence_number == 1);
}

/* ------------------------------------------------------------------ */
/* Full-duplex: sending our own message and receiving the peer's       */
/* incoming message concurrently                                       */
/* ------------------------------------------------------------------ */

/* Mirrors the real application's architecture (peer.c): one thread
 * calls packetizer_send_data() and blocks in its stop-and-wait ACK
 * loop, while a second thread concurrently calls
 * packetizer_receive_data() for a message arriving from the peer,
 * exactly as a dedicated UDP receiver thread would. This test's goal
 * is to confirm those two roles don't block each other -- not to
 * prove general thread-safety under arbitrary concurrent access (the
 * core's own comments already note it assumes a single outstanding
 * send and a single in-progress reassembly at a time; two receive
 * threads racing each other, for instance, is out of scope here). */

static int g_duplex_ack_sends = 0; /* ACK frames this transport actually transmitted */

/* Forwards ACK frames straight into packetizer_receive_data(), as if
 * they had just arrived over the wire; simply records any DATA frame
 * we send ourselves (its own ACK is delivered explicitly by the test
 * on a delayed thread below, not synchronously here, so it genuinely
 * arrives mid-wait rather than before anyone is listening for it). */
static int transport_full_duplex(void * data, uint16_t len)
{
    uint8_t type = ((uint8_t *)data)[OFFSET_PACKET_TYPE];

    if (type == PACKET_TYPE_DATA)
    {
        g_data_send_count++;
        memcpy(g_last_wire, data, len);
        g_last_wire_len = len;
        return 0;
    }

    if (type == PACKET_TYPE_ACK)
    {
        g_duplex_ack_sends++;
        uint8_t copy[PACKET_MTU];
        memcpy(copy, data, len);
        return packetizer_receive_data(copy, len);
    }

    return 0;
}

/* Delivers the ACK for OUR OWN outgoing fragment after a longer delay
 * (10ms) than the peer's incoming message below (2ms), so the two
 * events are deliberately staggered to land while packetizer_send_data()
 * is genuinely still mid-wait, without the two threads racing each
 * other into shared state at the same instant. Uses packetizer_send_ack()
 * directly (visible here via the white-box #include) rather than
 * looping our own DATA frame back, to keep "my own message's ack"
 * cleanly separate from "the peer's own, unrelated message" below. */
static void * deliver_own_ack_after_delay(void * arg)
{
    (void)arg;
    struct timespec delay = { .tv_sec = 0, .tv_nsec = 10000000 }; /* 10ms */
    nanosleep(&delay, NULL);

    uint16_t seq   = read_be16(&g_last_wire[OFFSET_SEQUENCE_NUMBER]);
    uint16_t frag  = read_be16(&g_last_wire[OFFSET_FRAGMENT_INDEX]);
    uint16_t count = read_be16(&g_last_wire[OFFSET_FRAGMENT_COUNT]);
    packetizer_send_ack(seq, frag, count);
    return NULL;
}

/* Simulates the remote peer sending us a message of their own at the
 * same time we are mid-way through sending ours: after a short delay
 * (well before deliver_own_ack_after_delay's, so it lands squarely
 * inside our own send's wait window), feeds a freshly built, unrelated
 * DATA frame straight into packetizer_receive_data() -- exactly as the
 * receiver thread in the real application does for a datagram that
 * just arrived on the socket. */
static void * deliver_peer_incoming_message(void * arg)
{
    (void)arg;
    struct timespec delay = { .tv_sec = 0, .tv_nsec = 2000000 }; /* 2ms */
    nanosleep(&delay, NULL);

    packetizer_frame_t frame = {0};
    uint8_t payload[] = "incoming from peer";
    /* A sequence_number that cannot collide with our own outgoing
     * message's (freshly assigned by reset_packetizer()'s
     * packetizer_init() starting from 1). */
    packetizer_pack_data(payload, (uint32_t)(sizeof(payload) - 1), 0, 1, 12345, &frame);

    uint8_t wire[PACKET_MTU];
    uint16_t len = packetizer_serialize_frame(&frame, wire);
    packetizer_receive_data(wire, len);
    return NULL;
}

static void test_full_duplex_send_and_receive_concurrently(void)
{
    reset_packetizer(transport_full_duplex);
    g_duplex_ack_sends = 0;

    pthread_t own_ack_tid, peer_msg_tid;
    pthread_create(&own_ack_tid, NULL, deliver_own_ack_after_delay, NULL);
    pthread_create(&peer_msg_tid, NULL, deliver_peer_incoming_message, NULL);

    const char * outgoing = "outgoing from us";
    int rc = packetizer_send_data((void *)outgoing, (uint32_t)strlen(outgoing));

    pthread_join(own_ack_tid, NULL);
    pthread_join(peer_msg_tid, NULL);

    /* Our own send completed successfully, undisturbed by the
     * concurrent, unrelated inbound traffic arriving mid-wait. */
    CHECK(rc == 0);
    CHECK(g_data_send_count == 1); /* no spurious retransmission */

    /* The peer's unrelated message, arriving mid-wait, was still
     * correctly validated, delivered to the application, and
     * acknowledged: receiving is not blocked by our own in-flight
     * send, nor does it disturb it. */
    CHECK(g_delivered_count == 1);
    CHECK(g_last_delivered_seq == 12345);
    /* Two ACK-type transport sends are expected here: one the
     * packetizer itself generated for the peer's incoming message
     * (processed above), and one from deliver_own_ack_after_delay's
     * direct packetizer_send_ack() call synthesizing the ack for our
     * own outgoing message -- both routed through the same
     * transport_full_duplex, hence both counted by this counter. */
    CHECK(g_duplex_ack_sends == 2);
}


int main(void)
{
    RUN_TEST(test_crc_valid_frame_is_accepted);
    RUN_TEST(test_crc_corrupted_payload_is_rejected);
    RUN_TEST(test_crc_corrupted_crc_field_is_rejected);
    RUN_TEST(test_crc_zero_length_payload_frame_round_trips);

    RUN_TEST(test_receive_rejects_null_or_undersized_input);
    RUN_TEST(test_receive_rejects_wrong_sof);
    RUN_TEST(test_receive_rejects_inconsistent_payload_length);
    RUN_TEST(test_receive_rejects_unknown_packet_type);
    RUN_TEST(test_receive_rejects_fragment_joining_mid_stream);
    RUN_TEST(test_send_rejects_invalid_arguments);
    RUN_TEST(test_send_rejects_message_too_large_to_fragment);
    RUN_TEST(test_init_rejects_null_send_fn);
    RUN_TEST(test_init_rejects_null_on_message);

    RUN_TEST(test_ack_received_in_time_completes_send);
    RUN_TEST(test_ack_timeout_retransmits_then_fails);

    RUN_TEST(test_nack_exhausting_retries_aborts_send);

    RUN_TEST(test_sequence_number_wraps_from_65535_to_1);
    RUN_TEST(test_messages_still_delivered_across_wraparound);

    RUN_TEST(test_full_duplex_send_and_receive_concurrently);

    printf("\n%d checks run, %d failed\n", g_checks_run, g_checks_failed);
    return g_checks_failed;
}