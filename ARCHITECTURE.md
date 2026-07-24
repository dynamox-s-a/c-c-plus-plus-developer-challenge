# Arquitetura do Projeto — Packetizer Challenge

## Visão Geral

Este projeto implementa uma **biblioteca de packetização** para comunicação confiável entre dispositivos IoT em ambientes industriais ruidosos (cenário: sensores de vibração similares ao produto Dynamox, comunicando via BLE em ambiente com alto ruído eletromagnético).

A biblioteca é **independente de transporte** (transport-agnostic): ela não sabe nada sobre UDP, BLE, UART ou qualquer outro meio físico. Ela apenas transforma mensagens da aplicação em pacotes e reconstrói as mensagens no outro lado, garantindo entrega confiável.

---

## Estrutura de Diretórios

```
packetizer-challenge/
│
├── CMakeLists.txt              ← Build system raiz
│
├── lib/
│   └── packetizer/             ← Biblioteca core (transport-agnostic)
│       ├── CMakeLists.txt
│       ├── include/
│       │   └── packetizer.h    ← API pública da biblioteca
│       └── src/
│           ├── crc16.h/c       ← Cálculo de integridade CRC-16/CCITT
│           ├── cobs.h/c        ← Framing COBS (delimitação de pacotes)
│           ├── packetizer_internal.h  ← Formato do pacote no fio + estados internos
│           └── packetizer.c    ← Implementação do protocolo ARQ
│
├── transport/
│   └── udp/                    ← Transporte UDP (demo)
│       ├── CMakeLists.txt
│       ├── udp_transport.h     ← Interface do transporte
│       └── udp_transport.c     ← Implementação com sockets UDP non-blocking
│
├── apps/
│   └── peer/                   ← Aplicação demo full-duplex
│       ├── CMakeLists.txt
│       └── main.c              ← App interativa (enviar msg e arquivos)
│
├── tools/
│   └── channel_sim/            ← Simulador de canal hostil
│       ├── CMakeLists.txt
│       └── main.c              ← Proxy UDP que injeta perda/corrupção/duplicação
│
└── tests/
    ├── CMakeLists.txt
    ├── test_framework.h        ← Framework de teste mínimo (sem dependências)
    ├── main.c                  ← Ponto de entrada dos testes
    ├── test_crc16.c            ← Testes unitários do CRC-16
    ├── test_cobs.c             ← Testes unitários do COBS
    └── test_packetizer.c       ← Testes de integração do protocolo
```

---

## Descrição Detalhada de Cada Arquivo

### `lib/packetizer/`

É o coração do projeto. Compilada como biblioteca estática (`libpacketizer.a`). **Não contém nenhuma chamada de sistema** (sem `read()`, `write()`, `sleep()`, `select()`). Toda comunicação com o mundo externo ocorre via callbacks.

---

#### `include/packetizer.h` — API Pública

Define a interface que a aplicação usa. Funções principais:

| Função | O que faz |
|--------|-----------|
| `pkt_create(cfg)` | Cria uma instância do packetizador com callbacks e configurações |
| `pkt_destroy(pkt)` | Libera todos os recursos |
| `pkt_send(pkt, data, len, &id)` | Envia uma mensagem (fragmenta automaticamente se necessário) |
| `pkt_feed(pkt, data, len)` | Alimenta bytes recebidos do transporte |
| `pkt_tick(pkt, now_ms)` | Avança o timer interno (detecta timeout de ACK e retransmite) |
| `pkt_is_busy(pkt)` | Retorna 1 se há mensagem aguardando ACK |

**Callbacks configuráveis:**

- `write(data, len, ctx)` → chamado para enviar bytes pelo transporte
- `on_message(data, len, ctx)` → chamado quando uma mensagem completa é recebida
- `on_status(msg_id, status, ctx)` → informa se a entrega foi OK ou falhou

---

#### `src/crc16.h` / `src/crc16.c` — Integridade CRC-16/CCITT

Implementa o **CRC-16/IBM-3740** (polinômio `0x1021`, init `0xFFFF`).

- Detecta todos os erros de 1 bit e 2 bits em mensagens de até 32.767 bits
- Detecta a maioria dos erros em rajada (burst errors), comuns em ambientes industriais
- Amplamente usado em protocolos industriais: XMODEM, Bluetooth, SD Card
- Dois pontos de entrada: `crc16_ccitt(buf, len)` e `crc16_update(crc, byte)` para cálculo incremental

