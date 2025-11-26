# BigNumber Implementation Roadmap

## Current Status
- ✅ Long division implemented (O(n²))
- ✅ All 536 tests passing
- ✅ Architecture planned and documented

## Implementation Phases

### Phase 1: Unification with BigNumberCore Orchestrator + BigNumber Wrapper
**Goal**: Extract adaptive BigNumberCore with std::variant + create BigNumber wrapper

**Tasks:**
1. Create `IBigNumber.h` - Base interface
2. Create `BigNumberCore.h` - Orchestrator with std::variant (implementation selection)
3. Create `BigNumberCore.cpp` - Factory (all #ifdefs here)
4. Create `BigNumber.h` - User-facing wrapper with type variant
5. Create `BigNumber.cpp` - Type promotion/demotion logic
6. Create `BigNumberNaive.h` - Current CPU implementation
7. Update `BigCardinal` to inherit from `BigNumberCore`
8. Test with existing code

**Files:**
- `lib/IBigNumber.h` (new)
- `lib/BigNumberCore.h` (new)
- `lib/BigNumberCore.cpp` (new)
- `lib/BigNumberCore.t.cc` (new - core tests)
- `lib/BigNumber.h` (new - user-facing wrapper)
- `lib/BigNumber.cpp` (new - type logic)
- `lib/BigNumber.t.cc` (new - BigNumber tests)
- `lib/BigNumberNaive.h` (new - extract from BigCardinal)
- `lib/BigCardinal.h` (modify - inherit BigNumberCore)

**Estimated**: 2-3 days

---

### Phase 2: Karatsuba Multiplication
**Goal**: Implement Karatsuba in BigNumberNaive (or BigNumberOptimized)

**Tasks:**
1. Implement Karatsuba algorithm
2. Add threshold logic (use naive for small numbers)
3. Benchmark performance
4. Test thoroughly

**Files:**
- `lib/BigNumberNaive.cpp` (add Karatsuba)
- Or create `lib/BigNumberOptimized.h` with Karatsuba

**Estimated**: 2-3 days

---

### Phase 3: SIMD Implementations
**Goal**: Add SIMD implementations (AVX2, AVX-512)

**Tasks:**
1. Create `BigNumberAVX2.h` - AVX2 implementation
2. Create `BigNumberAVX512.h` - AVX-512 implementation
3. Implement SIMD operations (add, multiply)
4. Add detection logic
5. Benchmark vs CPU

**Files:**
- `lib/BigNumberAVX2.h` (new)
- `lib/BigNumberAVX2.cpp` (new)
- `lib/BigNumberAVX512.h` (new)
- `lib/BigNumberAVX512.cpp` (new)

**Estimated**: 1 week

---

### Phase 4: GPU Implementations (Future)
**Goal**: Add GPU support (CUDA, OpenCL)

**Tasks:**
1. Create `BigNumberCUDA.h` - CUDA implementation
2. Create `BigOpenCL.h` - OpenCL implementation
3. Implement GPU kernels
4. Add GPU detection
5. Benchmark vs SIMD/CPU

**Files:**
- `lib/BigNumberCUDA.h` (new)
- `lib/BigNumberCUDA.cpp` (new)
- `lib/BigNumberCUDA.t.cc` (new - GPU-specific tests)
- `lib/BigOpenCL.h` (new)

**Estimated**: 2-3 weeks (when needed)

---

## Quick Start: Phase 1

### Step 1: Create IBigNumber Interface
```cpp
// lib/IBigNumber.h
class IBigNumber {
  virtual void add(...) = 0;
  virtual void multiply(...) = 0;
  // ... operations
};
```

### Step 2: Extract BigNumberNaive
```cpp
// lib/BigNumberNaive.h
class BigNumberNaive : public IBigNumber {
  // Current BigCardinal operations
};
```

### Step 3: Create BigNumber Orchestrator
```cpp
// lib/BigNumber.h
class BigNumber {
  std::variant<std::unique_ptr<BigNumberNaive>, ...> impl_;
  // Auto-detect, hot swap
};
```

### Step 4: Update BigCardinal
```cpp
// lib/BigCardinal.h
class BigCardinal : public BigNumber {
  // Inherits all functionality
};
```

## Success Criteria

1. ✅ All existing tests pass
2. ✅ Performance equal or better
3. ✅ Can select implementation: `BigCardinal(100, BIG_AVX2)`
4. ✅ Auto-detection works
5. ✅ Hot swap works
6. ✅ Clean code (no #ifdefs in headers)

## Next Steps

1. **Start Phase 1**: Create BigNumber orchestrator
2. **Test thoroughly**: Ensure no regressions
3. **Add Karatsuba**: Phase 2
4. **Add SIMD**: Phase 3
5. **Add GPU**: Phase 4 (when needed)

Ready to begin implementation!

