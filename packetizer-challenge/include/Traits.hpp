#ifndef __TRAITS__
#define __TRAITS__

class UnixNetworkDevice;
class Enabled;

template <typename T>
struct Traits;

template <>
struct Traits<UnixNetworkDevice> {
  static constexpr const char* Address = "127.0.0.1";
  static constexpr char ClientPort[2] = {0xF, 0xA};
  static constexpr char ServerPort[2] = {0xF, 0xB};
};

template <>
struct Traits<Enabled> {
  static constexpr bool Trace = true;
  static constexpr bool Error = true;
};

#endif
