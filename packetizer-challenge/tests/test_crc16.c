/* CRC-16/CCITT unit tests */
#include "test_framework.h"
#include "../lib/packetizer/src/crc16.h"

/* Known-good vectors from https://crccalc.com (CRC-16/IBM-3740) */
TEST(crc16_empty)
{
    /* CRC of empty input = 0xFFFF (initial value) */
    ASSERT_EQ(crc16_ccitt(NULL, 0), 0xFFFF);
}

TEST(crc16_known_vector_123456789)
{
    /* "123456789" → 0x29B1 */
    const uint8_t data[] = "123456789";
    ASSERT_EQ(crc16_ccitt(data, 9), 0x29B1);
}

TEST(crc16_single_zero_byte)
{
    uint8_t b = 0x00;
    uint16_t crc = crc16_ccitt(&b, 1);
    /* Verify update gives same result */
    ASSERT_EQ(crc, crc16_update(0xFFFF, 0x00));
}

TEST(crc16_incremental_matches_bulk)
{
    const uint8_t data[] = {0x01, 0x02, 0x03, 0xAA, 0xFF, 0x00};
    uint16_t bulk = crc16_ccitt(data, sizeof(data));

    uint16_t inc = 0xFFFF;
    for (size_t i = 0; i < sizeof(data); i++) {
        inc = crc16_update(inc, data[i]);
    }
    ASSERT_EQ(bulk, inc);
}

TEST(crc16_detects_single_bit_flip)
{
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint16_t original = crc16_ccitt(data, sizeof(data));
    data[2] ^= 0x08;  /* flip one bit */
    uint16_t flipped = crc16_ccitt(data, sizeof(data));
    ASSERT_NE(original, flipped);
}
