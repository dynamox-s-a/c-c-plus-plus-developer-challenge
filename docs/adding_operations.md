# Adding New Operations

## Overview

The system uses a simple `Operation` struct interface. Any operation just needs to:
1. Implement an execution function
2. Define an `Operation` struct
3. Register in the operation manager

## Step-by-Step Guide

### 1. Create operation file

Create `src/operations/your_operation.c`:
```c
#include "operation.h"
#include <stdio.h>

// Implementation function
static int your_operation_execute(const double* values, int count, double* result) {
    // Validate input
    if (values == NULL || result == NULL || count <= 0) {
        fprintf(stderr, "ERROR: Invalid input\n");
        return -1;
    }
    
    // Your logic here
    *result = values[0];  // Example
    
    return 0;  // Success
}

// Export the operation
Operation operation_your = {
    .name = "your_op",
    .description = "Description of your operation",
    .supports_array = 1,  // 1 = yes, 0 = no
    .execute = your_operation_execute
};
```

### 2. Create header file

Create `include/operations/your_operation.h`:
```c
#ifndef YOUR_OPERATION_H
#define YOUR_OPERATION_H

#include "operation.h"

extern Operation operation_your;

#endif
```

### 3. Register in manager

Edit `src/core/operation_manager.c`:
```c
#include "operations/your_operation.h"  // Add include

static Operation* available_operations[] = {
    &operation_add,
    &operation_mean,
    &operation_determinant,
    &operation_your  // Add here
};
```

### 4. Update CMakeLists.txt

Add to `OPERATIONS_SOURCES`:
```cmake
set(OPERATIONS_SOURCES
    src/operations/addition.c
    src/operations/mean.c
    src/operations/determinant.c
    src/operations/your_operation.c  # Add here
)
```

### 5. Rebuild
```bash
cd build
make
./dynamox_calculator
```

Your operation is now available in the menu!

## Example: Multiplication Operation

See `src/operations/multiply.c` for a complete working example.