# BigNumber Architecture

## Class Hierarchy

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
- Shared by all types

**Level 2: Type Selection (BigNumber)**
- `std::variant<BigCardinal, BigInteger, BigDecimal>`
- Handles number type (unsigned/signed/decimal)
- User-facing convenience

### Inheritance Chain
- **BigNumberCore**: Base orchestrator with implementation selection (std::variant)
- **BigCardinal**: Inherits BigNumberCore, adds unsigned semantics
- **BigInteger**: Inherits BigCardinal, adds sign handling
- **BigDecimal**: Inherits BigInteger, adds decimal point

### Composition
- **BigNumberCore** contains:
  - `std::variant<BigNumberNaive*, BigNumberAVX2*, BigNumberCUDA*, ...>` - Implementation selection
  - `std::vector<BigDigit> digits_` - Shared storage
  - `BigImpl active_type_` - Current implementation type

- **BigCardinal** inherits all from BigNumberCore, no additional storage

- **BigInteger** adds:
  - `i8 signal_` - Sign field

- **BigDecimal** adds:
  - `int decimal_places_` - Decimal point position

- **BigNumber** contains:
  - `std::variant<BigCardinal, BigInteger, BigDecimal> value_` - Type selection

## User API

### Pattern 1: General BigNumber (Simple - Recommended)

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
BigCardinal a(100);
BigCardinal b(50);
BigCardinal c = a + b;  // Auto-detects best implementation

// Explicit implementation selection
BigCardinal d(200, BIG_AVX2);
```

**BigInteger** - Signed integers
```cpp
BigInteger a(-100);
BigInteger b(50);
BigInteger c = a + b;  // Handles signs automatically

// With explicit implementation
BigInteger d(-200, BIG_AVX2);
```

**BigDecimal** - Fixed-point decimals
```cpp
BigDecimal a("123.456");
BigDecimal b("78.90");
BigDecimal c = a + b;  // Maintains decimal precision

// With explicit implementation
BigDecimal d("100.5", BIG_AVX2);
```

### Implementation Selection

**Auto-detection (default):**
```cpp
BigCardinal a(100);  // Uses best available (GPU → SIMD → CPU)
```

**Explicit selection:**
```cpp
BigCardinal a(100, BIG_AVX2);   // Force AVX2
BigCardinal b(100, BIG_CUDA);   // Force CUDA
BigCardinal c(100, BIG_NAIVE);  // Force CPU naive
```

**Hot swap:**
```cpp
BigCardinal a(100);
a.setImplementation(BIG_AVX2);  // Change at runtime
```

### Type Promotion/Demotion

**Automatic Promotion (in BigNumber operations):**
```cpp
BigNumber a = BigCardinal(100);      // Cardinal
BigNumber b = BigInteger(-50);       // Integer
BigNumber c = a + b;                 // Result: Integer (promoted)

BigNumber d = BigInteger(100);       // Integer
BigNumber e = BigDecimal("50.5");   // Decimal
BigNumber f = d + e;                 // Result: Decimal (promoted)
```

**Optional Demotion (explicit):**
```cpp
BigNumber a = BigInteger(100);       // Integer
BigNumber b = BigInteger(-50);     // Integer
BigNumber c = a + b;                 // Result: Integer(50)

// Demote to Cardinal if safe
BigCardinal d = c.asCardinal();     // Integer(50) → Cardinal(50)

// Or check first
if(c.isInteger() && c.asInteger().sign() >= 0) {
  BigCardinal e = c.asCardinal();   // Safe demotion
}
```

**Type Conversions (inheritance):**
```cpp
BigCardinal c(100);
BigInteger i = c;  // Cardinal → Integer (adds positive sign)
BigDecimal d = i;  // Integer → Decimal (adds 0 decimal places)
```

## Implementation Details

### BigNumberCore (Orchestrator)

**Purpose**: Adaptive implementation selection with std::variant

**Contains**:
- Implementation variant (BigNumberNaive, BigNumberAVX2, BigNumberCUDA, etc.)
- Shared digits storage
- Auto-detection logic
- Hot swap capability

**Base class for BigCardinal/BigInteger/BigDecimal**

### BigNumber (User-Facing Wrapper)

**Purpose**: General type that can hold any number type

**Contains**:
- Type variant: `std::variant<BigCardinal, BigInteger, BigDecimal>`
- Auto-promotion in operations
- Optional demotion via `asCardinal()`, `asInteger()`, `asDecimal()`

**Recommended for most users** - Simple, flexible, automatic

### Implementation Classes

**IBigNumber** - Base interface
- Pure virtual methods for all operations
- All implementations inherit from this

**BigNumberNaive, BigNumberAVX2, BigNumberCUDA, etc.** - Concrete implementations
- Implement IBigNumber interface
- Each optimized for specific hardware

## Usage Patterns

### Pattern 1: Simple Usage (Most Common)
```cpp
// Users work with type-specific classes
BigCardinal a(100);
BigInteger b(-50);
BigDecimal c("123.45");

