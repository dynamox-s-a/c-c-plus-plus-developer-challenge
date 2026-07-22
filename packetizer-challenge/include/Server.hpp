#ifndef __SERVER__
#define __SERVER__

#include <DynamoxProtocol.hpp>
#include <network/UnixNetworkDevice.hpp>
#include <utility/Debugger.hpp>

class Server {
 public:
  Server() {
    Debugger<TRACE>() << "Server() {" << Debugger<PRINT>::endl;

    UnixNetworkDevice communicator(Traits<UnixNetworkDevice>::ServerPort);
    DynamoxProtocol device(communicator);

    while (true) {
      NetworkBuffer* buffer = device.receive();

      if (!buffer) continue;

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
