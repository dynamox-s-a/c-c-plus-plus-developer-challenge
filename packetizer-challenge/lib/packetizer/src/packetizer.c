#include "../include/packetizer.h"
#include "packetizer_internal.h"
#include "cobs.h"
#include "crc16.h"

#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Estrutura interna                                                           */
/* -------------------------------------------------------------------------- */

struct packetizer_s {
    /* --- Configuração fornecida pelo usuário --- */
    pkt_config_t cfg;

    /* --- Estado de transmissão (TX) --- */
    tx_state_t   tx_state;
    uint8_t      tx_msg_id;      /* contador de mensagens, volta ao 0 após 255 */
    uint8_t      tx_frag_idx;    /* índice do fragmento sendo transmitido agora */
    uint8_t      tx_frag_total;  /* total de fragmentos da mensagem atual */
    uint8_t      tx_retries;     /* quantidade de retransmissões realizadas */
    uint32_t     tx_deadline;    /* prazo máximo para receber o ACK (em ms) */
    uint8_t*     tx_msg_buf;     /* cópia da mensagem alocada na heap */
    uint16_t     tx_msg_len;     /* tamanho da mensagem em bytes */

    /* --- Acumulador do fluxo de bytes recebidos (RX) --- */
    uint8_t      rx_raw[PKT_RX_BUF_SIZE];
    size_t       rx_raw_len;

    /* --- Slots de remontagem de mensagens (RX) --- */
    rx_slot_t    rx_slots[PKT_RX_SLOTS];

    /* Anel de MSG_IDs recentemente concluídos — permite enviar ACK para
     * duplicatas tardias sem entregar a mesma mensagem duas vezes. */
    uint8_t      rx_done_ids[PKT_RX_SLOTS];
    size_t       rx_done_head;
    size_t       rx_done_count;
};

/* -------------------------------------------------------------------------- */
/* Funções auxiliares internas                                                 */
/* -------------------------------------------------------------------------- */

/** Monta e transmite um pacote DATA para o fragmento de índice frag_idx. */
static int tx_send_fragment(packetizer_t* pkt, uint8_t frag_idx)
{
    /* --- Calcula o intervalo de bytes da mensagem que este fragmento carrega --- */
    uint16_t max_pl  = pkt->cfg.max_payload;
    uint16_t msg_len = pkt->tx_msg_len;

    uint16_t offset = (uint16_t)(frag_idx * max_pl);   /* byte inicial na mensagem */
    uint16_t pl_len = (uint16_t)(msg_len - offset);    /* bytes restantes */
    if (pl_len > max_pl) pl_len = max_pl;               /* limita ao payload máximo */

    /* --- Define as flags de posição do fragmento dentro da mensagem --- */
    uint8_t flags = 0;
    if (frag_idx == 0)              flags |= PKT_FLAG_FIRST; /* é o primeiro fragmento */
    if (offset + pl_len >= msg_len) flags |= PKT_FLAG_LAST;  /* é o último fragmento */

    /* --- Monta o cabeçalho do pacote no buffer local --- */
    uint8_t raw[PKT_RAW_MAX];
    raw[PKT_HDR_TYPE]        = PKT_TYPE_DATA;
    raw[PKT_HDR_MSG_ID]      = pkt->tx_msg_id;
    raw[PKT_HDR_FRAG_IDX]    = frag_idx;
    raw[PKT_HDR_FLAGS]       = flags;
    raw[PKT_HDR_MSG_LEN_HI]  = (frag_idx == 0) ? (uint8_t)(msg_len >> 8)   : 0;
    raw[PKT_HDR_MSG_LEN_LO]  = (frag_idx == 0) ? (uint8_t)(msg_len & 0xFF) : 0;
    raw[PKT_HDR_PAYLOAD_LEN] = (uint8_t)pl_len;

    /* --- Copia os bytes do fragmento logo após o cabeçalho --- */
    memcpy(&raw[PKT_HDR_SIZE], &pkt->tx_msg_buf[offset], pl_len);

    /* --- Calcula e anexa o CRC-16 ao final do pacote --- */
    size_t raw_len   = PKT_HDR_SIZE + pl_len;
    uint16_t crc     = crc16_ccitt(raw, raw_len);
    raw[raw_len++]   = (uint8_t)(crc >> 8);    /* byte alto do CRC */
    raw[raw_len++]   = (uint8_t)(crc & 0xFF);  /* byte baixo do CRC */

    /* --- Codifica com COBS: garante que 0x00 só aparece como delimitador --- */
    uint8_t encoded[COBS_ENCODED_MAX(PKT_RAW_MAX)];
    size_t enc_len = cobs_encode(encoded, raw, raw_len);

    /* --- Entrega os bytes codificados ao transporte configurado pelo usuário --- */
    return pkt->cfg.write(encoded, enc_len, pkt->cfg.user_ctx);
}

