#include "crc16.h"

#define CRC16_POLY 0x1021U
#define CRC16_INIT 0xFFFFU

uint16_t crc16_update(uint16_t crc, uint8_t byte)
{
    crc ^= (uint16_t)((uint16_t)byte << 8);
    for (int i = 0; i < 8; i++) {
        if (crc & 0x8000U) {
            crc = (uint16_t)((crc << 1) ^ CRC16_POLY);
        } else {
            crc = (uint16_t)(crc << 1);
        }
    }
    return crc;
}

uint16_t crc16_ccitt(const uint8_t *data, size_t len)
{
    uint16_t crc = CRC16_INIT;
    for (size_t i = 0; i < len; i++) {
        crc = crc16_update(crc, data[i]);
    }
    return crc;
}
