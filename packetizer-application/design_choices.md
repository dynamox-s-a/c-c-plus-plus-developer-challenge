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