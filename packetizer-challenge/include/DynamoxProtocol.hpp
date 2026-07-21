#ifndef __DYNAMOX_PROTOCOL__
#define __DYNAMOX_PROTOCOL__

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <network/NetworkDevice.hpp>

struct PacketHeader {
  uint32_t sequence;
  uint32_t ack;
  uint16_t size;
  uint16_t flags;
  uint32_t crc;
};

struct Packet {
  PacketHeader header;

  template <typename T = uint8_t*>
  T data(this auto& self) {
    return reinterpret_cast<T>(reinterpret_cast<uint8_t*>(&self) +
                               sizeof(PacketHeader));
  }
};

class DynamoxProtocol {
  enum {
    ACK = 1 << 0,
    DATA = 1 << 1,
  };

 public:
  explicit DynamoxProtocol(NetworkDevice& device)
      : device_(device), sequence_(0) {}

  NetworkBuffer* alloc(size_t size) {
    NetworkBuffer* buffer = device_.alloc(size + sizeof(PacketHeader));
    if (buffer) buffer->advance(sizeof(PacketHeader));
    return buffer;
  }

  void free(NetworkBuffer* buffer) { return device_.free(buffer); }

  void release(NetworkBuffer* buffer) { return device_.release(buffer); }

  int send(NetworkBuffer* buffer, const NetworkAddress destination,
           uint32_t timeout = DefaultTimeout, uint32_t retries = 5) {
    buffer->rewind(sizeof(PacketHeader));

    Packet* packet = buffer->data<Packet*>();
    uint32_t sequence = sequence_++;

    packet->header.sequence = sequence;
    packet->header.ack = 0;
    packet->header.size =
        static_cast<uint16_t>(buffer->length() - sizeof(PacketHeader));
    packet->header.flags = DATA;
    packet->header.crc = 0;
    packet->header.crc = crc32(packet, buffer->length());

    bool acked = false;
    for (uint32_t retry = 0; retry <= retries && !acked; ++retry) {
      device_.send(buffer, destination);
      if (join(sequence, timeout)) {
        acked = true;
        break;
      }
    }

    buffer->advance(sizeof(PacketHeader));
    if (acked) return buffer->length() - buffer->offset();
    return -1;
  }

  NetworkBuffer* receive(uint32_t timeout = DefaultTimeout) {
    uint64_t start = ms();

    while (true) {
      uint64_t current = ms();
      uint64_t elapsed = current - start;

      if (elapsed >= timeout) {
        return nullptr;
      }

      uint32_t remaining = static_cast<uint32_t>(timeout - elapsed);
      NetworkBuffer* buffer = device_.receive(remaining);
      if (!buffer) {
        continue;
      }

      Packet* packet = buffer->data<Packet*>();
      uint32_t crc = packet->header.crc;
      packet->header.crc = 0;

      uint32_t calculated =
          crc32(packet, sizeof(PacketHeader) + packet->header.size);

      if (crc == calculated && (packet->header.flags & DATA)) {
        ack(packet->header.sequence, buffer->source());
        buffer->advance(sizeof(PacketHeader));
        return buffer;
      }

      device_.free(buffer);
    }
  }

  bool ack(uint32_t sequence, const NetworkAddress& destination) {
    NetworkBuffer* buffer = device_.alloc(sizeof(PacketHeader));
    bool response = false;

    if (buffer) {
      Packet* packet = buffer->data<Packet*>();
      packet->header.sequence = sequence_++;
      packet->header.ack = sequence;
      packet->header.size = 0;
      packet->header.flags = ACK;
      packet->header.crc = 0;
      packet->header.crc = crc32(packet, sizeof(PacketHeader));

      response = device_.send(buffer, destination);
      device_.free(buffer);
    }
    return response;
  }

  bool join(uint32_t sequence, uint32_t timeout) {
    uint64_t start = ms();
    bool acked = false;

    while (true) {
      uint64_t current = ms();
      uint64_t elapsed = current - start;

      if (elapsed >= timeout) {
        return false;
      }

      uint32_t remaining = static_cast<uint32_t>(timeout - elapsed);
      NetworkBuffer* buffer = device_.receive(remaining);
      if (!buffer) {
        continue;
      }

      Packet* packet = buffer->data<Packet*>();
      uint32_t crc = packet->header.crc;
      packet->header.crc = 0;

      uint32_t calculated =
          crc32(packet, sizeof(PacketHeader) + packet->header.size);

      if (crc == calculated) {
        if ((packet->header.flags & ACK) && (packet->header.ack == sequence)) {
          acked = true;
        }
      }

      device_.free(buffer);

      if (acked) break;
    }
    return acked;
  }

  static uint64_t ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * 1000 + (ts.tv_nsec / 1000000);
  }

  uint32_t crc32(const void* data, size_t length) {
    const uint8_t* bytes = static_cast<const uint8_t*>(data);
    uint32_t crc = 0xFFFFFFFF;

    while (length--) {
      crc ^= *bytes++;
      for (int i = 0; i < 8; ++i) {
        if (crc & 1)
          crc = (crc >> 1) ^ 0xEDB88320;
        else
          crc >>= 1;
      }
    }
    return ~crc;
  }

 private:
  static constexpr size_t DefaultTimeout = 2000;

 private:
  NetworkDevice& device_;
  uint32_t sequence_;
};

#endif
