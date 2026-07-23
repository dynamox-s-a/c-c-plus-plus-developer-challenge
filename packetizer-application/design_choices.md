# Design choices 

This document explains the reasoning behind the main design decisions made for the packetizer.

## Frame format

```text
+------+--------+------+--------+--------+--------+---------+-------+
| SOF  | Length | Type | Seq No | Frag # | Total  | Payload | CRC16 |
+------+--------+------+--------+--------+--------+---------+-------+
```

The packet format, represented by `packetizer_frame_t` in `packetizer.h`, consists of the following fields:

- **SOF**: Used to synchronize the receiver with the beginning of a packet. An EOF marker was intentionally omitted because the payload length field already defines the packet boundaries. This simplifies the framing logic by requiring only a single synchronization marker.

- **Payload length**: Specifies the payload size in bytes, excluding the header and CRC fields. This field allows the receiver to determine exactly how many bytes belong to the current packet.

- **Packet type**: Identifies the purpose of the packet (e.g., data, acknowledgment, or control message). This simplifies full-duplex communication, allowing acknowledgments and data packets to coexist without ambiguity.

- **Sequence number**: Monotonically increasing packet identifier used to detect duplicated packets and maintain packet ordering when necessary. Wrap-around is handled using modulo arithmetic instead of simple numerical comparison.

- **Fragment index**: Indicates the position of the current fragment within a fragmented message. Large messages are split into multiple packets before transmission.

- **Fragment count**: Indicates the total number of fragments that compose the original message, allowing the receiver to determine when reassembly is complete.

- **Payload**: Contains the application data. The payload is represented by a pointer rather than a fixed-size buffer to avoid unnecessary memory allocation and copying.

- **CRC**: Cyclic Redundancy Check used to verify packet integrity. Packets that fail the CRC validation are discarded.


## Packetizer core

- **Framing & serialization:** Frames start with a single `SOF` byte (0xAA). The in-memory `packetizer_frame_t` is serialized by `packetizer_serialize_frame()` into a fixed `PACKET_MTU` buffer using big-endian helpers (`write_be16`/`write_be32`).The Maximum Transmission Unit's value is chosen to mimic a low-power embedded device's limited transmission packet size, e.g. Bluetooth Low Energy. The CRC-32 covers everything from `packet_type` through the payload; the leading SOF is excluded.

- **CRC & validation:** A CRC-32 (`packetizer_crc32()`) protects each frame. Received frames are rejected on bad SOF, mismatched length, or CRC failures and never delivered to the application.

- **Fragmentation & reassembly:** Large messages are split into fragments of at most `PAYLOAD_MAX_SIZE`. Each fragment carries `sequence_number`, `fragment_index`, and `fragment_count`. The receiver tracks a single in-flight message via `g_reassembly` and hands validated fragment bytes directly to the application using the `on_message` callback rather than buffering the whole message.

- **Reliable delivery (stop-and-wait):** Sending uses a stop-and-wait scheme: each DATA fragment is sent and the sender blocks in `packetizer_wait_for_ack()` until an ACK arrives or timeouts/retries are exhausted. Retransmissions use the exact serialized bytes so no re-serialization is required on retry.

- **ACK / NACK handling:** The receiver acknowledges each accepted fragment with a PACKET_TYPE_ACK. A PACKET_TYPE_NACK sets `g_pending_ack.acked` to -1, causing the sender to retry immediately up to a NACK limit. ACKs and NACKs are matched by `sequence_number` and `fragment_index` to unblock the sender.

- **Transport-agnostic design & APIs:** The core is transport-agnostic: the caller provides a `send_fn` in `packetizer_init()`. Control frames and DATA frames share the same MTU-sized stack buffer to avoid extra allocations.

- **Timing, concurrency & limits:** Timeouts use `clock_gettime(CLOCK_MONOTONIC)`. The implementation assumes simple concurrency: a single message-in-flight for reassembly and a single pending fragment awaiting ACK (`g_pending_ack`), with `volatile` used for the ACK flag instead of mutexes or atomics. Errors are surfaced via standard errno codes (e.g., `EINVAL`, `EIO`, `ETIMEDOUT`, `EBADMSG`, `EMSGSIZE`).


