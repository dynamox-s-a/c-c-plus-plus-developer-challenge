#include <Client.hpp>
#include <Server.hpp>

int main(int argc, char* argv[]) {
  if (argc < 2) {
    Debugger<PRINT>() << "Usage: " << argv[0] << " <SERVER|CLIENT>"
                      << Debugger<PRINT>::endl;
    return 1;
  }

  srand(time(NULL));

  if (*argv[1] == 'S') {
    Server _;
  } else if (*argv[1] == 'C') {
    Client client(argc, argv);
  } else {
    Debugger<ERROR>() << "Invalid Input!" << Debugger<ERROR>::endl;
  }
}
