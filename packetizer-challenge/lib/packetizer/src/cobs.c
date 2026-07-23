#include "cobs.h"
#include <stddef.h>

size_t cobs_encode(uint8_t *dst, const uint8_t *src, size_t src_len)
{
    size_t write_idx  = 0;
    size_t code_idx   = 0;   /* position of the current overhead byte */
    uint8_t code      = 1;   /* distance to next 0x00 (or end of block) */

    code_idx = write_idx;    /* reserve first overhead byte */
    write_idx++;

    for (size_t i = 0; i < src_len; i++) {
        if (src[i] == 0x00) {
            /* Finish current block */
            dst[code_idx] = code;
            code_idx = write_idx;
            write_idx++;
            code = 1;
        } else {
            dst[write_idx] = src[i];
            write_idx++;
            code++;
            if (code == 0xFF) {
                /* Block full (254 data bytes) — flush without a real zero */
                dst[code_idx] = code;
                code_idx = write_idx;
                write_idx++;
                code = 1;
            }
        }
    }

    dst[code_idx]  = code;    /* write final overhead byte */
    dst[write_idx] = 0x00;    /* packet delimiter */
    write_idx++;

    return write_idx;
}

size_t cobs_decode(uint8_t *dst, const uint8_t *src, size_t src_len)
{
    size_t read_idx  = 0;
    size_t write_idx = 0;

    while (read_idx < src_len) {
        uint8_t code = src[read_idx];
        read_idx++;

        if (code == 0x00) {
            /* 0x00 must not appear inside an encoded frame */
            return 0;
        }

        /* Copy (code - 1) non-zero bytes */
        uint8_t num_data = (uint8_t)(code - 1u);
        if (read_idx + num_data > src_len) {
            /* Truncated frame */
            return 0;
        }
        for (uint8_t i = 0; i < num_data; i++) {
            if (src[read_idx] == 0x00) {
                return 0; /* corrupt: 0x00 in data portion */
            }
            dst[write_idx] = src[read_idx];
            write_idx++;
            read_idx++;
        }

        /* If this wasn't the last block (code < 0xFF), a zero was encoded */
        if (code < 0xFF && read_idx < src_len) {
            dst[write_idx] = 0x00;
            write_idx++;
        }
    }

    return write_idx;
}
