# Solução — Message Packetizer

## O que é este projeto?

É uma **biblioteca C para transmissão confiável de mensagens**, escrita do zero, pensada para funcionar sobre qualquer meio físico — serial, UDP, Bluetooth, o que for.

O problema que ela resolve é simples de entender: quando você manda dados por um canal de comunicação, coisas ruins podem acontecer. O pacote pode se corromper no caminho, pode se perder, pode chegar duplicado. A maioria dos protocolos de baixo nível simplesmente não se preocupa com isso — eles jogam os bytes no fio e torcem para o melhor.

Essa biblioteca resolve esse problema em três camadas:

**Primeiro**, ela embala cada mensagem num formato próprio com cabeçalho (tipo, ID, índice de fragmento, tamanho) e um checksum CRC-16 no final. Se um único bit virar durante a transmissão, o receptor detecta e descarta o pacote corrompido.

**Segundo**, ela usa COBS para garantir que o byte `0x00` nunca apareça dentro dos dados — só no fim do pacote, como marcador de quadro. Isso permite que o receptor saiba exatamente onde um pacote começa e termina, mesmo recebendo um fluxo contínuo de bytes.

**Terceiro**, ela implementa um protocolo de confirmação: o transmissor espera um ACK antes de mandar o próximo fragmento. Se o ACK não chega no prazo, ele retransmite. Se chegar um NAK, ele retransmite imediatamente. Mensagens grandes são automaticamente quebradas em fragmentos e remontadas do outro lado.

Junto com a biblioteca vêm um **app de demonstração** (dois terminais que trocam mensagens e arquivos em tempo real via UDP) e um **simulador de canal hostil** que injeta perdas, corrupções e duplicatas para estressar o protocolo — além de uma suíte de 19 testes automatizados cobrindo tudo isso.

---

## Como compilar e rodar

### Requisitos

- GCC com suporte a C99
- CMake 3.16 ou superior
- Linux (ou WSL no Windows)

### Compilar

```bash
cd packetizer-challenge
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Rodar os testes

```bash
./tests/unit_tests
```

### Rodar a demo (dois terminais)

**Terminal A:**
```bash
./apps/peer/peer --local-port 5000 --remote-port 5001
```

**Terminal B:**
```bash
./apps/peer/peer --local-port 5001 --remote-port 5000
```

Digite em qualquer terminal:
```
msg Olá mundo
file /caminho/para/arquivo.txt
```

### Rodar com canal hostil (simulador de perdas)

**Terminal 1 — Peer A:**
```bash
./apps/peer/peer --local-port 5000 --remote-port 5002
```

**Terminal 2 — Peer B:**
```bash
./apps/peer/peer --local-port 5001 --remote-port 5003
```

**Terminal 3 — Simulador (15% perda, 5% corrupção, 5% duplicação):**
```bash
./tools/channel_sim/channel_sim \
  --a-port 5002 --b-port 5003 \
  --a-dest 5001 --b-dest 5000 \
  --loss 15 --corrupt 5 --dup 5
```

---

## Estrutura do projeto

```
packetizer-challenge/
├── lib/packetizer/        # Biblioteca core (transport-agnostic)
│   ├── include/           # API pública (packetizer.h)
│   └── src/               # Implementação (packetizer.c, cobs.c, crc16.c)
├── transport/udp/         # Transporte UDP para a demo
├── apps/peer/             # App full-duplex (msg + file)
├── tools/channel_sim/     # Simulador de canal hostil
└── tests/                 # 19 testes automatizados
```

## Formato do pacote (no fio)

```
[ COBS overhead ] [ TYPE | MSG_ID | FRAG_IDX | FLAGS | MSG_LEN_HI | MSG_LEN_LO | PAYLOAD_LEN | PAYLOAD... | CRC16_HI | CRC16_LO ] [ 0x00 ]
```

- **TYPE**: DATA (0x01), ACK (0x02) ou NAK (0x03)
- **MSG_ID**: contador 0–255, identifica a mensagem
- **FRAG_IDX**: índice do fragmento dentro da mensagem
- **FLAGS**: FIRST (0x01) e LAST (0x02)
- **MSG_LEN**: tamanho total da mensagem original (só no primeiro fragmento)
- **PAYLOAD_LEN**: bytes de dado neste fragmento
- **CRC-16/CCITT**: cobre cabeçalho + payload (polinômio 0x1021, init 0xFFFF)
- **0x00**: delimitador de quadro COBS
