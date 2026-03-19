# Dynamox Arithmetic Calculator

A console-based arithmetic calculator with a Human-Machine Interface (HMI), developed for the Dynamox C/C++ Developer Challenge.

## Features

### Implemented Operations

| Operation          | Input Mode   | Description                                                                                            |
| ------------------ | ------------ | ------------------------------------------------------------------------------------------------------ |
| **Determinant**    | Matrix (NxN) | Computes the determinant using LU decomposition with partial pivoting. User selects matrix dimensions. |
| **Sum**            | Array        | Computes the sum of all elements in an array.                                                          |
| **Mean / Average** | Array        | Computes the arithmetic mean of an array.                                                              |
| **Minimum**        | Array        | Finds the minimum value in an array.                                                                   |
| **Maximum**        | Array        | Finds the maximum value in an array.                                                                   |
| **Factorial**      | Single Value | Computes n! for a non-negative integer.                                                                |

### Bonus Features

- **Extensible operation registry** — New operations can be added by registering a function pointer, without modifying existing code.
- **Persistent operation log** — Every operation is logged to `operations.log` with timestamp, inputs, and result. Users can view past entries from the menu.
- **Configurable matrix dimensions** — Determinant supports any square matrix size (1x1 up to 20x20).
- **Comprehensive error handling** — Invalid inputs, division by zero, non-square matrices, memory allocation failures, and overflow are all detected and reported gracefully.

## Project Structure

```
.
├── main.c                      # Application entry point
├── Makefile                    # Build system
├── README.md                   # This file
└── lib/
    ├── matrix/
    │   ├── matrix.h            # Matrix operations API
    │   └── matrix.c            # Matrix implementation (alloc, determinant, etc.)
    ├── operations/
    │   ├── operations.h        # Arithmetic operations API
    │   └── operations.c        # Sum, mean, min, max, factorial
    ├── registry/
    │   ├── registry.h          # Operation registry API
    │   └── registry.c          # Dynamic operation registration
    ├── logger/
    │   ├── logger.h            # Persistent log API
    │   └── logger.c            # File-based operation logging
    └── hmi/
        ├── hmi.h               # HMI API
        └── hmi.c               # Console menu and user interaction
```

## Building and Running

### Prerequisites

- **C Compiler** (GCC, MinGW, or Visual Studio)
- **Math library** support

**⚠️ No C compiler installed?** See [`INSTALL.md`](INSTALL.md) for detailed setup instructions.

### Quick Build (Windows)

```cmd
.\build.bat
```

### Quick Build (Linux/macOS)

```bash
make
```

### Run

```cmd
.\calculator.exe    # Windows
./calculator        # Linux/macOS
```

### Manual Compilation

```bash
# GCC/MinGW
gcc -Wall -Wextra -Wpedantic -std=c11 -O2 -o calculator main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c -lm

# Visual Studio (Windows)
cl /W4 /O2 /Fe:calculator.exe main.c lib/matrix/matrix.c lib/operations/operations.c lib/registry/registry.c lib/logger/logger.c lib/hmi/hmi.c
```

### Test the Code

```bash
# Compile and run unit tests
gcc -o test_simple test_simple.c lib/operations/operations.c lib/matrix/matrix.c -lm
./test_simple
```

## Usage Example

```
  ╔══════════════════════════════════════╗
  ║      ARITHMETIC CALCULATOR HMI       ║
  ╠══════════════════════════════════════╣
  ║  [1] Determinant (Matrix)            ║
  ║  [2] Sum (Array)                     ║
  ║  [3] Mean / Average (Array)          ║
  ║  [4] Minimum (Array)                 ║
  ║  [5] Maximum (Array)                 ║
  ║  [6] Factorial (Single Value)        ║
  ╠══════════════════════════════════════╣
  ║  [7] View Operation Log              ║
  ║  [0] Exit                            ║
  ╚══════════════════════════════════════╝

  Select an option: 1

  Enter number of rows (1-20): 3
  Enter number of columns (1-20): 3
  Enter matrix elements:
    [1][1]: 1
    [1][2]: 2
    [1][3]: 3
    [2][1]: 4
    [2][2]: 5
    [2][3]: 6
    [3][1]: 7
    [3][2]: 8
    [3][3]: 9

  ┌─────────────────────────────────┐
  │  Result: 0                      │
  └─────────────────────────────────┘
```

## Adding New Operations

To add a new operation, register it in `main.c` inside `register_builtin_operations()`:

```c
/* Example: register a custom "Product" operation */
static op_status_t op_product(const double *values, size_t count, double *result)
{
    *result = 1.0;
    for (size_t i = 0; i < count; i++) {
        *result *= values[i];
    }
    return OP_OK;
}

/* In register_builtin_operations(): */
registry_add_array_op("Product (Array)", INPUT_ARRAY, (op_array_fn)op_product);
```

The operation will automatically appear in the HMI menu.

## Design Decisions

- **Contiguous matrix allocation** — Matrix data is stored in a single `calloc` block for cache-friendly access and simplified deallocation, important in resource-constrained embedded systems.
- **LU decomposition with partial pivoting** — Chosen over cofactor expansion for O(n³) vs O(n!) complexity, making it practical for larger matrices.
- **Operation registry pattern** — Follows the Open/Closed Principle: new operations can be added without modifying existing code.
- **Defensive error handling** — All functions validate inputs and return typed error codes rather than crashing.
- **C11 standard** — Modern C with strict warnings for firmware-quality code.
