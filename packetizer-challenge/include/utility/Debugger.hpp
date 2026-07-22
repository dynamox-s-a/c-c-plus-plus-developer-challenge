#ifndef __DEBUGGER__
#define __DEBUGGER__

#include <Traits.hpp>
#include <iostream>

enum DebuggerLevel {
  PRINT,
  ERROR,
  WARNING,
  TRACE,
};

template <DebuggerLevel Level>
class Debugger {
  static constexpr const char* RED = "\033[31m";
  static constexpr const char* ORANGE = "\033[38;5;208m";
  static constexpr const char* RESET = "\033[0m";

  using Manipulator = std::ostream& (*)(std::ostream&);

 public:
  Debugger() = default;

  template <typename T>
  Debugger& operator<<(const T& value) {
    if constexpr (Level == ERROR && Traits<Enabled>::Error) {
      std::cerr << RED << value << RESET;
    } else if (Level == WARNING && Traits<Enabled>::Warning) {
      std::cout << ORANGE << value << RESET;
    } else if ((Level == TRACE && Traits<Enabled>::Trace) || Level == PRINT) {
      std::cout << value;
    }
    return *this;
  }

  Debugger& operator<<(Manipulator manipulator) {
    if constexpr (Level == ERROR && Traits<Enabled>::Error) {
      manipulator(std::cerr);
      exit(1);
    } else if (Level == WARNING && Traits<Enabled>::Warning) {
      manipulator(std::cout);
    } else if ((Level == TRACE && Traits<Enabled>::Trace) || Level == PRINT) {
      manipulator(std::cout);
    }
    return *this;
  }

  static std::ostream& endl(std::ostream& os) { return std::endl(os); }
  static std::ostream& hex(std::ostream& os) {
    os << std::hex;
    return os;
  }
};

#endif
