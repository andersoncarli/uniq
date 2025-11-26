#include "test_framework.h"
#include "BigNumberCore.h"
#include "BigDigit.h"
#include <cstring>

using namespace bign_test;

namespace bign {

TEST(BigNumberCore) {
  // Basic construction
  BigNumberCore n0;
  CHECK(n0.getDigits().size() == 1);
  CHECK(n0.getDigits()[0] == BigDigit(0));
  
  BigNumberCore n1(BIG_NAIVE);
  CHECK(n1.getImplementation() == BIG_NAIVE);
  CHECK(n1.getDigits().size() == 1);
  
  // Implementation selection
  CHECK(strcmp(n0.getImplementationName(), "BigNumberNaive") == 0);
  CHECK(n0.getImplementation() == BIG_AUTO || n0.getImplementation() == BIG_NAIVE);
  
  // Set implementation
  n0.setImplementation(BIG_NAIVE);
  CHECK(n0.getImplementation() == BIG_NAIVE);
  
  // Copy construction
  BigNumberCore n2(n0);
  CHECK(n2.getDigits().size() == n0.getDigits().size());
  CHECK(n2.getImplementation() == n0.getImplementation());
  
  // Move construction
  BigNumberCore n3(std::move(n2));
  CHECK(n3.getDigits().size() == 1);
  
  // Assignment
  BigNumberCore n4;
  n4 = n3;
  CHECK(n4.getDigits().size() == n3.getDigits().size());
  
  // Set digits
  std::vector<BigDigit> test_digits = {BigDigit(42), BigDigit(100)};
  n4.setDigits(test_digits);
  CHECK(n4.getDigits().size() == 2);
  CHECK(n4.getDigits()[0] == BigDigit(42));
  CHECK(n4.getDigits()[1] == BigDigit(100));
};

TEST(BigNumberCoreOperations) {
  // Create test numbers
  BigNumberCore a, b;
  a.setDigits({BigDigit(10)});
  b.setDigits({BigDigit(5)});
  
  // Addition
  BigNumberCore sum = a;
  sum.add(b);
  CHECK(sum.getDigits().size() >= 1);
  CHECK(sum.getDigits()[0].value >= 15);
  
  // Comparison
  int cmp = a.compare(b);
  CHECK(cmp > 0);  // 10 > 5
  
  BigNumberCore c;
  c.setDigits({BigDigit(10)});
  CHECK(a.compare(c) == 0);  // 10 == 10
  
  // Multiplication
  BigNumberCore prod = a;
  prod.multiply(b);
  CHECK(prod.getDigits().size() >= 1);
  
  // Division
  BigNumberCore quotient, remainder;
  a.divide(b, quotient, remainder);
  CHECK(quotient.getDigits().size() >= 1);
  CHECK(remainder.getDigits().size() >= 1);
  
  // Bitwise operations
  BigNumberCore bitwise = a;
  bitwise.bitwiseAnd(b);
  CHECK(bitwise.getDigits().size() >= 1);
  
  // Shifts
  BigNumberCore shifted = a;
  shifted.shiftLeft(2);
  CHECK(shifted.getDigits().size() >= 1);
  
  shifted = a;
  shifted.shiftRight(1);
  CHECK(shifted.getDigits().size() >= 1);
};

} // namespace bign

int main() {
    TestFramework::runTests();
    return TestFramework::getExitCode();
}
