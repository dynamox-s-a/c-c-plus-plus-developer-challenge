#ifndef __CLIENT__
#define __CLIENT__

#include <stdio.h>
#include <unistd.h>

#include <DynamoxProtocol.hpp>
#include <network/UnixNetworkDevice.hpp>
#include <utility/Debugger.hpp>

class Client {
 public:
  Client(int, char* argv[]) {
    Debugger<TRACE>() << "Client() {" << Debugger<PRINT>::endl;

    UnixNetworkDevice communicator(Traits<UnixNetworkDevice>::ClientPort);
    DynamoxProtocol device(communicator);

    Debugger<TRACE>() << "Reading Input From <STDIN>..."
                      << Debugger<TRACE>::endl;

    if (isatty(STDIN_FILENO)) {
      Debugger<ERROR>() << "Usage: " << "<INPUT> | " << argv[0] << " CLIENT"
                        << Debugger<ERROR>::endl;
      return;
    }

    void* data;
    int result = read(&data);

    if (result < 0) {
      Debugger<ERROR>() << "Failed To Read Input From <STDIN>"
                        << Debugger<ERROR>::endl;
    }

    Debugger<TRACE>() << "Successfully Read " << result
                      << " Bytes From <STDIN>!" << Debugger<TRACE>::endl;

    NetworkBuffer* buffer = device.alloc(result);

    if (!buffer) {
      Debugger<ERROR>() << "Failed To Allocate NIC Buffer!"
                        << Debugger<ERROR>::endl;
    }

    buffer->fill(data, result);

    result = device.send(buffer,
                         NetworkAddress(Traits<UnixNetworkDevice>::ServerPort));

    device.free(buffer);

    if (result < 0) {
      Debugger<ERROR>() << "Send Failed!" << Debugger<ERROR>::endl;
    } else {
      Debugger<PRINT>() << "Sent: " << result;
      Debugger<PRINT>() << " Bytes!" << Debugger<PRINT>::endl;
    }

    Debugger<TRACE>() << "}" << Debugger<PRINT>::endl;
  }

  ~Client() {
    Debugger<TRACE>() << "~Client() {" << Debugger<TRACE>::endl
                      << "}" << Debugger<TRACE>::endl;
  }

  static int read(void** destination) {
    size_t capacity = 4096;
    size_t size = 0;

    char* buffer = (char*)malloc(capacity);

    if (!buffer) {
      perror("malloc");
      return -1;
    }

    for (;;) {
      if (size == capacity) {
        capacity *= 2;
        char* temporary = (char*)realloc(buffer, capacity);
        if (!temporary) {
          free(buffer);
          perror("realloc");
          return -1;
        }
        buffer = temporary;
      }
      int n = ::read(STDIN_FILENO, buffer + size, capacity - size);

      if (n < 0) {
        perror("read");
        free(buffer);
        return -1;
      }

      if (n == 0) break;

      size += n;
    }
    *destination = buffer;
    return size;
  }
};

#endif