// Operations work automatically
auto sum = a + BigCardinal(50);
auto diff = b - BigInteger(25);
auto prod = c * BigDecimal("2.0");
```

### Pattern 2: Performance-Critical
```cpp
// Select optimal implementation
BigCardinal a(100, BIG_AVX2);
BigCardinal b(50, BIG_AVX2);
auto result = a * b;  // Uses AVX2 for multiplication
```

### Pattern 3: Testing/Benchmarking
```cpp
// Compare implementations
BigCardinal a(100, BIG_NAIVE);
BigCardinal b(100, BIG_AVX2);
BigCardinal c(100, BIG_CUDA);

// Benchmark each
benchmark(a * a);
benchmark(b * b);
benchmark(c * c);
```

## Key Design Decisions

1. **Two-Level Variant System**
   - **BigNumberCore**: Handles implementation selection (CPU/SIMD/GPU)
   - **BigNumber**: Handles type selection (Cardinal/Integer/Decimal)
   - Separation of concerns: hardware vs. semantics

2. **General BigNumber for Simplicity**
   - Users can use `BigNumber` without type worries
   - Auto-promotes in operations
   - Optional explicit types for safety

3. **Inheritance for Type Hierarchy**
   - BigCardinal IS-A BigNumberCore
   - Clean inheritance chain
   - Natural polymorphism

4. **Shared storage**
   - All types share `digits_` vector
   - No duplication
   - Efficient memory usage

5. **Adaptive by default**
   - Auto-detects best implementation
   - Auto-promotes types in operations
   - User can override if needed
   - Hot swap for flexibility

## Class Structure Details

### BigNumberCore (Orchestrator)

**Purpose**: Handles implementation selection (CPU/SIMD/GPU)

```cpp
class BigNumberCore {
protected:
  std::variant<BigNumberNaive*, BigNumberAVX2*, ...> impl_;
  std::vector<BigDigit> digits_;
  BigImpl active_type_;
  
public:
  // Implementation selection
  void setImplementation(BigImpl type);
  BigImpl getImplementation() const;
  
  // Core operations (delegate to impl_)
  void add(const BigNumberCore& n);
  void multiply(const BigNumberCore& n);
  // ...
};
```

### BigNumber (User-Facing Wrapper)

**Purpose**: General type that can hold any number type

```cpp
class BigNumber {
private:
  std::variant<BigCardinal, BigInteger, BigDecimal> value_;
  
public:
  // Constructors - auto-detect type
  BigNumber(digit n);                    // → BigCardinal
  BigNumber(i64 n);                      // → BigInteger (if negative)
  BigNumber(const string& s);            // → BigDecimal (if has '.')
  
  // Operations - auto-promote/demote
  BigNumber operator+(const BigNumber& n) const;
  BigNumber operator-(const BigNumber& n) const;
  
  // Type queries and accessors
  bool isCardinal() const;
  BigCardinal asCardinal() const;
  // ...
};
```

## File Structure

```
lib/
├── BigNumberCore.h          # Orchestrator (implementation selection)
├── BigNumberCore.cpp        # Factory (all #ifdefs here)
├── BigNumberCore.t.cc       # Core tests
├── IBigNumber.h             # Implementation interface
├── BigCardinal.h            # Cardinal (inherits BigNumberCore)
├── BigInteger.h             # Integer (inherits BigCardinal)
├── BigDecimal.h             # Decimal (inherits BigInteger)
├── BigNumber.h              # User-facing wrapper (type selection)
├── BigNumber.cpp            # Type promotion/demotion logic
├── BigNumber.t.cc           # BigNumber tests
├── BigNumberNaive.h         # CPU naive implementation
├── BigNumberAVX2.h          # AVX2 SIMD implementation
├── BigNumberCUDA.h          # CUDA GPU implementation
└── ...
```

## Summary

**Users work with**: 
- **BigNumber** (recommended) - General type, auto-promotes
- **BigCardinal/BigInteger/BigDecimal** (optional) - Type-specific

**BigNumberCore provides**: Implementation orchestration (CPU/SIMD/GPU)
**BigNumber provides**: Type orchestration (Cardinal/Integer/Decimal)
**Relationship**: Inheritance (IS-A) + Composition (HAS-A variant)
**Storage**: Shared digits vector
**Selection**: Auto-detect or explicit (both levels)

Clean, simple, powerful, flexible!

