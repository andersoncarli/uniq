# BigNumber Unification Plan

## Goal
Create adaptive BigNumber orchestrator with std::variant, keeping BigCardinal/BigInteger/BigDecimal as user-facing types.

## Architecture

```
BigNumberCore (orchestrator, implementation selection)
    ↓
BigCardinal (unsigned, inherits BigNumberCore)
    ↓
BigInteger (signed, inherits BigCardinal)
    ↓
BigDecimal (decimal, inherits BigInteger)

BigNumber (user-facing, type selection via std::variant)
    ↓
std::variant<BigCardinal, BigInteger, BigDecimal>
```

## Relationship Model

### Two-Level Variant System

**Level 1: Implementation Selection (BigNumberCore)**
- `std::variant<BigNumberNaive*, BigNumberAVX2*, BigNumberCUDA*, ...>`
- Handles hardware implementation (CPU/SIMD/GPU)

**Level 2: Type Selection (BigNumber)**
- `std::variant<BigCardinal, BigInteger, BigDecimal>`
- Handles number type (unsigned/signed/decimal)

### Inheritance (IS-A)
- **BigCardinal IS-A BigNumberCore**: Inherits implementation selection
- **BigInteger IS-A BigCardinal**: Adds sign handling
- **BigDecimal IS-A BigInteger**: Adds decimal point

### Composition (HAS-A)
- **BigNumberCore HAS-A**: `std::variant<BigNumberNaive*, BigNumberAVX2*, ...>` - Implementation selection
- **BigNumberCore HAS-A**: `std::vector<BigDigit> digits_` - Shared storage
- **BigNumber HAS-A**: `std::variant<BigCardinal, BigInteger, BigDecimal>` - Type selection
- **BigInteger HAS-A**: `i8 signal_` - Sign field
- **BigDecimal HAS-A**: `int decimal_places_` - Decimal point position

## User API

### Pattern 1: General BigNumber (Recommended)

**BigNumber** - General type, auto-detects and promotes
```cpp
// Users don't worry about types
BigNumber a(100);           // Auto: Cardinal
BigNumber b(-50);           // Auto: Integer
BigNumber c("123.456");     // Auto: Decimal

// Operations work automatically with promotion
BigNumber sum = a + b;      // Cardinal + Integer → Integer
BigNumber prod = c * a;     // Decimal * Cardinal → Decimal
```

### Pattern 2: Type-Specific (Explicit)

**BigCardinal** - Unsigned integers
```cpp
BigCardinal a(100);              // Auto-detect best implementation
BigCardinal b(50, BIG_AVX2);     // Explicit AVX2
BigCardinal c = a + b;
```

**BigInteger** - Signed integers
```cpp
BigInteger a(-100);
BigInteger b(50);
BigInteger c = a + b;  // Handles signs automatically
```

**BigDecimal** - Fixed-point decimals
```cpp
BigDecimal a("123.456");
BigDecimal b("78.90");
BigDecimal c = a + b;  // Maintains decimal precision
```

### Implementation Selection

```cpp
// Auto-detect (default)
BigCardinal a(100);  // Uses best available (GPU → SIMD → CPU)

// Explicit selection
BigCardinal b(100, BIG_AVX2);
BigCardinal c(100, BIG_CUDA);

// Hot swap
a.setImplementation(BIG_AVX2);
```

### Type Promotion/Demotion

**Automatic Promotion (in BigNumber operations):**
```cpp
BigNumber a = BigCardinal(100);      // Cardinal
BigNumber b = BigInteger(-50);       // Integer
BigNumber c = a + b;                 // Result: Integer (promoted)
```

**Optional Demotion (explicit):**
```cpp
BigNumber a = BigInteger(100);
BigNumber b = BigInteger(-50);
BigNumber c = a + b;                 // Result: Integer(50)

// Demote to Cardinal if safe
BigCardinal d = c.asCardinal();      // Integer(50) → Cardinal(50)
```

**Type Conversions (inheritance):**
```cpp
BigCardinal c(100);
BigInteger i = c;      // Cardinal → Integer (adds positive sign)
BigDecimal d = i;      // Integer → Decimal (adds 0 decimal places)
```

## Implementation Details

### BigNumberCore (Orchestrator)

**Purpose**: Adaptive implementation selection with std::variant

**Contains**:
- `std::variant<BigNumberNaive*, BigNumberAVX2*, BigNumberCUDA*, ...> impl_` - Implementation selection
- `std::vector<BigDigit> digits_` - Shared storage
- `BigImpl active_type_` - Current implementation type
- Auto-detection logic
- Hot swap capability

**Base class for BigCardinal/BigInteger/BigDecimal**

### BigNumber (User-Facing Wrapper)

**Purpose**: General type that can hold any number type

**Contains**:
- `std::variant<BigCardinal, BigInteger, BigDecimal> value_` - Type selection
- Auto-promotion in operations
- Optional demotion via `asCardinal()`, `asInteger()`, `asDecimal()`

**Recommended for most users** - Simple, flexible, automatic

### IBigNumber (Interface)

**Purpose**: Base interface for all implementations

**Methods**:
- `add()`, `subtract()`, `multiply()`, `divide()`
- `compare()`
- `bitwiseAnd()`, `bitwiseOr()`, `bitwiseXor()`, `bitwiseNot()`
- `shiftLeft()`, `shiftRight()`
- `name()`, `priority()`, `available()`

### Implementation Classes

- **BigNumberNaive**: CPU naive (always available)
- **BigNumberOptimized**: CPU optimized (Karatsuba, etc.)
- **BigNumberAVX2**: AVX2 SIMD (256-bit)
- **BigNumberAVX512**: AVX-512 SIMD (512-bit)
- **BigNumberNEON**: ARM NEON
- **BigNumberCUDA**: NVIDIA CUDA
- **BigNumberOpenCL**: OpenCL

