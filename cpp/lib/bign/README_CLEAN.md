# BigNumber Refactoring - Clean bign Namespace Implementation

## Overview

This directory contains a **completely isolated** `bign` namespace implementation of the BigNumber architecture. It is **entirely separate** from the `uniq` namespace, avoiding all circular dependency issues.

## Structure

### Core Files (Clean Implementation)

- **`IBigNumber.h`** - Base interface for all implementations
- **`BigNumberNaive.h/.cpp`** - Naive CPU implementation (always available)
- **`BigNumberCore.h/.cpp`** - Orchestrator with `std::variant` for implementation selection
- **`BigDigit.h`** - Uses the existing `bign::BigDigit` from this directory

### Test Files

- **`tests.cc`** - Isolated test suite for `bign` namespace
- **`Makefile`** - Simple build system for tests

## Key Features

1. **Complete Isolation**: No dependencies on `uniq` namespace
2. **Clean Architecture**: Uses `std::variant` for implementation selection
3. **Extensible**: Easy to add new implementations (SIMD, GPU, etc.)
4. **Testable**: Self-contained test suite

## Building and Testing

```bash
cd lib/bign
make
make test
```

Or manually:
```bash
g++ -std=c++17 -I.. -I../.. -DTESTING -o bign_tests \
    BigNumberNaive.cpp BigNumberCore.cpp tests.cc -lpthread
./bign_tests
```

## Architecture

```
BigNumberCore (orchestrator)
  └── std::variant<
        std::unique_ptr<BigNumberNaive>,
        ... (future: BigNumberAVX2, BigNumberCUDA, etc.)
      >
```

## Next Steps

1. ✅ Core infrastructure complete
2. ⏭️ Add more implementations (Karatsuba, SIMD, GPU)
3. ⏭️ Implement full long division algorithm
4. ⏭️ Create user-facing `BigNumber` wrapper with type variants

## Notes

- All files use `bign` namespace
- Uses `bign::BigDigit` from `lib/bign/BigDigit.h`
- No circular dependencies
- Clean separation from existing `uniq` codebase