/** Monta e envia um pacote ACK ou NAK para a mensagem e fragmento indicados. */
static int tx_send_ack(packetizer_t* pkt, uint8_t type, uint8_t msg_id, uint8_t frag_idx)
{
    /* --- Monta o cabeçalho: ACK/NAK não carrega payload, apenas campos de controle --- */
    uint8_t raw[PKT_OVERHEAD];
    raw[PKT_HDR_TYPE]        = type;       /* PKT_TYPE_ACK ou PKT_TYPE_NAK */
    raw[PKT_HDR_MSG_ID]      = msg_id;
    raw[PKT_HDR_FRAG_IDX]    = frag_idx;
    raw[PKT_HDR_FLAGS]       = 0;
    raw[PKT_HDR_MSG_LEN_HI]  = 0;
    raw[PKT_HDR_MSG_LEN_LO]  = 0;
    raw[PKT_HDR_PAYLOAD_LEN] = 0;

    /* --- Calcula e anexa o CRC-16 ao final do pacote --- */
    size_t raw_len   = PKT_HDR_SIZE;
    uint16_t crc     = crc16_ccitt(raw, raw_len);
    raw[raw_len++]   = (uint8_t)(crc >> 8);    /* byte alto */
    raw[raw_len++]   = (uint8_t)(crc & 0xFF);  /* byte baixo */

    /* --- Codifica com COBS e entrega ao transporte --- */
    uint8_t encoded[COBS_ENCODED_MAX(PKT_OVERHEAD)];
    size_t enc_len = cobs_encode(encoded, raw, raw_len);

    return pkt->cfg.write(encoded, enc_len, pkt->cfg.user_ctx);
}

/** Busca um slot de remontagem ativo para msg_id, ou aloca um livre. */
static rx_slot_t* rx_get_slot(packetizer_t* pkt, uint8_t msg_id)
{
    /* --- Primeira passagem: procura slot já ativo para este msg_id --- */
    for (size_t i = 0; i < PKT_RX_SLOTS; i++) {
        if (pkt->rx_slots[i].active && pkt->rx_slots[i].msg_id == msg_id) {
            return &pkt->rx_slots[i];
        }
    }

    /* --- Segunda passagem: aloca qualquer slot livre --- */
    for (size_t i = 0; i < PKT_RX_SLOTS; i++) {
        if (!pkt->rx_slots[i].active) {
            return &pkt->rx_slots[i];
        }
    }

    return NULL; /* todos os slots estão ocupados */
}

/** Libera um slot de remontagem e zera sua memória. */
static void rx_free_slot(rx_slot_t* slot)
{
    /* --- Libera o buffer de remontagem e reinicia o slot (active volta a 0) --- */
    free(slot->buf);
    memset(slot, 0, sizeof(*slot));
}

