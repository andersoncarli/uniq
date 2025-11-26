#pragma once
#include "BigInteger.h"
// Forward declarations for future implementations
// #include "BigDecimal.h"  // TODO: Implement fixed-point decimal
// #include "BigFloat.h"    // TODO: Implement floating-point
namespace uniq {

// BigNumber is unified interface - currently aliased to BigInteger
// Future: Could become base class or wrapper for Cardinal/Integer/Decimal/Float
using BigNumber = BigInteger;

// Re-export utility functions for backward compatibility (functions are already in namespace)
// pow, gcd, lcm, sqrt, mod_pow are available via BigInteger namespace

} // namespace uniq
