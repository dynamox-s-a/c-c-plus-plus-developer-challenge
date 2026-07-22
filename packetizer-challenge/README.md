# Dynamox C/C++ Developer Challenge

This repository contains a custom C++ network protocol implementation developed for the **Dynamox** technical evaluation challenge. 

# Design Overview

This project implements a lightweight reliable transport protocol on top of an unreliable network device. The goal is to provide reliable delivery while keeping the implementation simple, modular, and easy to understand.

The protocol is intentionally designed to resemble the core ideas of transport protocols such as TCP, while remaining significantly simpler and better suited for the requirements of this challenge.

# Architecture

The implementation is divided into two independent layers.

## Network Device

`NetworkDevice` is responsible only for packet transmission and reception.

It provides:

- Buffer allocation and release.
- Packet transmission.
- Packet reception with timeout support.
- MTU discovery.
- Address abstraction.

The protocol does not depend on any specific transport technology. Any implementation (UDP, simulated network, embedded hardware, etc.) only needs to implement the `NetworkDevice` interface.

## Dynamox Protocol

`DynamoxProtocol` is responsible for reliability.

Its responsibilities include:

- Packet fragmentation.
- Packet reassembly.
- CRC32 integrity verification.
- Acknowledgements (ACK).
- Retransmissions after timeout.
- Duplicate packet detection.
- Ordered message reconstruction.

The protocol operates entirely above the transport layer and treats the underlying network as unreliable.

# Packet Format

Each fragment contains a fixed header followed by the payload.

| Field | Description |
|--------|-------------|
| Sequence | Message identifier |
| Ack | Acknowledged sequence number |
| Size | Payload size |
| Fragment | Current fragment index |
| Fragments | Total number of fragments |
| Flags | Packet type (DATA / ACK) |
| CRC | CRC32 checksum |

# Sending

Sending follows the sequence below:

1. Split the message according to the MTU.
2. Add a protocol header to each fragment.
3. Compute the CRC32 checksum.
4. Send one fragment.
5. Wait for its ACK.
6. Retransmit on timeout until the retry limit is reached.
7. Continue with the next fragment.

This produces a simple stop-and-wait protocol where every fragment is individually acknowledged.

# Receiving

The receiver performs the following steps:

1. Receive a packet.
2. Validate the CRC.
3. Ignore corrupted packets.
4. Ignore invalid fragments.
5. Ignore duplicated fragments.
6. Store valid fragments.
7. Send an ACK immediately.
8. Return the reconstructed buffer once all fragments have been received.

# Reliability

Reliability is achieved through three mechanisms:

- CRC32 detects packet corruption.
- ACK packets confirm successful delivery.
- Retransmissions recover from packet loss.

Duplicate fragments are detected and acknowledged again without being processed twice.

# Buffer Management

The protocol uses a linked list of `NetworkBuffer` objects.

Advantages of this design:

- Large messages are fragmented without requiring contiguous memory.
- No additional copies are required during transmission.
- Reassembly simply reconnects the received buffers.

This minimizes memory overhead while keeping the implementation simple.

# Assumptions

The implementation makes the following assumptions:

- The underlying transport may lose packets.
- Packets may arrive corrupted.
- Duplicate packets may be received.
- Fragment ordering is preserved by the transport.
- Only one message is reassembled at a time.
- Messages are sent sequentially.
- Packet reordering is not supported.
- The maximum number of fragments is limited to 256.
- ACK packets are not retransmitted.

These assumptions significantly simplify the implementation while remaining adequate for the scope of this challenge.

# Error Handling

The protocol currently detects:

- Packet corruption (CRC mismatch)
- Packet loss (timeout)
- Duplicate fragments
- Invalid fragment indexes
- Allocation failures

Transmission fails if a fragment cannot be acknowledged after the configured retry limit.

## 🛠️ Prerequisites & Building

### Prerequisites
* C++17 (or higher) compliant compiler (`g++` or `clang++`)
* `make`
* Linux / POSIX-compliant environment
  
### Building the Project

To compile the source code, run:

```bash
make clean && make
```

## 💻 Step-by-Step Execution Tutorial

### Step 1: Start the Server

In your **first terminal**, launch the server instance. The server must be running first to listen for incoming packets on the `NetworkDevice` abstraction layer:

```bash
./build/Dynamox SERVER
```
### Step 2: Send Data from the Client

Open a **second terminal** and start the client. The client reads its input from **standard input (`stdin`)**, making it easy to integrate with common Unix utilities.

For example, to send the contents of a text file:

```bash
cat data/LoremIpsum.txt | ./build/Dynamox CLIENT
```

## 🧪 Running the Automated Tests

The repository includes an automated test script that starts the server in a separate terminal and executes predefined client-side transmission tests.

Simply run:

```bash
make tests
```

> **Note:** `make tests` requires either **`gnome-terminal`** or **`xterm`** to be installed so it can automatically launch the server process in a separate terminal window.



