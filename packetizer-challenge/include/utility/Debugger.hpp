#ifndef __DEBUGGER__
#define __DEBUGGER__

#include <Traits.hpp>
#include <iostream>

enum DebuggerLevel {
  PRINT,
  ERROR,
  TRACE,
};

template <DebuggerLevel Level>
class Debugger {
  using Manipulator = std::ostream& (*)(std::ostream&);

 public:
  Debugger() = default;

  template <typename T>
  Debugger& operator<<(const T& value) {
    if constexpr (Level == ERROR && Traits<Enabled>::Error) {
      std::cerr << value;
    } else if ((Level == TRACE && Traits<Enabled>::Trace) || Level == PRINT) {
      std::cout << value;
    }
    return *this;
  }

  Debugger& operator<<(Manipulator manipulator) {
    if constexpr (Level == ERROR && Traits<Enabled>::Error) {
      manipulator(std::cerr);
      exit(1);
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
