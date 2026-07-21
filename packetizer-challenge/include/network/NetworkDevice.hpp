#ifndef __NETWORK_DEVICE__
#define __NETWORK_DEVICE__

#include <network/NetworkAddress.hpp>
#include <network/NetworkBuffer.hpp>

class NetworkDevice {
 public:
  virtual ~NetworkDevice() = default;
  virtual NetworkBuffer* alloc(size_t) = 0;
  virtual int send(NetworkBuffer*, NetworkAddress) = 0;
  virtual void free(NetworkBuffer*) = 0;
  virtual size_t mtu() = 0;
  virtual NetworkBuffer* receive(size_t) = 0;
  virtual void release(NetworkBuffer*) = 0;
};

#endif
