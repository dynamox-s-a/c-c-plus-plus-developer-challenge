#ifndef __SERVER__
#define __SERVER__

#include <DynamoxProtocol.hpp>
#include <network/UnixNetworkDevice.hpp>
#include <network/UnsafeUnixNetworkDevice.hpp>
#include <utility/Debugger.hpp>

class Server {
 public:
  Server() {
    Debugger<TRACE>() << "Server() {" << Debugger<PRINT>::endl;

    Traits<DynamoxProtocol>::Network communicator(Traits<Server>::Address);
    DynamoxProtocol device(communicator);

    while (true) {
      NetworkBuffer* buffer = device.receive();

      if (!buffer) continue;

      Debugger<PRINT>() << Debugger<PRINT>::dec;
      Debugger<PRINT>() << "Received: " << buffer->length();
      Debugger<PRINT>() << " Bytes!" << Debugger<PRINT>::endl;

      for (size_t i = 0; i < buffer->length(); i++) {
        Debugger<PRINT>() << Debugger<PRINT>::hex << (*buffer)[i];
      }

      Debugger<PRINT>() << Debugger<PRINT>::endl;

      device.release(buffer);
    }

    Debugger<TRACE>() << "}" << Debugger<PRINT>::endl;
  }
};

#endif
