/**
 * @file cobs.h
 * @brief Consistent Overhead Byte Stuffing (COBS) framing.
 *
 * COBS encodes a block of bytes so that 0x00 never appears in the encoded
 * output.  A single 0x00 byte is then used as a reliable packet delimiter in
 * the byte stream, solving framing without needing escape sequences.
 *
 * Overhead: at most 1 byte per 254 input bytes, plus one trailing 0x00.
 * Worst case encoded size = input_len + ceil(input_len / 254) + 1.
 *
 * Reference: Cheshire & Baker, "Consistent Overhead Byte Stuffing", IEEE/ACM
 * Transactions on Networking, 1999.
 */

#ifndef PKT_COBS_H
#define PKT_COBS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Worst-case encoded buffer size needed for src_len input bytes. */
#define COBS_ENCODED_MAX(n) ((n) + (n) / 254u + 2u)

/**
 * Encode @p src_len bytes of @p src into @p dst using COBS.
 *
 * @param dst      Output buffer — must be at least COBS_ENCODED_MAX(src_len).
 * @param src      Input data.
 * @param src_len  Number of input bytes.
 * @return         Number of bytes written to @p dst (includes trailing 0x00).
 */
size_t cobs_encode(uint8_t *dst, const uint8_t *src, size_t src_len);

/**
 * Decode a COBS-encoded frame into @p dst.
 *
 * @param dst      Output buffer — must be at least @p src_len bytes.
 * @param src      Encoded bytes, **without** the trailing 0x00 delimiter.
 * @param src_len  Number of encoded bytes (not counting the 0x00).
 * @return         Number of decoded bytes, or 0 on encoding error.
 */
size_t cobs_decode(uint8_t *dst, const uint8_t *src, size_t src_len);

#ifdef __cplusplus
}
#endif

#endif /* PKT_COBS_H */
