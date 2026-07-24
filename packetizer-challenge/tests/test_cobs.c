/* COBS framing unit tests */
#include "test_framework.h"
#include "../lib/packetizer/src/cobs.h"

#include <string.h>

TEST(cobs_round_trip_no_zeros)
{
    const uint8_t src[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t enc[COBS_ENCODED_MAX(sizeof(src))];
    uint8_t dec[sizeof(src)];

    size_t enc_len = cobs_encode(enc, src, sizeof(src));
    ASSERT(enc_len > 0);
    ASSERT_EQ(enc[enc_len - 1], 0x00);  /* must end with delimiter */

    size_t dec_len = cobs_decode(dec, enc, enc_len - 1);
    ASSERT_EQ(dec_len, sizeof(src));
    ASSERT_MEM_EQ(dec, src, sizeof(src));
}

TEST(cobs_round_trip_with_zeros)
{
    const uint8_t src[] = {0x00, 0x01, 0x00, 0x02, 0x00};
    uint8_t enc[COBS_ENCODED_MAX(sizeof(src))];
    uint8_t dec[sizeof(src) + 4];

    size_t enc_len = cobs_encode(enc, src, sizeof(src));
    ASSERT(enc_len > 0);

    /* Encoded must not contain 0x00 before the final delimiter */
    for (size_t i = 0; i < enc_len - 1; i++) {
        ASSERT_NE(enc[i], 0x00);
    }

    size_t dec_len = cobs_decode(dec, enc, enc_len - 1);
    ASSERT_EQ(dec_len, sizeof(src));
    ASSERT_MEM_EQ(dec, src, sizeof(src));
}

TEST(cobs_all_zeros)
{
    uint8_t src[8];
    memset(src, 0, sizeof(src));
    uint8_t enc[COBS_ENCODED_MAX(sizeof(src))];
    uint8_t dec[sizeof(src)];

    size_t enc_len = cobs_encode(enc, src, sizeof(src));
    size_t dec_len = cobs_decode(dec, enc, enc_len - 1);
    ASSERT_EQ(dec_len, sizeof(src));
    ASSERT_MEM_EQ(dec, src, sizeof(src));
}

TEST(cobs_254_byte_block_no_zeros)
{
    /* Exactly 254 non-zero bytes → overhead byte = 0xFF, no injected zero */
    uint8_t src[254];
    for (size_t i = 0; i < sizeof(src); i++) src[i] = (uint8_t)(i + 1);

    uint8_t enc[COBS_ENCODED_MAX(sizeof(src))];
    uint8_t dec[sizeof(src)];

    size_t enc_len = cobs_encode(enc, src, sizeof(src));
    ASSERT_EQ(enc[0], 0xFF);  /* full block indicator */

    size_t dec_len = cobs_decode(dec, enc, enc_len - 1);
    ASSERT_EQ(dec_len, sizeof(src));
    ASSERT_MEM_EQ(dec, src, sizeof(src));
}

TEST(cobs_empty_input)
{
    uint8_t enc[4];
    uint8_t dec[4];

    size_t enc_len = cobs_encode(enc, NULL, 0);
    ASSERT_EQ(enc_len, 2u);          /* overhead byte + delimiter */
    ASSERT_EQ(enc[0], 0x01);         /* points past itself, no zeros */
    ASSERT_EQ(enc[1], 0x00);

    size_t dec_len = cobs_decode(dec, enc, enc_len - 1);
    ASSERT_EQ(dec_len, 0u);
}

TEST(cobs_decode_corrupt_zero_in_data)
{
    /* A 0x00 inside encoded data (before delimiter) is invalid */
    uint8_t corrupt[] = {0x03, 0x01, 0x00, 0x01}; /* 0x00 in data position */
    uint8_t dec[8];
    size_t dec_len = cobs_decode(dec, corrupt, sizeof(corrupt));
    ASSERT_EQ(dec_len, 0u);  /* must detect corruption */
}
