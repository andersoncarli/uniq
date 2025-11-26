# BigNumber Implementation Guide

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

## User API

### Pattern 1: General BigNumber (Recommended)

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

```cpp
BigCardinal a(100);
BigCardinal b(50, BIG_AVX2);  // Explicit implementation
auto c = a + b;

BigInteger d(-100);
BigInteger e(50);
auto f = d + e;  // Handles signs

BigDecimal g("123.456");
BigDecimal h("78.90");
auto i = g + h;  // Maintains precision
```

### Implementation Selection

```cpp
// Auto-detect (default)
BigCardinal a(100);

// Explicit
BigCardinal b(100, BIG_AVX2);
BigCardinal c(100, BIG_CUDA);

// Hot swap
a.setImplementation(BIG_AVX2);
```

## Implementation Details

### BigNumberCore (Orchestrator)

- Contains `std::variant` of implementations
- Auto-detects best available
- Provides hot swap
- Base class for BigCardinal/BigInteger/BigDecimal

### BigNumber (User-Facing)

- Contains `std::variant` of types (Cardinal/Integer/Decimal)
- Auto-promotes in operations
- Optional demotion via `asCardinal()`, `asInteger()`, `asDecimal()`

### IBigNumber (Interface)

- Pure virtual interface
- All implementations inherit from this
- Defines operation contracts

### Implementation Classes

- **BigNumberNaive**: CPU naive (always available)
- **BigNumberAVX2**: AVX2 SIMD
- **BigNumberAVX512**: AVX-512 SIMD
- **BigNumberCUDA**: NVIDIA CUDA
- **BigNumberOpenCL**: OpenCL

## File Structure

```
lib/
├── BigNumberCore.h          # Orchestrator
├── BigNumberCore.cpp        # Factory
├── BigNumberCore.t.cc       # Tests
├── IBigNumber.h             # Interface
├── BigCardinal.h            # Cardinal
├── BigInteger.h             # Integer
├── BigDecimal.h             # Decimal
├── BigNumber.h              # User-facing wrapper
├── BigNumber.cpp            # Type promotion logic
├── BigNumber.t.cc           # BigNumber tests
├── BigNumberNaive.h         # CPU naive
├── BigNumberAVX2.h          # AVX2
└── BigNumberCUDA.h          # CUDA
```

## Build Flags

```cmake
option(BIG_NAIVE "CPU naive" ON)
option(BIG_AVX2 "AVX2 SIMD" ON)
option(BIG_CUDA "CUDA GPU" OFF)
```

## Implementation Order

1. **Unification**: Create BigNumberCore orchestrator + BigNumber wrapper
2. **Karatsuba**: Fast multiplication
3. **SIMD**: AVX2/AVX-512 implementations
4. **GPU**: CUDA/OpenCL (future)

See `IMPLEMENTATION_ROADMAP.md` for details.
