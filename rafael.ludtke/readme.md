# Dynamox C/C++ Developer Challenge – Rafael Ludtke

## Overview

This repository contains my implementation for the Dynamox C/C++ Developer Challenge. The goal was to develop an application that performs configurable arithmetic operations and provides a Human Machine Interface (HMI) for user interaction.

## Implemented Features

- **HMI Console:** Users interact with the application via a terminal-based interface.
- **Operation Selection:** Users can select the type of operation to perform.
- **Single Value Input:** Supports operations on single values.
- **Array Input:** Supports operations on arrays of values where applicable.
- **Multiple Operations:** At least two operations are available.
- **Determinant Calculation:** Includes calculation of the matrix determinant (currently supports up to 2x2 matrices).
- **Modular Code Structure:** Code is organized into libraries for maintainability.
- **Persistent Logging:** Operations are logged to `operations.log`.
- **Error Handling:** User input is validated.
- **Matrix Dimension Selection:** Users can select matrix dimensions for determinant calculation (currently up to 2x2). Transpose matrix calculation has no limit (planned feature).

## Technical Details

- **Language:** C++
- **Entry Point:** [main.cpp](main.cpp)
- **Build System:** CMake ([CMakeLists.txt](CMakeLists.txt))
- **Libraries:** All major functionalities are separated into header/source pairs under `include/` and `src/`.

## How to Build and Run

1. Clone the repository.
2. Navigate to the `rafael.ludtke` directory.
3. Build using CMake:
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
4. Run the executable:
    ```sh
    ./dynamox
    ```

## Future Improvements

- Add support for creating new operation types via configuration.
- Add support for Laplace expansion for calculating determinants of larger matrices.
- Limit matrix input size to prevent hardware overload.

## Notes

- The HMI is currently implemented as a console interface for simplicity.
- It can be simply extended to support a different hardware by the HMI interface.

---