## Implementation Steps

### Phase 1: Create BigNumber Orchestrator

**Tasks:**
- [ ] Create `IBigNumber.h` - Base interface
- [ ] Create `BigNumberCore.h` - Orchestrator with std::variant
- [ ] Create `BigNumberCore.cpp` - Factory (all #ifdefs here)
- [ ] Create `BigNumber.h` - User-facing wrapper with type variant
- [ ] Create `BigNumber.cpp` - Type promotion/demotion logic
- [ ] Create `BigNumberNaive.h` - Extract current CPU implementation
- [ ] Update `BigCardinal` to inherit from `BigNumberCore`
- [ ] Test that existing functionality still works

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

**Key Implementation Notes:**
- BigNumber is orchestrator, provides implementation selection
- BigCardinal/BigInteger/BigDecimal inherit from BigNumber
- All #ifdefs centralized in BigNumber.cpp factory
- Clean interface, no #ifdefs in headers

### Phase 2: Karatsuba Multiplication

**Tasks:**
- [ ] Implement Karatsuba in BigNumberNaive (or create BigNumberOptimized)
- [ ] Add threshold logic (use naive for small numbers)
- [ ] Benchmark performance improvement
- [ ] Test thoroughly

**Files:**
- `lib/BigNumberNaive.cpp` (add Karatsuba)
- Or `lib/BigNumberOptimized.h` (new optimized implementation)

### Phase 3: SIMD Implementations

**Tasks:**
- [ ] Create `BigNumberAVX2.h` - AVX2 implementation
- [ ] Create `BigNumberAVX512.h` - AVX-512 implementation
- [ ] Implement SIMD operations (add, multiply)
- [ ] Add detection logic
- [ ] Benchmark vs CPU

**Files:**
- `lib/BigNumberAVX2.h` (new)
- `lib/BigNumberAVX2.cpp` (new)
- `lib/BigNumberAVX512.h` (new)
- `lib/BigNumberAVX512.cpp` (new)

### Phase 4: GPU Implementations (Future)

**Tasks:**
- [ ] Create `BigNumberCUDA.h` - CUDA implementation
- [ ] Create `BigNumberOpenCL.h` - OpenCL implementation
- [ ] Implement GPU kernels
- [ ] Add GPU detection
- [ ] Benchmark vs SIMD/CPU

**Files:**
- `lib/BigNumberCUDA.h` (new)
- `lib/BigNumberCUDA.cpp` (new)
- `lib/BigNumberCUDA.t.cc` (new - GPU-specific tests)

## Files to Create/Modify

### New Files
- `lib/IBigNumber.h` - Base interface
- `lib/BigNumberCore.h` - Orchestrator (implementation selection)
- `lib/BigNumberCore.cpp` - Factory
- `lib/BigNumberCore.t.cc` - Core tests
- `lib/BigNumber.h` - User-facing wrapper (type selection)
- `lib/BigNumber.cpp` - Type promotion/demotion logic
- `lib/BigNumber.t.cc` - BigNumber tests
- `lib/BigNumberNaive.h` - CPU naive implementation
- `lib/BigNumberAVX2.h` - AVX2 SIMD implementation
- `lib/BigNumberAVX512.h` - AVX-512 SIMD implementation

### Modified Files
- `lib/BigCardinal.h` - Inherit from BigNumberCore
- `lib/BigInteger.h` - Inherit from BigCardinal (unchanged)
- `lib/BigDecimal.h` - Inherit from BigInteger (unchanged)
- `lib/uniq.h` - Update includes
- `tests/tests.cc` - Add BigNumber tests

## Build System

```cmake
# CPU implementations
option(BIG_NAIVE "CPU naive" ON)
option(BIG_OPTIMIZED "CPU optimized" ON)

# SIMD implementations
option(BIG_AVX2 "AVX2 SIMD" ON)
option(BIG_AVX512 "AVX-512 SIMD" ON)
option(BIG_NEON "ARM NEON" OFF)

# GPU implementations
option(BIG_CUDA "CUDA GPU" OFF)
option(BIG_OPENCL "OpenCL GPU" OFF)

# Add definitions
if(BIG_NAIVE)
  add_definitions(-DBIG_NAIVE)
endif()

if(BIG_AVX2)
  check_cxx_compiler_flag("-mavx2" HAS_AVX2)
  if(HAS_AVX2)
    add_definitions(-DBIG_AVX2)
    target_compile_options(bignumber PRIVATE -mavx2)
  endif()
endif()
```

## Success Criteria

1. ✅ All existing tests pass (536 tests)
2. ✅ Performance equal or better
3. ✅ Can select implementation: `BigCardinal(100, BIG_AVX2)`
4. ✅ Auto-detection works
5. ✅ Hot swap works
6. ✅ Clean code (no #ifdefs in headers)

## Next Steps

1. **Start Phase 1**: Create BigNumber orchestrator
2. **Incremental**: Complete one phase before moving to next
3. **Test Frequently**: Run tests after each major change
4. **Reference Current Code**: Use BigCardinal as reference for BigNumberNaive
5. **Keep It Simple**: Start with basic functionality, optimize later

**Status**: Ready to start implementation
**Estimated Effort**: ~1.5 weeks (unification + Karatsuba + SIMD)
**Priority**: High (foundation for optimizations)

See:
- `COMPLETE_CLASS_MODEL.md` - Complete class model with all implementations
- `ARCHITECTURE.md` - Architecture overview and user API
- `IMPLEMENTATION_GUIDE.md` - Implementation guide
- `IMPLEMENTATION_ROADMAP.md` - Step-by-step roadmap
