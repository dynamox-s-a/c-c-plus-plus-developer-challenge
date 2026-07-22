#ifndef __UNSAFE_UNIX_NETWORK_DEVICE__
#define __UNSAFE_UNIX_NETWORK_DEVICE__

#include <network/UnixNetworkDevice.hpp>

class UnsafeUnixNetworkDevice : public UnixNetworkDevice {
  using UnixNetworkDevice::UnixNetworkDevice;

 public:
  int send(NetworkBuffer* buffer, const NetworkAddress& destination) override {
    if (rand() % 100 < Traits<UnsafeUnixNetworkDevice>::DropProbability) {
      Debugger<WARNING>() << "UnsafeUnixNetworkDevice Drop a Buffer!"
                          << Debugger<WARNING>::endl;
      return -1;
    }

    if (rand() % 100 < Traits<UnsafeUnixNetworkDevice>::CorruptionProbability) {
      Debugger<WARNING>() << "UnsafeUnixNetworkDevice Corrupted a Buffer!"
                          << Debugger<WARNING>::endl;
      size_t offset = rand() % buffer->length();
      buffer->data()[offset] = rand();
    }

    return UnixNetworkDevice::send(buffer, destination);
  }
};

#endif
