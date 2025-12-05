# Future Improvements

## Short-term (1-2 sprints)

### High Priority
- [ ] **LU Decomposition for NxN determinants**
  - Current: 2x2 and 3x3 (direct formulas) and 4x4 to 6x6 (LU Decomposition that need improvements)
  - Implemented: 4x4, 5x5, 6x6 using LU with partial pivoting
  - Limit: 6x6 due to input buffer constraint (256 chars)
  - Future: Increase input buffer to support 10x10 if needed
  - Improvement: Still need improvements in LU decomposition. Support up to 10x10 using LU decomposition
  - Complexity: O(n³) but more numerically stable

- [ ] **Binary log format option**
  - Current: CSV text (human-readable)
  - Improvement: Binary format for production
  - Benefit: 60-70% size reduction, faster writes

- [ ] **Enhanced input validation**
  - Current: Basic validation
  - Improvement: Range checks, overflow detection
  - Add: Configurable precision thresholds
    * Allow users to set epsilon for floating-point comparisons
    * Configurable singularity threshold for determinant
    * Min/max value limits to prevent overflow
    * Useful for different use cases (scientific, financial, etc.)

### Medium Priority
- [ ] **Unit test suite**
  - Framework: Native C or Google Test
  - Coverage: All operations + edge cases
  - CI/CD: Automated testing on commits

- [ ] **Log rotation**
  - Current: Infinite append
  - Improvement: Max size with rotation
  - Configuration: Max entries or file size

---

## Medium-term (1-2 months)

### Plugin System
- [ ] **Dynamic operation loading**
  - Load operations from shared libraries (.so/.dll)
  - Hot-reload without recompilation
  - API versioning for compatibility

### Expression Parser
- [ ] **User-defined operations**
  - Parse mathematical expressions
  - Example: "2*x^2 + 3*x + 1"
  - Library options: TinyExpr, MathParser

### Configuration System
- [ ] **Operation parameters from config file**
  - JSON or INI format
  - Runtime configuration without recompile
  - Default values with override capability

---

## Long-term (3+ months)

### Performance Optimization
- [ ] **SIMD for array operations**
  - Use SSE/AVX for vector operations
  - Platform-specific implementations
  - Target: Addition, Mean, Multiply operations
  - Technology: SSE2 (x86) / NEON (ARM)
  - Expected speedup: 2-4x for arrays > 100 elements
  - Implementation: ~50 lines per operation
  - Trade-off: Complexity vs performance gain
  - Recommendation: Implement only if profiling shows bottleneck

- [ ] **Memory pool allocator**
  - Pre-allocated pool for matrix operations
  - Zero fragmentation
  - Deterministic allocation time

### Advanced Features
- [ ] **Real-time constraints**
  - WCET (Worst Case Execution Time) analysis
  - Preemptive operation cancellation
  - Deadline monitoring

- [ ] **Matrix operation library**
  - Multiplication, inversion, eigenvalues
  - QR decomposition
  - Singular Value Decomposition (SVD)

### Embedded-specific
- [ ] **Fixed-point arithmetic option**
  - For MCUs without FPU
  - Configurable precision
  - Performance benchmarks vs floating-point

- [ ] **Power consumption optimization**
  - Sleep modes between operations
  - DMA for large data transfers
  - Clock scaling based on workload

---

## Architecture Enhancements

### Considered but not implemented
- [ ] **Multi-threading support**
  - Parallel operation execution
  - Thread-safe logging
  - Trade-off: Complexity vs performance gain

- [ ] **Network interface**
  - TCP/IP for remote operations
  - REST API wrapper
  - Useful for: Distributed embedded systems

- [ ] **Graphical interface**
  - Desktop: Cross-platform GUI (GTK/Qt)
  - Embedded: LVGL (Light and Versatile Graphics Library)
    * Optimized for resource-constrained systems
    * ~64KB RAM, ~100KB Flash
    * Touch screen support
    * Used in IoT devices, industrial HMIs
  - Real-time plotting
  - Trade-off: Resource overhead vs usability
  Production recommendation: 
    - Field device with LCD display → LVGL
    - Remote monitoring → Web interface
    - Diagnostic tool → Qt (desktop)

---

## Documentation
- [ ] Doxygen API documentation
- [ ] Performance benchmarks
- [ ] Memory profiling reports
  - Tools: Valgrind (runtime), gcc -fstack-usage (compile-time)
  - Generate reports showing:
    * Stack usage per function
    * Static memory layout
    * Peak memory consumption
    * Verification of zero heap usage
  - Purpose: Validate suitability for target embedded platform
  - Deliverable: memory_profile.md with resource budget analysis
- [ ] Porting guide for different MCUs