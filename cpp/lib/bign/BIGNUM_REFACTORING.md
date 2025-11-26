# BigNumber Refactoring Plan

## Quick Wins Completed ✓

1. **Fixed Digit bitwise operators** - Now return `Digit` instead of `bool`
2. **Added input validation** - Base validation, bounds checking in constructors and format functions
3. **Implemented proper bit-shift operations** - Replaced inefficient multiply/divide with bit-level operations
4. **Added sqrt() and mod_pow()** - Essential mathematical functions
5. **Fixed FlowException thread-safety** - Removed mutable string, construct message in constructor
6. **Documented storage order** - Clarified digit storage implementation

## Major Refactorings Needed

### Priority 1: Signed Arithmetic (HIGH)

**Current State**: `signal` field exists but is never used. All operations assume unsigned numbers.

**Required Changes**:
- Implement sign handling in all arithmetic operations (+, -, *, /, %)
- Update comparison operators to handle negative numbers
- Fix string constructor to properly parse negative numbers (partially done)
- Add `abs()`, `negate()`, `sign()` helper functions
- Update `format()` to output negative sign when `signal < 0`

**Files**: `lib/Number.h`
**Estimated Effort**: 8-12 hours
**Dependencies**: None

**Implementation Notes**:
- Subtraction currently throws on underflow - need to allow negative results
- Multiplication: `sign(result) = sign(a) * sign(b)`
- Division: handle negative dividends/divisors correctly
- Comparisons: negative < positive, compare absolute values when signs differ

---

### Priority 2: Algorithm Optimization (MEDIUM-HIGH)

**Current State**: 
- Multiplication: O(n²) naive algorithm
- Division: Repeated subtraction (O(quotient))
- Shifts: Now optimized for small shifts, but fallback to multiply/divide for large shifts

**Required Changes**:

#### 2.1 Karatsuba Multiplication
- Implement Karatsuba algorithm for numbers with > threshold digits
- Threshold: ~50-100 digits (needs benchmarking)
- Fallback to naive for small numbers

#### 2.2 Long Division Algorithm
- Replace repeated subtraction with proper long division
- O(n²) complexity instead of O(quotient)
- Handle normalization and quotient estimation

#### 2.3 Optimized Large Shifts
- For shifts > BITS(digit), use digit-level operations
- Shift whole digits, then bit-shift remainder
- Avoid multiplication/division fallback

**Files**: `lib/Number.h` (multiplication, division, shift operations)
**Estimated Effort**: 12-16 hours
**Dependencies**: None (can be done incrementally)

---

### Priority 3: Memory & Performance (MEDIUM)

**Current State**: 
- Frequent vector resizing in hot paths
- Many temporary Number objects created
- No memory pooling

**Required Changes**:

#### 3.1 Reserve Strategy
- Pre-allocate vector capacity in multiplication/division
- Use `reserve()` before loops that grow vectors
- Estimate result size before operations

#### 3.2 Reduce Temporary Objects
- Use move semantics more aggressively
- Return by value optimization (RVO) where possible
- Consider in-place operations for compound assignments

#### 3.3 Small Number Optimization (SSO)
- For numbers fitting in single digit, avoid vector allocation
- Use union or optional<vector> for small numbers
- Only allocate vector when needed

**Files**: `lib/Number.h`
**Estimated Effort**: 6-8 hours
**Dependencies**: None

---

### Priority 4: API Completeness (MEDIUM)

**Current State**: Missing several standard BigNumber operations

**Required Changes**:

#### 4.1 Conversion Functions
```cpp
int64_t to_int64() const;  // with overflow detection
uint64_t to_uint64() const; // with overflow detection
double to_double() const;   // approximate conversion
string to_string(int base=10) const; // alias for format()
```

#### 4.2 Utility Functions
```cpp
Number abs() const;
Number negate() const;
int sign() const;  // returns -1, 0, or 1
bool isPositive() const;
bool isNegative() const;
Number min(const Number& a, const Number& b);
Number max(const Number& a, const Number& b);
```

