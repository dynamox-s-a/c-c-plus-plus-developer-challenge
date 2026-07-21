#ifndef __NETWORK_BUFFER__
#define __NETWORK_BUFFER__

#include <assert.h>

#include <network/NetworkAddress.hpp>

class NetworkBuffer {
 public:
  constexpr NetworkBuffer(void* start = 0, size_t tail = 0, size_t head = 0)
      : start_(static_cast<uint8_t*>(start)),
        head_(start_ + head),
        tail_(start_ + tail) {}

  virtual ~NetworkBuffer() = default;

  template <typename T = uint8_t*>
  [[nodiscard]]
  T data(this auto&& self) {
    return reinterpret_cast<T>(self.head_);
  }

  template <typename T = uint8_t*>
  [[nodiscard]]
  T start(this auto&& self) {
    return reinterpret_cast<T>(self.start_);
  }

  [[nodiscard]]
  uint8_t operator[](this auto&& self, size_t i) {
    NetworkBuffer* buffer = &self;
    size_t offset = i;

    while (buffer) {
      size_t length = buffer->tail_ - buffer->head_;
      if (offset < length) return buffer->head_[offset];

      offset -= length;
      buffer = buffer->next();
    }

    assert(false);
  }

  [[nodiscard]]
  size_t length() const {
    return static_cast<size_t>(tail_ - start_);
  }

  [[nodiscard]]
  size_t offset() const {
    return static_cast<size_t>(head_ - start_);
  }

  bool advance(size_t bytes) {
    head_ += bytes;
    return true;
  }

  bool rewind(size_t bytes) {
    head_ -= bytes;
    return true;
  }

  bool extend(size_t bytes) {
    tail_ += bytes;
    return true;
  }

  bool shrink(size_t bytes) {
    tail_ -= bytes;
    return true;
  }

  NetworkBuffer* next() const { return next_; }
  void next(NetworkBuffer* next) { next_ = next; }

  virtual NetworkAddress source() = 0;

 private:
  uint8_t* const start_;

  uint8_t* head_;
  uint8_t* tail_;

  NetworkBuffer* next_;
};

#endif
