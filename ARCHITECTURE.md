# Architecture Decisions

## Design Principles

1. **Simplicity** - Pure C, structs and function pointers
2. **Static Allocation** - No malloc/free (embedded best practices)
3. **Modularity** - Add operations without modifying existing code
4. **Readability** - Any developer can maintain the code

## Technical Decisions

### 1. Why pure C instead of C++?

**Decision**: Use C structs with function pointers  
**Rationale**: 
- Zero runtime overhead
- Simpler code to understand
- Suitable for resource-constrained embedded systems
- Easier maintenance for developers of any level

**Alternative considered**: C++ classes with inheritance  
**Trade-off**: Less type-safety, but greater clarity and performance

---

### 2. Maximum matrix size: 10x10

**Decision**: `#define MAX_MATRIX_SIZE 10`  
**Rationale**:
- Stack allocation: 10×10×8 bytes = 800 bytes (acceptable)
- Determinant has O(n³) complexity - 10x10 is already heavy
- Avoids dynamic allocation (memory fragmentation)

**For production**: Evaluate actual needs and available MCU resources

---

### 3. CSV format for logging (text)

**Decision**: Text-based CSV file  
**Rationale**:
- Easy debugging and validation
- Evaluator can inspect directly
- Portable across systems

**Trade-off**: 2-3x larger than binary  
**Future improvement**: Compact binary format for production

---

### 4. Determinant: Only 2x2 and 3x3

**Decision**: Direct implementation (non-recursive)  
**Rationale**:
- 2x2: Direct formula (2 multiplications)
- 3x3: Sarrus rule (simple and efficient)
- NxN: Requires LU decomposition (higher complexity)

**Future improvement**: Implement LU decomposition for NxN

---

## Assumptions (Handling Ambiguity)

### Interface (HMI)
- **Chosen**: Interactive terminal
- **Matrix input**: Space or comma-separated values
- **Format**: Ask for dimensions, then read values

### Input Validation
- Detects non-numeric input
- Validates matrix dimensions (square)
- Enforces maximum size limit

### Persistence
- **File**: `logs/operations.csv`
- **Format**: timestamp,operation,input,result,status
- **Rotation**: Not implemented (infinite append)
- **Future improvement**: Size limit with rotation

---

## Resource Usage

### Stack Memory (worst case)
- Matrix buffer: 800 bytes (double[10][10])
- Input buffer: 256 bytes
- Local variables: ~100 bytes
- **Total**: ~1.2 KB

### Static Memory
- Operations array: 48 bytes (4 pointers × 12 bytes/struct)
- Constant strings: ~200 bytes
- **Total**: ~250 bytes

### Heap
- **Zero allocation** - All memory is stack or static

---

## Extensibility

New operations can be added by:
1. Creating a new file in `src/operations/`
2. Implementing the `Operation` struct
3. Adding reference in `operation_manager.c`

See `docs/adding_operations.md` for detailed instructions.

---

## Questions for Product Owner

If this were a real production system, I would clarify:

1. **Target hardware specifications?** (RAM/Flash constraints)
2. **Real-time requirements?** (maximum operation latency)
3. **Log retention policy?** (rotate, archive, or delete)
4. **Maximum matrix dimensions needed?** (affects memory budget)
5. **Floating-point precision required?** (float vs double)