---

#### `src/cobs.h` / `src/cobs.c` — Framing COBS

**COBS** (Consistent Overhead Byte Stuffing) é a camada de delimitação de pacotes.

**Problema que resolve:** Em um fluxo de bytes, como saber onde um pacote começa e termina? A solução mais simples seria usar um byte especial como delimitador (`0x00`), mas e se o dado contém `0x00`?

**Solução COBS:** Recodifica o bloco de dados para que `0x00` nunca apareça no payload codificado. Assim, um único byte `0x00` pode ser usado de forma confiável como delimitador de pacote no stream.

- Overhead: no máximo 1 byte a cada 254 bytes de entrada
- Determinístico: sem escaping, sem bytes especiais no meio do dado
- Se o decodificador encontrar `0x00` no meio do frame (antes do delimitador): o frame está corrompido → descartado

---

#### `src/packetizer_internal.h` — Formato do Pacote no Fio

Define o formato binário de cada pacote **após decodificação COBS**:

```
Byte 0    : TYPE       (DATA=0x01 | ACK=0x02 | NAK=0x03)
Byte 1    : MSG_ID     (identificador da mensagem, 0-255, rolling)
Byte 2    : FRAG_IDX  (índice do fragmento dentro da mensagem)
Byte 3    : FLAGS      (bit0=FIRST | bit1=LAST)
Bytes 4-5 : MSG_LEN    (tamanho total da mensagem, big-endian, válido só no FIRST)
Byte 6    : PAYLOAD_LEN (bytes de payload neste pacote, 0-240)
Bytes 7…  : PAYLOAD    (dados da aplicação)
Últimos 2  : CRC16     (big-endian, cobre todos os bytes anteriores)
```

Overhead por pacote: **9 bytes** (7 header + 2 CRC).

---

#### `src/packetizer.c` — Protocolo ARQ Stop-and-Wait

Implementa a lógica completa do protocolo. Duas máquinas de estado independentes:

**Estado TX (transmissão):**
1. `TX_IDLE` → aguardando uma mensagem para enviar
2. `TX_WAIT_ACK` → fragmento enviado, aguardando ACK

Fluxo: fragmenta mensagem → envia fragmento 0 → aguarda ACK → envia fragmento 1 → … → ACK do último fragmento → `on_status(OK)`.

Se o timer expira sem ACK: retransmite. Após `max_retries` tentativas sem sucesso: `on_status(ERROR)`.

**Estado RX (recepção):**
- Acumula bytes brutos até encontrar o delimitador `0x00`
- Decodifica COBS
- Valida CRC (descarta silenciosamente se inválido)
- Processa o pacote:
  - `ACK` → avança o estado TX
  - `NAK` → retransmite imediatamente
  - `DATA` → verifica duplicata, monta buffer de reassembly, entrega quando completo
- Detecção de duplicatas pós-entrega: mantém um ring buffer com os últimos MSG_IDs concluídos. Se um duplicado chegar após a entrega, envia ACK sem re-entregar.

---

### `transport/udp/`

#### `udp_transport.h` / `udp_transport.c` — Transporte UDP

Implementação do transporte para o demo. Características:

- Sockets UDP **non-blocking** (O_NONBLOCK)
- Bind em `local_port`, envia para `remote_host:remote_port`
- Expõe `udp_fd()` para uso com `select()`/`poll()`
- Facilmente substituível por outro transporte (serial, BLE, pipe nomeado) sem alterar a biblioteca core

---

### `apps/peer/`

#### `main.c` — Aplicação Demo Full-Duplex

Aplicação interativa que roda como **dois processos** (A e B) se comunicando pelo packetizador via UDP.

**Arquitetura de threads:**
- **Thread principal:** lê stdin e chama `pkt_send()`
- **Thread de rede:** recebe datagrams UDP, chama `pkt_feed()` e `pkt_tick()` a cada 10ms
- Ambas compartilham a instância do packetizador protegida por `pthread_mutex`

**Comandos disponíveis:**
```
msg <texto>    → Envia mensagem de texto
file <caminho> → Envia um arquivo (salvo como "received_<nome>" no destino)
quit           → Encerra
```

**Protocolo de arquivo:** prefixo `FILE:<nome>\n<conteúdo>` — autocontido dentro do payload da mensagem.

---

