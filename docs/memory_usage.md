# Memory Usage Analysis

## Static Memory Allocation

### Operations Registry
- Operation structs: 4 × 32 bytes = 128 bytes
- Operation pointers array: 4 × 8 bytes = 32 bytes
- **Total**: ~160 bytes

### String Constants
- Operation names and descriptions: ~200 bytes
- Error messages: ~300 bytes
- **Total**: ~500 bytes

### Logger
- File pointer: 8 bytes
- Buffers (static): 0 bytes (uses stack)
- **Total**: ~8 bytes

**Total Static Memory**: ~670 bytes

---

## Stack Memory Usage (per operation)

### HMI Layer
- Input values buffer: 100 × 8 bytes = 800 bytes
- Local variables: ~50 bytes
- **Total**: ~850 bytes

### Operations
- Determinant: Inline variables = ~100 bytes
- Other operations: ~50 bytes each

**Maximum Stack per Call**: ~1 KB

---

## Heap Memory

**Zero heap allocation** - All memory is stack or static.

Benefits for embedded systems:
- No fragmentation
- Deterministic memory usage
- No malloc/free overhead
- Predictable behavior

---

## File System

### Log File
- CSV text format
- Approximate size per entry: 80-120 bytes
- 1000 operations ≈ 100 KB
- No automatic rotation (append only)

**Future improvement**: Add log rotation at configurable size limit.

---

## Log Storage Strategy

**Current**: Append-only CSV (no rotation)

**Rationale**: 
- Flash wear consideration (limited write cycles)
- Deterministic behavior (no file operations during runtime)
- Acceptable for device lifetime at expected usage

**Production recommendations**:
1. External storage (SD card) if rotation needed
2. Circular buffer in RAM with periodic flush
3. Size limit + disable logging when full
4. Remote logging via network

**Risk**: Log file grows unbounded
**Mitigation**: Monitor in production. Configure filesystem quotas or implement size checks to prevent unbounded growth.

---

## Total Memory Footprint

| Component | RAM Usage |
|-----------|-----------|
| Static    | ~670 B    |
| Stack     | ~1 KB     |
| Heap      | 0 B       |
| **Total** | **~1.7 KB** |

Suitable for MCUs with:
- RAM: 2+ KB
- Flash: 16+ KB (with standard library)