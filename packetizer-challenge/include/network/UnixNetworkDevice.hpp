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

  constexpr size_t mtu() { return 2048; }

  int send(NetworkBuffer* buffer, NetworkAddress address) override {
    if (socket_ < 0 || buffer == nullptr) return -1;
    Debugger<TRACE>() << "UnixNetworkDevice::send(" << buffer << ") {"
                      << Debugger<TRACE>::endl;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;

    uint16_t port = *reinterpret_cast<const uint16_t*>(address.data());
    addr.sin_port = htons(port);

    inet_pton(AF_INET, Traits<UnixNetworkDevice>::Address, &addr.sin_addr);

    int result = sendto(socket_, buffer->data(), buffer->length(), 0,
                        reinterpret_cast<sockaddr*>(&addr), sizeof(addr));

    Debugger<TRACE>() << "}" << Debugger<TRACE>::endl;

    return result;
  }

  NetworkBuffer* alloc(size_t length) override {
    return new NetworkBuffer(new unsigned char[length], length);
  }

  void free(NetworkBuffer* buffer) override {
    delete[] buffer->data();
    delete buffer;
  }

  NetworkBuffer* receive() override {
    auto* buffer = new unsigned char[mtu()];

    sockaddr_in addr{};
    socklen_t addrlen = sizeof(addr);

    int length = recvfrom(socket_, buffer, mtu(), 0,
                          reinterpret_cast<sockaddr*>(&addr), &addrlen);

    if (length < 0) {
      delete[] buffer;
      return nullptr;
    }

    return new NetworkBuffer(buffer, length);
  }

  void release(NetworkBuffer* buffer) override {
    delete[] buffer->data();
    delete buffer;
  }

 private:
  int socket_;
  int port_;
};

#endif
