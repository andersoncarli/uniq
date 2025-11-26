// Main test file that includes all unit tests
#include "test.h"
#include "test.cc"

// Include all test files
#include "BigDigit.t.cc"
#include "BigCardinal.t.cc"
#include "BigInteger.t.cc"
#include "BigNumberCore.t.cc"
#include "BigNumber.t.cc"
#include "BigDecimal.t.cc"
// #include "BigPrime.t.cc"  // TODO: Fix BigPrime compatibility with new BigNumber

int main() {
  return bign::runTests();
}

