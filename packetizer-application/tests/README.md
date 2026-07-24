# Auto test suite overview

This document summarizes the purpose of each automated test in the packetizer test suite.

## Build and run the test suite

From the packetizer-application/tests directory, run the following commands:

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
./test_packetizer
```

## CRC and frame validation
- test_crc_valid_frame_is_accepted: verifies that a correctly serialized frame passes CRC validation and is delivered to the application.
- test_crc_corrupted_payload_is_rejected: confirms that altering the payload bytes causes the frame to be rejected with an invalid-message error.
- test_crc_corrupted_crc_field_is_rejected: checks that tampering with the CRC bytes also causes the frame to be rejected.
- test_crc_zero_length_payload_frame_round_trips: ensures control frames with a zero-length payload still serialize and validate correctly.

## Input handling and frame validation
- test_receive_rejects_null_or_undersized_input: verifies that null or too-short input buffers are rejected.
- test_receive_rejects_wrong_sof: confirms frames with an invalid start-of-frame marker are rejected.
- test_receive_rejects_inconsistent_payload_length: checks that mismatches between the declared payload length and the actual input size fail validation.
- test_receive_rejects_unknown_packet_type: ensures unsupported packet types are rejected.
- test_receive_rejects_fragment_joining_mid_stream: confirms a fragment with an index greater than zero cannot be accepted when its starting fragment was never seen.

## Send and initialization behavior
- test_send_rejects_invalid_arguments: verifies invalid send parameters are rejected.
- test_send_rejects_message_too_large_to_fragment: checks that oversized payloads fail before fragmentation is attempted.
- test_init_rejects_null_send_fn: confirms initialization rejects a missing transport send function.
- test_init_rejects_null_on_message: checks that initialization also rejects a missing application callback.

## ACK and NACK handling
- test_ack_received_in_time_completes_send: ensures a timely ACK completes the send path successfully.
- test_ack_timeout_retransmits_then_fails: verifies that missing ACKs trigger retries and eventually fail with a timeout.
- test_nack_exhausting_retries_aborts_send: confirms repeated NACK responses cause the sender to give up after the configured retry limit.

## Sequence number wraparound
- test_sequence_number_wraps_from_65535_to_1: validates that the sequence counter wraps correctly from 65535 to 1 without using zero.
- test_messages_still_delivered_across_wraparound: ensures messages continue to be delivered and reassembled correctly after wraparound.

## Full-duplex behavior
- test_full_duplex_send_and_receive_concurrently: checks that sending a message and receiving an unrelated inbound message can happen at the same time without interfering with each other.