/** Processa um pacote bruto já decodificado (COBS) e com CRC validado. */
static void process_packet(packetizer_t* pkt, const uint8_t* raw, size_t len)
{
    /* --- Sanidade mínima: descarta pacotes curtos demais para ter cabeçalho --- */
    if (len < PKT_OVERHEAD) return;

    /* --- Extrai os campos do cabeçalho para variáveis locais legíveis --- */
    uint8_t type     = raw[PKT_HDR_TYPE];
    uint8_t msg_id   = raw[PKT_HDR_MSG_ID];
    uint8_t frag_idx = raw[PKT_HDR_FRAG_IDX];
    uint8_t flags    = raw[PKT_HDR_FLAGS];
    uint16_t msg_len = (uint16_t)(((uint16_t)raw[PKT_HDR_MSG_LEN_HI] << 8) |
        raw[PKT_HDR_MSG_LEN_LO]);
    uint8_t pl_len   = raw[PKT_HDR_PAYLOAD_LEN];

    /* --- Verifica se o comprimento total bate com o declarado no cabeçalho --- */
    size_t expected_raw_len = PKT_HDR_SIZE + pl_len + PKT_CRC_SIZE;
    if (len != expected_raw_len) return;

    const uint8_t* payload = &raw[PKT_HDR_SIZE];

    /* ------------------------------------------------------------------ */
    /* ACK — confirmação de entrega de fragmento                           */
    /* ------------------------------------------------------------------ */
    if (type == PKT_TYPE_ACK) {
        if (pkt->tx_state == TX_WAIT_ACK &&
            msg_id == pkt->tx_msg_id &&
            frag_idx == pkt->tx_frag_idx) {

            uint8_t last_frag = (uint8_t)(pkt->tx_frag_total - 1u);

            if (frag_idx == last_frag) {
                /* Último fragmento confirmado: mensagem entregue com sucesso */
                pkt->tx_state = TX_IDLE;
                free(pkt->tx_msg_buf);
                pkt->tx_msg_buf = NULL;
                if (pkt->cfg.on_status) {
                    pkt->cfg.on_status(pkt->tx_msg_id,
                        PKT_STATUS_OK,
                        pkt->cfg.user_ctx);
                }
            }
            else {
                /* ACK parcial: avança para o próximo fragmento */
                pkt->tx_frag_idx++;
                pkt->tx_retries = 0;
                tx_send_fragment(pkt, pkt->tx_frag_idx);
                pkt->tx_deadline = 0; /* timer será rearmado no próximo pkt_tick() */
            }
        }
        return;
    }

    /* ------------------------------------------------------------------ */
    /* NAK — rejeição: receptor pediu retransmissão                        */
    /* ------------------------------------------------------------------ */
    if (type == PKT_TYPE_NAK) {
        if (pkt->tx_state == TX_WAIT_ACK &&
            msg_id == pkt->tx_msg_id &&
            frag_idx == pkt->tx_frag_idx) {
            /* Trata como timeout: força retransmissão imediata */
            pkt->tx_retries = pkt->cfg.max_retries; /* força o caminho de retransmissão */
            pkt->tx_deadline = 0;
        }
        return;
    }

    /* ------------------------------------------------------------------ */
    /* DATA — fragmento de dados recebido                                  */
    /* ------------------------------------------------------------------ */
    if (type == PKT_TYPE_DATA) {
        /* Verifica se este msg_id já foi entregue (duplicata tardia) */

        for (size_t i = 0; i < pkt->rx_done_count; i++) {
            size_t idx = (pkt->rx_done_head + PKT_RX_SLOTS - pkt->rx_done_count + i)
                % PKT_RX_SLOTS;
            if (pkt->rx_done_ids[idx] == msg_id) {
                tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);
                return;
            }
        }

        rx_slot_t* slot = rx_get_slot(pkt, msg_id);
        if (!slot) {
            /* Sem slot livre: envia NAK para o transmissor tentar mais tarde */
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }

        /* --- Inicializa o slot ao receber o primeiro fragmento --- */
        if (flags & PKT_FLAG_FIRST) {
            if (slot->active && slot->msg_id == msg_id) {
                /* Primeiro fragmento duplicado (retransmissão): ACK e ignora */
                tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);
                return;
            }
            if (msg_len == 0 || (uint32_t)msg_len > PKT_MAX_MSG_SIZE) {
                tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
                return;
            }
            slot->buf = (uint8_t*)malloc(msg_len);
            if (!slot->buf) {
                tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
                return;
            }
            slot->msg_len = msg_len;
            slot->received = 0;
            slot->msg_id = msg_id;
            slot->next_frag = 0;
            slot->active = 1;
        }

        if (!slot->active || slot->msg_id != msg_id) {
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }

        /* --- Detecção de fragmento duplicado ou fora de ordem --- */
        if (frag_idx < slot->next_frag) {
            /* Fragmento já recebido antes: reenvia ACK silenciosamente */
            tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);
            return;
        }
        if (frag_idx != slot->next_frag) {
            /* Fragmento fora de ordem: solicita retransmissão via NAK */
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }

        /* --- Copia o payload no buffer de remontagem --- */
        if ((uint32_t)slot->received + pl_len > slot->msg_len) {
            /* Overflow: payload excede o tamanho declarado — mensagem corrompida */
            rx_free_slot(slot);
            tx_send_ack(pkt, PKT_TYPE_NAK, msg_id, frag_idx);
            return;
        }
        memcpy(&slot->buf[slot->received], payload, pl_len);
        slot->received = (uint16_t)(slot->received + pl_len);
        slot->next_frag = (uint8_t)(slot->next_frag + 1u);

        /* Confirma o recebimento deste fragmento */
        tx_send_ack(pkt, PKT_TYPE_ACK, msg_id, frag_idx);

        /* --- Entrega a mensagem completa ao receber o último fragmento --- */
        if (flags & PKT_FLAG_LAST) {
            if (slot->received == slot->msg_len) {
                pkt->cfg.on_message(slot->buf, slot->received,
                    pkt->cfg.user_ctx);
            }
            /* Registra msg_id como concluído para ACK de duplicatas tardias */
            pkt->rx_done_ids[pkt->rx_done_head] = msg_id;
            pkt->rx_done_head = (pkt->rx_done_head + 1u) % PKT_RX_SLOTS;
            if (pkt->rx_done_count < PKT_RX_SLOTS) pkt->rx_done_count++;
            rx_free_slot(slot);
        }
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* API pública                                                                 */
/* -------------------------------------------------------------------------- */

