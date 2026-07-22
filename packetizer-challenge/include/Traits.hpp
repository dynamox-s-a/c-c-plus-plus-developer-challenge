#ifndef __TRAITS__
#define __TRAITS__

class UnixNetworkDevice;
class UnsafeUnixNetworkDevice;
class DynamoxProtocol;
class Client;
class Server;
class Enabled;

template <typename T>
struct Traits;

template <>
struct Traits<UnixNetworkDevice> {
  static constexpr const char* Localhost = "127.0.0.1";
};

template <>
struct Traits<UnsafeUnixNetworkDevice> : Traits<UnixNetworkDevice> {
  static constexpr int DropProbability = 10;
  static constexpr int CorruptionProbability = 5;
};

template <>
struct Traits<Client> {
  static constexpr unsigned char Address[2] = {0xF, 0xA};
};

template <>
struct Traits<Server> {
  static constexpr unsigned char Address[2] = {0xF, 0xB};
};

template <>
struct Traits<Enabled> {
  static constexpr bool Trace = true;
  static constexpr bool Error = true;
  static constexpr bool Warning = true;
};

template <>
struct Traits<DynamoxProtocol> {
  static constexpr int DefaultTimeout = 100;
  static constexpr int DefaultRetries = 5;
  using Network = UnsafeUnixNetworkDevice;
};

#endif
