#include <DynamoxProtocol.hpp>
#include <network/UnixNetworkDevice.hpp>
#include <utility/Debugger.hpp>

class Client {
 public:
  Client() {
    Debugger<TRACE>() << "Client() {" << Debugger<PRINT>::endl;

    UnixNetworkDevice communicator(Traits<UnixNetworkDevice>::ClientPort);
    DynamoxProtocol device(communicator);

    while (true) {
      const char* message = "Hello World!";
      size_t length = strlen(message);
      NetworkBuffer* buffer = device.alloc(4096);
      memcpy(buffer->data(), message, length);
      int result = device.send(
          buffer, NetworkAddress(Traits<UnixNetworkDevice>::ServerPort));
      device.free(buffer);

      if (result < 0) {
        Debugger<ERROR>() << "Send Failed!" << Debugger<ERROR>::endl;
      } else {
        Debugger<PRINT>() << "Sent: " << result;
        Debugger<PRINT>() << " Bytes!" << Debugger<PRINT>::endl;
      }
    }

    Debugger<TRACE>() << "}" << Debugger<PRINT>::endl;
  }
};

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

      device.release(buffer);
    }

    Debugger<TRACE>() << "}" << Debugger<PRINT>::endl;
  }
};

int main(int argc, char* argv[]) {
  if (argc < 2) {
    Debugger<PRINT>() << "Usage: " << argv[0] << " <SERVER|CLIENT>"
                      << Debugger<PRINT>::endl;
    return 1;
  }

  if (*argv[1] == 'S') {
    Server _;
  } else if (*argv[1] == 'C') {
    Client _;
  } else {
    Debugger<ERROR>() << "Invalid Input!" << Debugger<ERROR>::endl;
  }
}