packetizer_t* pkt_create(const pkt_config_t* cfg)
{
    /* --- Valida os callbacks obrigatórios antes de alocar qualquer recurso --- */
    if (!cfg || !cfg->write || !cfg->on_message) return NULL;

    /* --- Aloca e inicializa a estrutura na heap (calloc zera todos os campos) --- */
    packetizer_t* pkt = (packetizer_t*)calloc(1, sizeof(*pkt));
    if (!pkt) return NULL;
    pkt->cfg = *cfg;

    /* --- Aplica valores padrão para campos não preenchidos pelo usuário --- */
    if (pkt->cfg.max_payload == 0 || pkt->cfg.max_payload > PKT_MAX_PAYLOAD) {
        pkt->cfg.max_payload = PKT_MAX_PAYLOAD;
    }
    if (pkt->cfg.timeout_ms == 0) {
        pkt->cfg.timeout_ms = 500;
    }
    if (pkt->cfg.max_retries == 0) {
        pkt->cfg.max_retries = 5;
    }

    return pkt;
}

void pkt_destroy(packetizer_t* pkt)
{
    if (!pkt) return;

    /* --- Libera o buffer de transmissão, caso haja mensagem em voo --- */
    free(pkt->tx_msg_buf);

    /* --- Libera os buffers de remontagem de todos os slots RX --- */
    for (size_t i = 0; i < PKT_RX_SLOTS; i++) {
        free(pkt->rx_slots[i].buf);
    }

    /* --- Libera a estrutura principal --- */
    free(pkt);
}

int pkt_send(packetizer_t* pkt, const uint8_t* data, size_t len, uint8_t* msg_id)
{
    /* --- Valida os parâmetros e impede envios simultâneos --- */
    if (!pkt || !data || len == 0 || len > PKT_MAX_MSG_SIZE) return -1;
    if (pkt->tx_state != TX_IDLE) return -1;

    /* --- Copia a mensagem para buffer interno (a biblioteca gerencia a memória) --- */
    pkt->tx_msg_buf = (uint8_t*)malloc(len);
    if (!pkt->tx_msg_buf) return -1;
    memcpy(pkt->tx_msg_buf, data, len);
    pkt->tx_msg_len = (uint16_t)len;

    /* --- Inicializa o estado de transmissão para esta mensagem --- */
    pkt->tx_msg_id++;           /* contador 0-255, volta ao zero naturalmente */
    pkt->tx_frag_idx  = 0;      /* próximo fragmento a enviar */
    pkt->tx_retries   = 0;      /* contador de retransmissões */
    pkt->tx_deadline  = 0;      /* timer é armado na primeira chamada a pkt_tick() */

    /* --- Calcula quantos fragmentos serão necessários --- */
    uint16_t max_pl = pkt->cfg.max_payload;
    pkt->tx_frag_total = (uint8_t)(((uint16_t)len + max_pl - 1u) / max_pl);

    /* --- Marca como em voo e retorna o ID da mensagem ao chamador --- */
    pkt->tx_state = TX_WAIT_ACK;
    if (msg_id) *msg_id = pkt->tx_msg_id;

    /* --- Envia o primeiro fragmento; os demais serão enviados conforme ACKs chegam --- */
    return tx_send_fragment(pkt, 0);
}

