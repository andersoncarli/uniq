# BigDecimal vs BigFloat: Implementation Decision

## Recommendation: **Implement Both, Start with BigDecimal**

### Why Both?

1. **Different Use Cases:**
   - **BigDecimal**: Financial, exact decimal calculations
   - **BigFloat**: Scientific computing, wide range

2. **Complementary, Not Competing:**
   - Different algorithms and storage
   - Different precision models
   - Different use cases

3. **Industry Standard:**
   - Java: `BigDecimal` + `BigDecimal` (no separate float)
   - Python: `decimal.Decimal` + `mpmath` (float)
   - C#: `decimal` + `BigFloat` libraries
   - GMP: `mpz_t` (int) + `mpf_t` (float)

## Architecture

```
BigCardinal (unsigned integers)
    ↓
BigInteger (signed integers)
    ↓
BigDecimal (fixed-point decimal) ← Start here
BigFloat (floating-point)        ← Add later
```

## Implementation Priority

### Phase 1: BigDecimal (Recommended First)
**Why:**
- More commonly needed
- Simpler implementation (fixed-point)
- Exact decimal representation
- Financial applications

**Implementation:**
- Store as `BigInteger` + `decimal_places` offset
- All operations maintain decimal precision
- Format: "123.456"

### Phase 2: BigFloat (Add Later)
**Why:**
- Scientific computing needs
- Wider value range
- Exponential notation

**Implementation:**
- Store as `sign` + `mantissa` (BigInteger) + `exponent` (i64)
- Variable precision
- Format: "1.23e10" or "1.23×10^10"

## Comparison

| Feature | BigDecimal | BigFloat |
|---------|-----------|----------|
| **Base** | 10 (decimal) | 2 or 10 |
| **Precision** | Fixed (exact) | Variable |
| **Range** | Limited by digits | Exponential |
| **Use Case** | Financial, exact | Scientific, approximate |
| **Complexity** | Simpler | More complex |
| **Performance** | Faster (fixed-point) | Slower (normalization) |
| **Rounding** | Exact | Approximate |

## Decision Matrix

**Choose BigDecimal if:**
- ✅ Financial calculations
- ✅ Exact decimal representation needed
- ✅ Currency/monetary operations
- ✅ Accounting applications
- ✅ Want simpler implementation

**Choose BigFloat if:**
- ✅ Scientific computing
- ✅ Need very large/small numbers
- ✅ Exponential notation needed
- ✅ Approximate precision acceptable
- ✅ Performance-critical numeric work

**Choose Both if:**
- ✅ General-purpose library
- ✅ Want to cover all use cases
- ✅ Have resources for both

## Recommendation

**Start with BigDecimal** because:
1. More commonly needed
2. Simpler to implement correctly
3. Exact representation is valuable
4. Can add BigFloat later if needed

**Add BigFloat later** when:
- Scientific computing needs arise
- Users request exponential notation
- Performance requirements demand it

## Current Status

- ✅ **BigCardinal** - Implemented (unsigned)
- ✅ **BigInteger** - Implemented (signed)
- 📝 **BigDecimal** - Placeholder created (ready for implementation)
- 📝 **BigFloat** - Placeholder created (ready for implementation)

Both placeholders are in place. Start implementing BigDecimal first, then add BigFloat when needed.

