#include "BigPrime.h"
#include "test.h"
using namespace std;
namespace bign {

TEST(PrimalityTest) {
  initPrimality(200);
  
  struct TestCase {
    BigNumber n;
    bool expected;
  };
  
  vector<TestCase> tests = {
    // Primes
    {2, true}, {3, true}, {5, true}, {7, true}, {11, true}, {13, true},
    {17, true}, {19, true}, {23, true}, {29, true}, {31, true},
    {101, true}, {103, true}, {107, true}, {109, true}, {113, true},
    // Composites
    {4, false}, {6, false}, {8, false}, {9, false}, {10, false},
    {15, false}, {21, false}, {25, false}, {100, false},
    // Large numbers
    {BigInteger(pow(BigCardinal(2), BigCardinal(31)) - BigCardinal(1)), true},
    {BigNumber("172947529"), false},
  };
  
  for(const auto& test : tests) {
    bool qr = isPrimeQR(test.n, 10);
    bool mr = isPrimeMR(test.n, 10);
    bool td = isPrimeTD(test.n);
    
    // All three algorithms should agree
    CHECK(qr == mr);
    CHECK(mr == td);
    CHECK(qr == test.expected);
    
    // Test firstFactor for composites
    if(!test.expected && test.n > 1) {
      BigNumber factor = firstFactor(test.n);
      CHECK(factor > 1);
      CHECK(factor <= test.n);
      CHECK((test.n % factor) == 0);
    }
  }
  
  // Edge cases
  CHECK(!isPrimeQR(0, 10));
  CHECK(!isPrimeQR(1, 10));
  CHECK(!isPrimeMR(0, 10));
  CHECK(!isPrimeMR(1, 10));
  CHECK(!isPrimeTD(0));
  CHECK(!isPrimeTD(1));
};

} // namespace bign
