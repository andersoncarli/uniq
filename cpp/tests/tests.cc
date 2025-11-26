#include "backward.hpp"
namespace backward {
backward::SignalHandling sh;
}

#define TESTING 1 // Comment to disable tests
#include "uniq.h"

#include "State.h"
#include "BigDigit.t.cc"
// #include "BigNumberCore.t.cc"  // Old version - disabled
#include "BigNumberCore2.t.cc"  // New version - testing
#include "BigCardinal.t.cc"
#include "BigInteger.t.cc"
#include "BigDecimal.t.cc"
#include "BigPrime.t.cc"

int main(){
  uniq::SILENT_TESTS = 1; // omit success report
  quick_exit( uniq::runTests() );
}
