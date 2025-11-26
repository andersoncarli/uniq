# BigNumber Library - Planning Summary

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
BigNumber sum = a + b;       // Cardinal + Integer → Integer
BigNumber prod = c * a;     // Decimal * Cardinal → Decimal
```

### Pattern 2: Type-Specific (Explicit)

**BigCardinal** - Unsigned integers
```cpp
BigCardinal a(100);
BigCardinal b(50, BIG_AVX2);
auto c = a + b;
```

**BigInteger** - Signed integers
```cpp
BigInteger a(-100);
BigInteger b(50);
auto c = a + b;  // Handles signs
```

**BigDecimal** - Fixed-point decimals
```cpp
BigDecimal a("123.456");
BigDecimal b("78.90");
auto c = a + b;  // Maintains precision
```

## Type Promotion/Demotion

**Automatic Promotion:**
```cpp
BigNumber a = BigCardinal(100);      // Cardinal
BigNumber b = BigInteger(-50);       // Integer
BigNumber c = a + b;                 // Result: Integer (auto-promoted)
```

**Optional Demotion:**
```cpp
BigNumber a = BigInteger(100);
BigNumber b = BigInteger(-50);
BigNumber c = a + b;                 // Result: Integer(50)

// Demote to Cardinal if safe
BigCardinal d = c.asCardinal();      // Integer(50) → Cardinal(50)
```

## Implementation Classes

- **IBigNumber**: Base interface
- **BigNumberCore**: Orchestrator (implementation selection)
- **BigNumber**: User-facing wrapper (type selection)
- **BigNumberNaive**: CPU naive (always available)
- **BigNumberAVX2**: AVX2 SIMD
- **BigNumberAVX512**: AVX-512 SIMD
- **BigNumberCUDA**: CUDA GPU (future)

## Documentation

- **`ARCHITECTURE.md`** - Architecture and user API
- **`UNIFIED_ARCHITECTURE.md`** - Detailed unified architecture
- **`COMPLETE_CLASS_MODEL.md`** - Complete class model
- **`IMPLEMENTATION_ROADMAP.md`** - Step-by-step plan
- **`BIGNUMBER_UNIFICATION_PLAN.md`** - Unification plan
- **`IMPLEMENTATION_GUIDE.md`** - Quick guide

## Implementation Order

1. ✅ Long Division (completed)
2. 🔄 Unification - Create BigNumberCore orchestrator + BigNumber wrapper
3. ⏳ Karatsuba - Fast multiplication
4. ⏳ SIMD - AVX2, AVX-512 implementations
5. ⏳ GPU - CUDA, OpenCL (future)

## Key Features

- ✅ General BigNumber type (users don't worry about types)
- ✅ Type-specific classes (for explicit control)
- ✅ Automatic type promotion in operations
- ✅ Optional type demotion (when safe)
- ✅ Runtime auto-detection (implementation)
- ✅ Compile-time selection
- ✅ Hot swap capability
- ✅ Clean interface (no #ifdefs in headers)
- ✅ Zero overhead (std::variant, direct calls)
- ✅ Extensible (easy to add implementations)

Ready to start implementation!
