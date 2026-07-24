/**
 * @file crc16.h
 * @brief CRC-16/CCITT (CRC-16/IBM-3740) — polynomial 0x1021, init 0xFFFF.
 *
 * This variant is widely used in embedded / industrial protocols (XMODEM,
 * Bluetooth, SD card) and detects all single-bit errors, all double-bit
 * errors (in messages up to 32767 bits), and most burst errors.
 */

#ifndef PKT_CRC16_H
#define PKT_CRC16_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compute CRC-16/CCITT over a buffer.
 *
 * @param data  Input bytes.
 * @param len   Number of bytes.
 * @return      16-bit CRC.
 */
uint16_t crc16_ccitt(const uint8_t *data, size_t len);

/**
 * Update a running CRC with one additional byte.
 * Initialise crc with 0xFFFF before the first call.
 */
uint16_t crc16_update(uint16_t crc, uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif /* PKT_CRC16_H */