#### 4.3 Random Number Generation
```cpp
static Number random(const Number& min, const Number& max);
static Number random_bits(int num_bits); // random number with N bits
```

**Files**: `lib/Number.h`
**Estimated Effort**: 4-6 hours
**Dependencies**: Signed arithmetic (Priority 1)

---

### Priority 5: Robustness & Safety (MEDIUM)

**Current State**: Some operations lack proper error handling

**Required Changes**:

#### 5.1 Bounds Checking
- Add `operator[]` bounds checking (currently unchecked)
- Validate array access in all operations
- Add `at()` method with bounds checking

#### 5.2 Input Validation
- Validate base parameter in all format/parse operations
- Check map length matches base requirements
- Validate string inputs for invalid characters

#### 5.3 Overflow Protection
- Add maximum size limits to prevent DoS
- Limit recursion depth in operations
- Timeout protection for expensive operations

**Files**: `lib/Digit.h`, `lib/Number.h`
**Estimated Effort**: 4-6 hours
**Dependencies**: None

---

### Priority 6: Platform Portability (LOW-MEDIUM)

**Current State**: Assumes specific platform features

**Required Changes**:

#### 6.1 Endianness Handling
- Document endianness assumptions
- Add compile-time checks for endianness
- Consider making storage order configurable

#### 6.2 Word Size Portability
- Replace `__WORDSIZE` with portable detection
- Handle 32-bit vs 64-bit differences
- Test on multiple platforms

#### 6.3 Type Definitions
- Ensure `uoverflow` is properly defined on all platforms
- Add fallbacks for platforms without `__uint128_t`
- Use standard integer types where possible

**Files**: `lib/Digit.h`, `lib/numtypes.h`
**Estimated Effort**: 4-6 hours
**Dependencies**: None

---

## Implementation Order Recommendation

### Phase 1: Core Functionality (Weeks 1-2)
1. **Signed Arithmetic** (Priority 1) - Foundation for everything else
2. **API Completeness** (Priority 4) - Essential conversions and utilities

### Phase 2: Performance (Weeks 3-4)
3. **Algorithm Optimization** (Priority 2) - Karatsuba, long division
4. **Memory & Performance** (Priority 3) - Reduce allocations, optimize hot paths

### Phase 3: Polish (Week 5)
5. **Robustness & Safety** (Priority 5) - Error handling, validation
6. **Platform Portability** (Priority 6) - Cross-platform support

---

## Testing Strategy

### For Each Refactoring:
1. **Unit Tests**: Test new functionality in isolation
2. **Regression Tests**: Ensure existing tests still pass
3. **Edge Cases**: Zero, one, negative numbers, large numbers
4. **Performance Tests**: Benchmark before/after for optimizations

### New Test Categories Needed:
- Signed arithmetic tests (negative numbers, sign handling)
- Large number performance tests (1000+ digits)
- Conversion function tests (overflow cases)
- Platform-specific tests (32-bit vs 64-bit)

---

## Success Criteria

- [ ] All 352+ existing tests pass
- [ ] Signed arithmetic fully functional
- [ ] Performance improved 2-10x for large numbers (depending on operation)
- [ ] Memory usage reduced by 20-30%
- [ ] API complete with all standard BigNumber operations
- [ ] Works on 32-bit and 64-bit platforms
- [ ] Comprehensive error handling

---

## Notes

1. **Breaking Changes**: Signed arithmetic may change behavior of existing code that assumes unsigned. Consider versioning.

2. **Performance Targets**:
   - Multiplication: 10x faster for 1000-digit numbers (Karatsuba)
   - Division: 100x faster for large quotients (long division)
   - Memory: 20-30% reduction through SSO and reserve strategy

3. **Compatibility**: Maintain backward compatibility where possible. Use feature flags if needed.

4. **Documentation**: Update API docs as features are added. Document algorithm choices and complexity.

---

## Estimated Total Effort

- **Phase 1**: 12-18 hours
- **Phase 2**: 18-24 hours  
- **Phase 3**: 8-12 hours
- **Total**: 38-54 hours

This plan can be executed incrementally, with each priority completed independently.

