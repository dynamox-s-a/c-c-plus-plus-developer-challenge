#include <crc32.h>


// É usado uma lookup table para não precisar recalcular valores CRC.
// Gera valores CRC para valores de 0 a 255 (1 byte) usando o padrão reverso polinomial do CRC32
// Ao computar um CRC, é procurado na lookup table o valor existente
// Referência: https://wiki.osdev.org/CRC32

static uint32_t crc_lookup_table[256];
static int table_initialized = 0;

static void crc32_init(void)
{
    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t crc = i;

        for (int j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xEDB88320;
            else
                crc >>= 1;
        }

        crc_lookup_table[i] = crc;
    }

    table_initialized = 1;
}


uint32_t crc32(const uint8_t* data, size_t length)
{
    if (!table_initialized)
        crc32_init();

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++)
    {
        uint8_t index = (crc ^ data[i]) & 0xFF;
        crc = (crc >> 8) ^ crc_lookup_table[index];
    }

    return crc ^ 0xFFFFFFFF;
}