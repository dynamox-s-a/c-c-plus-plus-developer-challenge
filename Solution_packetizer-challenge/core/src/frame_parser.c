#include <frame_parser.h>
#include <string.h>
#include <packet.h>


// calcula o tamanho do pacote serializado
static const size_t HEADER_SIZE = sizeof(uint16_t) + sizeof(Packet_type) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t);


// remove bytes que já foram procesados do buffer
static void frame_parser_consume(FrameParser* parser, size_t count)
{
    memmove(parser->buffer, parser->buffer + count, parser->length - count);
    parser->length -= count;
}


void frame_parser_init(FrameParser *parser)
{
    parser->length = 0;
}


// Adiciona novos bytes no buffer
bool frame_parser_append(FrameParser* parser, const uint8_t* data, size_t size)
{
    if (parser->length + size > FRAME_BUFFER_SIZE)
        return false;

    memcpy(parser->buffer + parser->length, data, size);
    parser->length += size;
    
    return true;
}


// se existe um pacote completo no buffer, extrai, e exclui o que não é nessário
bool frame_parser_next(FrameParser* parser, uint8_t* frame, size_t* frame_size)
{
    size_t i;

    for (i = 0; i + sizeof(uint16_t) <= parser->length; i++)
    {
        uint16_t start;

        memcpy(&start, parser->buffer + i, sizeof(start));

        if (start == PACKET_START_VALUE)
            break;
    }

    if (i == parser->length)
    {
        parser->length = 0;
        return false;
    }

    if (i > 0)
        frame_parser_consume(parser, i);

    if (parser->length < HEADER_SIZE)
        return false;

    size_t offset = sizeof(uint16_t) + sizeof(Packet_type);

    uint16_t payload_length;
    memcpy(&payload_length, parser->buffer + offset, sizeof(payload_length));
    size_t total_size = HEADER_SIZE + payload_length;

    if (parser->length < total_size)
        return false;

    memcpy(frame, parser->buffer, total_size);
    *frame_size = total_size;
    frame_parser_consume(parser, total_size);

    return true;
}