### `tools/channel_sim/`

#### `main.c` — Simulador de Canal Hostil

Proxy UDP bidirecional que senta entre os dois peers e injeta falhas aleatórias. Demonstra o **requisito bônus 1** do desafio.

```
Peer A → [channel_sim porta A] ~~~ [canal hostil] ~~~ [channel_sim porta B] → Peer B
```

**Falhas injetáveis (probabilidade configurável em %):**
- **Loss** (`--loss`): descarta o pacote inteiro
- **Corrupt** (`--corrupt`): inverte bits aleatórios no payload
- **Dup** (`--dup`): entrega o pacote duas vezes

Exibe log em tempo real de cada evento e estatísticas ao sair.

---

### `tests/`

#### `test_framework.h` + `main.c` — Framework de Teste Mínimo

Framework de teste escrito em C puro, sem dependências externas. Adequado para embarcados (sem `malloc` no framework, sem C++).

- Macro `TEST(nome)` registra o teste via `__attribute__((constructor))`
- Macros: `ASSERT`, `ASSERT_EQ`, `ASSERT_NE`, `ASSERT_MEM_EQ`, `FAIL`
- Imprime PASS/FAIL por teste e resultado final

---

#### `test_crc16.c` — 5 Testes do CRC-16

| Teste | Verifica |
|-------|----------|
| `crc16_empty` | Input vazio retorna 0xFFFF (valor inicial) |
| `crc16_known_vector_123456789` | Vetor padrão "123456789" → 0x29B1 |
| `crc16_single_zero_byte` | Consistência entre bulk e update incremental |
| `crc16_incremental_matches_bulk` | Resultado incremental == bulk para dados arbitrários |
| `crc16_detects_single_bit_flip` | Um flip de bit sempre gera CRC diferente |

---

#### `test_cobs.c` — 6 Testes do COBS

| Teste | Verifica |
|-------|----------|
| `cobs_round_trip_no_zeros` | Encode/decode sem zeros no input |
| `cobs_round_trip_with_zeros` | Encode/decode com zeros; nenhum 0x00 no encoded |
| `cobs_all_zeros` | Input composto inteiramente de zeros |
| `cobs_254_byte_block_no_zeros` | Bloco de 254 bytes sem zero (overhead byte = 0xFF) |
| `cobs_empty_input` | Input vazio produz frame mínimo válido |
| `cobs_decode_corrupt_zero_in_data` | 0x00 dentro do frame encoded → decodificação retorna 0 |

---

#### `test_packetizer.c` — 8 Testes de Integração

Usa um **harness in-process**: instâncias A e B conectadas por callbacks em memória. Simula o canal com flags `drop_next`, `drop_all`, `corrupt_next`, `dup_next`.

| Teste | Verifica |
|-------|----------|
| `send_short_message` | Mensagem de texto entregue corretamente |
| `send_binary_message_with_zeros` | Dados binários com bytes 0x00 sobrevivem ao COBS |
| `fragmented_message` | Mensagem grande fragmentada (payload=10) e remontada |
| `corrupt_packet_is_rejected_and_retransmitted` | CRC inválido → pacote descartado → retransmissão entrega corretamente |
| `dropped_packet_is_retransmitted` | Pacote perdido → timeout → retransmissão entrega |
| `duplicated_packet_delivered_once` | Pacote duplicado → entregue exatamente uma vez |
| `max_retries_exceeded_reports_error` | Link morto → `on_status(ERROR)` após esgotar tentativas |
| `busy_returns_error_when_inflight` | Segunda chamada a `pkt_send` com mensagem em voo retorna -1 |

---

## Como Compilar e Executar

```bash
# Configurar e compilar
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Rodar os testes
./tests/unit_tests

# Demo direto (dois terminais):
./apps/peer/peer --local-port 5000 --remote-port 5001   # Terminal A
./apps/peer/peer --local-port 5001 --remote-port 5000   # Terminal B

# Demo com canal hostil (quatro terminais):
./tools/channel_sim/channel_sim \
    --a-port 5002 --b-port 5003 \
    --a-dest 5001 --b-dest 5000 \
    --loss 15 --corrupt 5 --dup 5        # Terminal 1: simulador

./apps/peer/peer --local-port 5000 --remote-port 5002   # Terminal 2: Peer A
./apps/peer/peer --local-port 5001 --remote-port 5003   # Terminal 3: Peer B
```
