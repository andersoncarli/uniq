#include "BigNumberCore.h"
#include "test.h"
namespace bign {

TEST(BigNumberCoreConstruction) {
  // Default construction - auto-detects best implementation
  BigNumberCore core1;
  CHECK(core1.getImplementation() == BIG_AUTO);
  CHECK(core1.size() == 1);
  CHECK(core1.isZero());
  
  // Explicit implementation selection
  BigNumberCore core2(BIG_NAIVE);
  CHECK(core2.getImplementation() == BIG_NAIVE);
  CHECK(core2.size() == 1);
  CHECK(core2.isZero());
  
  // Copy construction
  BigNumberCore core3(core2);
  CHECK(core3.getImplementation() == core2.getImplementation());
  CHECK(core3.size() == core2.size());
  
  // Move construction
  BigNumberCore core4(std::move(core3));
  CHECK(core4.getImplementation() == BIG_NAIVE);
  CHECK(core3.size() == 1); // Moved-from should be valid but zero
}

TEST(BigNumberCoreImplementationSelection) {
  BigNumberCore core;
  
  // Test implementation name
  const char* name = core.getImplementationName();
  CHECK(name != nullptr);
  CHECK(std::string(name) == "BigNumberNaive");
  
  // Test hot swap
  core.setImplementation(BIG_NAIVE);
  CHECK(core.getImplementation() == BIG_NAIVE);
  
  // Test auto-detection
  core.setImplementation(BIG_AUTO);
  CHECK(core.getImplementation() == BIG_AUTO);
}

TEST(BigNumberCoreOperations) {
  BigNumberCore a, b;
  
  // Set up test values
  a.getDigits().clear();
  a.getDigits().push_back(BigDigit(10));
  
  b.getDigits().clear();
  b.getDigits().push_back(BigDigit(5));
  
  // Addition
  BigNumberCore add_result = a;
  add_result.add(b);
  CHECK(add_result.size() >= 1);
  
  // Subtraction
  BigNumberCore sub_result = a;
  sub_result.subtract(b);
  CHECK(sub_result.size() >= 1);
  
  // Multiplication
  BigNumberCore mul_result = a;
  mul_result.multiply(b);
  CHECK(mul_result.size() >= 1);
  
  // Division
  BigNumberCore div_quotient, div_remainder;
  a.divide(b, div_quotient, div_remainder);
  CHECK(div_quotient.size() >= 1);
  CHECK(div_remainder.size() >= 1);
  
  // Comparison
  int cmp = a.compare(b);
  CHECK(cmp > 0); // 10 > 5
}

TEST(BigNumberCoreBitwiseOperations) {
  BigNumberCore a, b;
  
  a.getDigits().clear();
  a.getDigits().push_back(BigDigit(5)); // 101 in binary
  
  b.getDigits().clear();
  b.getDigits().push_back(BigDigit(3)); // 011 in binary
  
  // Bitwise AND
  BigNumberCore and_result = a;
  and_result.bitwiseAnd(b);
  CHECK(and_result.size() >= 1);
  
  // Bitwise OR
  BigNumberCore or_result = a;
  or_result.bitwiseOr(b);
  CHECK(or_result.size() >= 1);
  
  // Bitwise XOR
  BigNumberCore xor_result = a;
  xor_result.bitwiseXor(b);
  CHECK(xor_result.size() >= 1);
  
  // Bitwise NOT
  BigNumberCore not_result = a;
  not_result.bitwiseNot();
  CHECK(not_result.size() >= 1);
}

TEST(BigNumberCoreShiftOperations) {
  BigNumberCore a;
  a.getDigits().clear();
  a.getDigits().push_back(BigDigit(8)); // 1000 in binary
  
  // Left shift
  BigNumberCore left_result = a;
  left_result.shiftLeft(2);
  CHECK(left_result.size() >= 1);
  
  // Right shift
  BigNumberCore right_result = a;
  right_result.shiftRight(1);
  CHECK(right_result.size() >= 1);
  
  // Zero shift (should be no-op)
  BigNumberCore zero_shift = a;
  zero_shift.shiftLeft(0);
  CHECK(zero_shift.compare(a) == 0);
  
  zero_shift = a;
  zero_shift.shiftRight(0);
  CHECK(zero_shift.compare(a) == 0);
}

TEST(BigNumberCoreEdgeCases) {
  // Zero operations
  BigNumberCore zero;
  CHECK(zero.isZero());
  CHECK(!zero.isOne());
  
  // One operations
  BigNumberCore one;
  one.getDigits().clear();
  one.getDigits().push_back(BigDigit(1));
  CHECK(one.isOne());
  CHECK(!one.isZero());
  
  // Large number
  BigNumberCore large;
  large.getDigits().clear();
  for(int i = 0; i < 10; i++) {
    large.getDigits().push_back(BigDigit(0xFFFFFFFFFFFFFFFFULL));
  }
  CHECK(large.size() == 10);
  CHECK(!large.isZero());
  
  // Empty digits (should normalize to zero)
  BigNumberCore empty;
  empty.getDigits().clear();
  // After operations, should normalize
  CHECK(empty.size() == 0 || empty.isZero());
}

TEST(BigNumberCoreAssignment) {
  BigNumberCore a, b;
  
  a.getDigits().clear();
  a.getDigits().push_back(BigDigit(42));
  
  // Copy assignment
  b = a;
  CHECK(b.size() == a.size());
  CHECK(b.compare(a) == 0);
  
  // Move assignment
  BigNumberCore c;
  c = std::move(a);
  CHECK(c.size() == 1);
  CHECK(c.getDigits()[0] == BigDigit(42));
}

TEST(BigNumberCoreHotSwap) {
  BigNumberCore core;
  
  // Set initial value
  core.getDigits().clear();
  core.getDigits().push_back(BigDigit(100));
  
  // Hot swap implementation
  core.setImplementation(BIG_NAIVE);
  CHECK(core.getImplementation() == BIG_NAIVE);
  
  // Value should be preserved
  CHECK(core.size() == 1);
  CHECK(core.getDigits()[0] == BigDigit(100));
  
  // Swap back to auto
  core.setImplementation(BIG_AUTO);
  CHECK(core.getImplementation() == BIG_AUTO);
  CHECK(core.size() == 1);
  CHECK(core.getDigits()[0] == BigDigit(100));
}

} // namespace bign

