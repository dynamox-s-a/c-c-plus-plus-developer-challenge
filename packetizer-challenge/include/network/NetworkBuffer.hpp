#ifndef __NETWORK_BUFFER__
#define __NETWORK_BUFFER__

class NetworkBuffer {
 public:
  NetworkBuffer(uint8_t* buffer, size_t length)
      : buffer_(buffer), length_(length) {}

  template <typename T = uint8_t*>
  [[nodiscard]]
  T data() const {
    return reinterpret_cast<T>(buffer_);
  }

  [[nodiscard]]
  size_t length() const {
    return length_;
  }

 private:
  uint8_t* const buffer_;
  const size_t length_;
};

#endif
