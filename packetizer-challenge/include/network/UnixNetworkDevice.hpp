#ifndef __UNIX_NETWORK_DEVICE__
#define __UNIX_NETWORK_DEVICE__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <Traits.hpp>
#include <network/NetworkAddress.hpp>
#include <network/NetworkDevice.hpp>
#include <utility/Debugger.hpp>

class UnixNetworkDevice : public NetworkDevice {
  class UnixNetworkBuffer : public NetworkBuffer {
   public:
    UnixNetworkBuffer(unsigned char* data, size_t length, uint16_t port = 0)
        : NetworkBuffer(data, length), port_(port) {}

    NetworkAddress source() override {
      return NetworkAddress(reinterpret_cast<const char*>(&port_),
                            sizeof(port_));
    }

   private:
    uint16_t port_;
  };

 public:
  UnixNetworkDevice(NetworkAddress port)
      : port_(*reinterpret_cast<const uint16_t*>(port.data())) {
    Debugger<TRACE>() << "UnixNetworkDevice() {" << Debugger<TRACE>::endl;

    socket_ = socket(AF_INET, SOCK_DGRAM, 0);

    if (socket_ < 0) {
      Debugger<ERROR>() << "Can't Create UnixNetworkDevice Socket!"
                        << Debugger<ERROR>::endl;
      return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_, (sockaddr*)(&addr), sizeof(addr)) < 0) {
      Debugger<ERROR>() << "Can't Bind UnixNetworkDevice Socket!"
                        << Debugger<ERROR>::endl;
      close(socket_);
      socket_ = -1;
      return;
    }

    Debugger<TRACE>() << "}" << Debugger<TRACE>::endl;
  }

  ~UnixNetworkDevice() {
    Debugger<TRACE>() << "~UnixNetworkDevice() {" << Debugger<TRACE>::endl;
    if (socket_ >= 0) close(socket_);
    Debugger<TRACE>() << "}" << Debugger<TRACE>::endl;
  }

  size_t mtu() override { return 2048; }

  int send(NetworkBuffer* buffer, NetworkAddress address) override {
    if (socket_ < 0 || buffer == nullptr) return -1;
    Debugger<TRACE>() << "UnixNetworkDevice::send(" << buffer << ") {"
                      << Debugger<TRACE>::endl;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;

    uint16_t port = *reinterpret_cast<const uint16_t*>(address.data());
    addr.sin_port = htons(port);

    inet_pton(AF_INET, Traits<UnixNetworkDevice>::Address, &addr.sin_addr);

    int result = sendto(socket_, buffer->data(), buffer->capacity(), 0,
                        reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

    Debugger<TRACE>() << "}" << Debugger<TRACE>::endl;

    return result;
  }

  NetworkBuffer* alloc(size_t length) override {
    if (length > mtu()) {
      Debugger<ERROR>() << "Can't Alloc a NetworkBuffer Bigger Than The MTU! "
                        << length << Debugger<ERROR>::endl;
      return nullptr;
    }

    return new UnixNetworkBuffer(new unsigned char[length], length);
  }

  void free(NetworkBuffer* raw) override {
    if (!raw) return;
    UnixNetworkBuffer* buffer = static_cast<UnixNetworkBuffer*>(raw);
    delete[] buffer->start();
    delete buffer;
  }

  NetworkBuffer* receive(size_t timeout = ~0ULL) override {
    timeval tv{};
    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    auto* buffer = new unsigned char[mtu()];

    sockaddr_in addr{};
    socklen_t addrlen = sizeof(addr);

    int length = recvfrom(socket_, buffer, mtu(), 0,
                          reinterpret_cast<sockaddr*>(&addr), &addrlen);

    if (length < 0) {
      delete[] buffer;
      return nullptr;
    }

    return new UnixNetworkBuffer(buffer, length, ntohs(addr.sin_port));
  }

  void release(NetworkBuffer* buffer) override { free(buffer); }

 private:
  int socket_;
  int port_;
};

#endif
