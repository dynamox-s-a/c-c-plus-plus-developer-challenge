# Dynamox C/C++ Calculator

Embedded-friendly calculation system with extensible operations and terminal interface.

## Requirements

- CMake 3.10+
- GCC (with C99 support)
- Linux/WSL/Ubuntu

## How to Build and Run
```bash
mkdir build
cd build
cmake ..
make
./dynamox_calculator
```

## Features

✅ Interactive terminal interface  
✅ Array operations: addition, mean and multiply  
✅ Determinant calculation 2x2 and 3x3 matrices with direct formula
✅ Implemented: Determinant calculation 4x4, 5x5, 6x6 using LU with partial pivoting (Need Improvements) 
✅ Persistent CSV logging  
✅ Easy to add new operations  

## Project Structure
```
src/
├── main.c                    # Entry point
├── core/                     # Core logic
├── hmi/                      # User interface
└── operations/               # Mathematical operations

docs/                         # Technical documentation
```

## Usage

After running, follow the interactive menu:

1. Select desired operation
2. Specify single value or array
3. Enter values
4. View result

Operation log is saved in `logs/operations.csv`

## Additional Documentation

- **ARCHITECTURE.md** - Design decisions and trade-offs
- **FUTURE_IMPROVEMENTS.md** - Planned enhancements
- **docs/adding_operations.md** - How to add new operations

## Input Limitations

### Buffer Sizes (Embedded System Constraints)

| Limit | Value | Description |
|-------|-------|--------|
| **Max input line** | 256 characters | Terminal buffer size |
| **Max array size** | 100 values | Maximum array elements |
Defined in input_validator.h

### Tips for Large Arrays

1. **Use scientific notation**: `1e6` instead of `1000000`
2. **Split operations**: Process in batches
3. **Use comma-separated**: More compact than spaces

## Author

Vitor Hirozawa - Dynamox C/C++ Developer Challenge