int pkt_feed(packetizer_t* pkt, const uint8_t* data, size_t len)
{
    if (!pkt || !data) return -1;

    for (size_t i = 0; i < len; i++) {
        uint8_t byte = data[i];

        /* --- Byte 0x00: delimitador COBS — processa o frame acumulado --- */
        if (byte == 0x00) {
            if (pkt->rx_raw_len > 0) {
                /* --- Decodifica COBS; resultado é sempre menor que a entrada --- */
                uint8_t decoded[PKT_RX_BUF_SIZE];
                size_t dec_len = cobs_decode(decoded,
                    pkt->rx_raw,
                    pkt->rx_raw_len);

                if (dec_len >= PKT_OVERHEAD) {
                    /* --- Valida o CRC recebido contra o CRC recalculado --- */
                    size_t payload_end = dec_len - PKT_CRC_SIZE;
                    uint16_t received_crc =
                        (uint16_t)(((uint16_t)decoded[payload_end] << 8) |
                            decoded[payload_end + 1]);
                    uint16_t computed_crc = crc16_ccitt(decoded, payload_end);

                    if (received_crc == computed_crc) {
                        process_packet(pkt, decoded, dec_len);
                    }
                    /* Pacote corrompido: descarta silenciosamente.
                     * O transmissor vai detectar o timeout e retransmitir. */
                }
                pkt->rx_raw_len = 0;
            }
        }
        /* --- Byte de dado: acumula no buffer até o próximo delimitador --- */
        else {
            if (pkt->rx_raw_len < PKT_RX_BUF_SIZE) {
                pkt->rx_raw[pkt->rx_raw_len++] = byte;
            }
            else {
                /* Overflow do buffer: reseta e aguarda o próximo frame */
                pkt->rx_raw_len = 0;
            }
        }
    }

    return 0;
}

void pkt_tick(packetizer_t* pkt, uint32_t now_ms)
{
    /* --- Só há trabalho a fazer se uma mensagem está aguardando ACK --- */
    if (!pkt || pkt->tx_state != TX_WAIT_ACK) return;

    /* --- Primeira chamada após o envio: arma o timer definindo o prazo --- */
    if (pkt->tx_deadline == 0) {
        pkt->tx_deadline = now_ms + pkt->cfg.timeout_ms;
        return;
    }

    /* --- Prazo ainda não expirou: nada a fazer --- */
    if ((int32_t)(now_ms - pkt->tx_deadline) < 0) {
        return;
    }

    /* --- Timeout expirado: ACK não chegou a tempo --- */
    if (pkt->tx_retries >= pkt->cfg.max_retries) {
        /* --- Esgotou as tentativas: cancela a transmissão e notifica a aplicação --- */
        pkt->tx_state = TX_IDLE;
        free(pkt->tx_msg_buf);
        pkt->tx_msg_buf = NULL;
        if (pkt->cfg.on_status) {
            pkt->cfg.on_status(pkt->tx_msg_id,
                PKT_STATUS_ERROR,
                pkt->cfg.user_ctx);
        }
        return;
    }

    /* --- Retransmite o fragmento e reinicia o timer para a próxima tentativa --- */
    pkt->tx_retries++;
    pkt->tx_deadline = now_ms + pkt->cfg.timeout_ms;
    tx_send_fragment(pkt, pkt->tx_frag_idx);
}

int pkt_is_busy(const packetizer_t* pkt)
{
    /* --- Retorna 1 se há mensagem em voo aguardando ACK, 0 caso contrário --- */
    return (pkt && pkt->tx_state != TX_IDLE) ? 1 : 0;
}
