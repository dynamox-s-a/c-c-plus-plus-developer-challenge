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

### 2. Maximum matrix size: 6x6

**Decision**: `#define MAX_MATRIX_SIZE 6  
**Rationale**:
- Stack allocation: 6×6×8 bytes = 288 bytes (acceptable)
- Determinant has O(n³) complexity - 6x6 is heavy
- Why not 10x10?
    - Input buffer is 256 chars
    - 10x10 = 100 values × ~5 chars = 500 chars (exceeds buffer)
    - 6x6 = 36 values × ~5 chars = 180 chars (safe margin)
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

### 4. Determinant: 2x2 to 6x6

**Decision**: Direct implementation for 2x2/3x3 (non-recursive), LU decomposition for 4x4/5x5/6x6  
**Rationale**:
- 2x2: Direct formula (2 multiplications)
- 3x3: Sarrus rule (simple and efficient)
- 2x2, 3x3: Direct formulas are faster (O(1) vs O(n³))
- 4x4+: LU decomposition provides better numerical stability
- 6x6 limit: Input buffer constraint (256 chars ≈ 36-49 values max)

**LU Decomposition details**:
- Algorithm: Gaussian elimination with partial pivoting
- Complexity: O(n³)
- Stability: Pivoting prevents division by small numbers

**Future improvement**: LU decomposition needs improvements

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