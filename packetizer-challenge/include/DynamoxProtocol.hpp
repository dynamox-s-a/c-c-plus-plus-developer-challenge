#ifndef __DYNAMOX_PROTOCOL__
#define __DYNAMOX_PROTOCOL__

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <network/NetworkDevice.hpp>

class DynamoxProtocol {
  static constexpr size_t DefaultTimeout = 2000;
  static constexpr size_t DefaultRetries = 5;
  static constexpr size_t Reassemblies = 16;
  static constexpr size_t Fragments = 64;

  struct Header {
    uint32_t sequence;
    uint32_t ack;
    uint16_t size;
    uint16_t fragment;
    uint16_t fragments;
    uint16_t flags;
    uint32_t crc;
  };

  struct Packet {
    Header header;

    template <typename T = uint8_t*>
    T data(this auto& self) {
      return reinterpret_cast<T>(reinterpret_cast<uint8_t*>(&self) +
                                 sizeof(Header));
    }
  };

  struct Reassembly {
    uint32_t sequence = 0;
    uint16_t fragments = 0;
    uint16_t received = 0;

    NetworkAddress source;
    NetworkBuffer* buffers[Fragments] = {};
  };

  enum {
    ACK = 1 << 0,
    DATA = 1 << 1,
  };

 public:
  explicit DynamoxProtocol(NetworkDevice& device)
      : device_(device), sequence_(0) {}

  NetworkBuffer* alloc(size_t size) {
    const size_t mtu = device_.mtu();

    NetworkBuffer* first = nullptr;
    NetworkBuffer* current = nullptr;

    while (size > 0) {
      size_t chunk =
          (size + sizeof(Header) > mtu) ? mtu - sizeof(Header) : size;

      NetworkBuffer* buffer = device_.alloc(chunk + sizeof(Header));

      if (!buffer) {
        free(first);
        return nullptr;
      }

      buffer->advance(sizeof(Header));
      buffer->next(nullptr);

      if (!first)
        first = buffer;
      else
        current->next(buffer);

      current = buffer;
      size -= chunk;
    }

    return first;
  }

  void free(NetworkBuffer* buffer) {
    while (buffer) {
      NetworkBuffer* next = buffer->next();
      device_.free(buffer);
      buffer = next;
    }
  }

  void release(NetworkBuffer* buffer) { device_.release(buffer); }

  int send(NetworkBuffer* buffer, const NetworkAddress& destination,
           uint32_t timeout = DefaultTimeout,
           uint32_t retries = DefaultRetries) {
    uint32_t sequence = sequence_++;
    size_t total = 0;

    uint16_t fragments = 0;
    for (auto* b = buffer; b; b = b->next()) fragments++;

    uint16_t fragment = 0;

    for (auto* b = buffer; b; b = b->next(), fragment++) {
      b->rewind(sizeof(Header));

      Packet* packet = b->data<Packet*>();

      packet->header.sequence = sequence;
      packet->header.ack = 0;
      packet->header.fragment = fragment;
      packet->header.fragments = fragments;
      packet->header.size = static_cast<uint16_t>(b->length() - sizeof(Header));
      packet->header.flags = DATA;
      packet->header.crc = 0;
      packet->header.crc = crc32(packet, b->length());

      bool acked = false;

      for (uint32_t retry = 0; retry <= retries && !acked; retry++) {
        device_.send(b, destination);
        acked = join(sequence, fragment, timeout);
      }

      if (!acked) {
        b->advance(sizeof(Header));
        return -1;
      }

      b->advance(sizeof(Header));
      total += b->length() - b->offset();
    }

    return total;
  }

  NetworkBuffer* receive(uint32_t timeout = DefaultTimeout) {
    Reassembly reassembly;
    uint64_t start = ms();

    while (true) {
      uint64_t current = ms();
      uint64_t elapsed = current - start;

      if (elapsed >= timeout) {
        for (size_t i = 0; i < reassembly.fragments; i++) {
          if (reassembly.buffers[i]) {
            device_.free(reassembly.buffers[i]);
          }
        }
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

      uint32_t calculated = crc32(packet, sizeof(Header) + packet->header.size);

      if (crc != calculated || !(packet->header.flags & DATA)) {
        device_.free(buffer);
        continue;
      }

      ack(packet->header.sequence, packet->header.fragment, buffer->source());

      if (packet->header.fragments == 0 ||
          packet->header.fragments > Fragments) {
        device_.free(buffer);
        continue;
      }

      if (packet->header.fragments == 1) {
        if (reassembly.received == 0) {
          buffer->advance(sizeof(Header));
          return buffer;
        } else {
          device_.free(buffer);
          continue;
        }
      }

      if (reassembly.received == 0) {
        reassembly.sequence = packet->header.sequence;
        reassembly.fragments = packet->header.fragments;
        reassembly.source = buffer->source();
      }

      if (packet->header.sequence != reassembly.sequence) {
        device_.free(buffer);
        continue;
      }

      uint16_t fragment = packet->header.fragment;

      if (fragment >= Fragments || fragment >= reassembly.fragments) {
        device_.free(buffer);
        continue;
      }

      if (reassembly.buffers[fragment]) {
        device_.free(buffer);
        continue;
      }

      buffer->advance(sizeof(Header));

      reassembly.buffers[fragment] = buffer;
      reassembly.received++;

      if (reassembly.received != reassembly.fragments) continue;

      for (uint16_t i = 0; i + 1 < reassembly.fragments; i++)
        reassembly.buffers[i]->next(reassembly.buffers[i + 1]);

      reassembly.buffers[reassembly.fragments - 1]->next(nullptr);

      return reassembly.buffers[0];
    }
  }

  bool ack(uint32_t sequence, uint16_t fragment,
           const NetworkAddress& destination) {
    NetworkBuffer* buffer = device_.alloc(sizeof(Header));
    bool response = false;

    if (buffer) {
      Packet* packet = buffer->data<Packet*>();
      packet->header.sequence = sequence_++;
      packet->header.ack = sequence;
      packet->header.fragment = fragment;
      packet->header.fragments = 1;
      packet->header.size = 0;
      packet->header.flags = ACK;
      packet->header.crc = 0;
      packet->header.crc = crc32(packet, sizeof(Header));

      response = device_.send(buffer, destination);
      device_.free(buffer);
    }
    return response;
  }

  bool join(uint32_t sequence, uint32_t fragment, uint32_t timeout) {
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

      uint32_t calculated = crc32(packet, sizeof(Header) + packet->header.size);

      if (crc == calculated) {
        if ((packet->header.flags & ACK) && (packet->header.ack == sequence) &&
            packet->header.fragment == fragment) {
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

  static uint32_t crc32(const void* data, size_t length) {
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
  NetworkDevice& device_;
  uint32_t sequence_;
};

#endif
