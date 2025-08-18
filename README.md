# Dynamox C/C++ Developer Challenge

## Author
**Sergio Henrique Oliveira Moraes Oliveira**  
Date: 2025-08-17

## Project Description
This project is a C/C++ console application developed for the Dynamox C/C++ Developer Challenge.  
It implements a simple task scheduler, a plugin-style operations system, and a user interface to perform various operations such as:

- Addition of two numbers
- Average of two numbers
- Mode of an array
- Determinant of a square matrix

The program also includes:

- Input validation
- Logging of operations (with timestamp)
- Modular architecture with task scheduling
- Future-proof design for adding more operations

## Features

- Menu-driven UI in the console
- Operations implemented as plugins for future extensibility
- Logging to `operations.log` with operation name, result, and timestamp
- Support for arrays and matrices with input validation
- Continuous task scheduler to manage UI, operations, and logging tasks

## Requirements

- C compiler (GCC, Clang, or Visual Studio)
- Standard C libraries (`stdio.h`, `stdlib.h`, `string.h`, `time.h`)

## How to Compile and Run the Project

### On Linux

1. Open a terminal and navigate to the project folder.
2. Compile all `.c` files using `gcc`:

```bash
gcc -o challenge main.c operations.c scheduler.c tasks.c log.c ui.c
```

3. Run the executable:

```bash
./challenge
```

### On Windows (Using MinGW)

1. Open the Command Prompt and navigate to the project folder.
2. Compile all `.c` files using `gcc`:

```bash
gcc -o challenge.exe main.c operations.c scheduler.c tasks.c log.c ui.c
```

3. Run the executable:

```bash
challenge.exe
```

## Notes
- Ensure that all source files (.c) and headers (.h) are in the same directory or adjust include paths accordingly.
- The program will display a menu for operations, ask for input, and log results in `operations.log`.

## How to Add a New Operation

The application uses a plugin-style system for operations. To add a new operation, follow these steps:

1. **Declare the function** in `operations.c` with the signature:
   ```c
   int my_new_operation(int a, int b);
   ```

2. **Add the function prototype in `operations.h`:**
   ```c
   int my_new_operation(int a, int b);
   ```

3. **Create an `Operation` struct** in your main or initialization code:
   ```c
   Operation op_new = {"My Operation", my_new_operation};
   ```

4. **Register the operation** using the registration function:
   ```c
   register_operation(op_new);
   ```

5. **Update `task_operations` in `tasks.c`** if the operation requires special input handling or non-standard arguments.

By following this pattern, your new operation will appear in the menu and integrate with the scheduler, UI, and logging system.

---

## Improvements 

To make this project suitable for **professional use in embedded systems**, the following improvements are recommended:

1. **Non-blocking UI**  
   - Replace `scanf` with an asynchronous state machine (FSM) that processes user input byte by byte.  
   - Prevents blocking the scheduler and keeps tasks responsive.

2. **Scheduler with Timing**  
   - Extend the cooperative scheduler to support **periodic tasks** using system ticks (e.g., `hal_millis()`).  
   - Avoids busy-wait loops and reduces CPU usage.

3. **Message Queues Between Tasks**  
   - Replace global shared variables with **static message queues (ring buffers)** for passing data between UI, Operations, and Logging tasks.  
   - Makes the system more modular and avoids race conditions.

4. **Unified Operation Plugins**  
   - Current design mixes binary ops (add, avg) with direct calls (mode, determinant).  
   - Create a unified plugin system supporting **binary, vector, and matrix operations** through dedicated function pointers.

5. **Determinant & Mode Robustness**  
   - Determinant: keep calculations in `double` but check for overflow before returning an `int`.  
   - Mode: optimize with sorting or counting, ensuring predictable runtime.


6. **Hardware Abstraction Layer (HAL)**  
   - Create a minimal HAL (`hal_init`, `hal_millis`, `hal_uart_read`, `hal_uart_write`) for portability.  
   - On PC → HAL uses `stdio`; on embedded → HAL maps to UART/timer drivers.

7. **Memory Safety**  
   - Avoid `malloc` and dynamic allocations.  
   - Use static buffers with configurable sizes (`#define MAX_ARRAY_SIZE`, `MAX_MATRIX_SIZE`, etc.).

8. **Compiler Warnings & Analysis**  
   - Build with strict flags: `-Wall -Wextra -Werror -Wconversion -Wshadow`.  
   - Run static analysis tools (`clang-tidy`, `cppcheck`) to catch hidden issues.  
   - Move towards **MISRA C** compliance for embedded reliability.

9. **Unit Testing**  
    - Create host-side tests for `operations.c` (add, avg, mode, determinant).  
    - Use `assert` or lightweight test framework to validate correctness.

---


