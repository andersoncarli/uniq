#pragma once
#include "IBigNumber.h"
#include "BigNumberNaive.h"

namespace bign {

// Karatsuba multiplication implementation
// Uses naive multiplication for small numbers, Karatsuba for large numbers
class BigNumberKaratsuba : public IBigNumber {
public:
  // Core arithmetic operations
  void add(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void subtract(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void multiply(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void divide(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b,
              std::vector<BigDigit>& quotient, std::vector<BigDigit>& remainder) override;
  
  // Comparison
  int compare(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b) const override;
  
  // Bitwise operations
  void bitwiseAnd(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void bitwiseOr(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void bitwiseXor(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void bitwiseNot(std::vector<BigDigit>& a) override;
  void shiftLeft(std::vector<BigDigit>& a, int bits) override;
  void shiftRight(std::vector<BigDigit>& a, int bits) override;
  
  // Metadata
  const char* name() const override { return "BigNumberKaratsuba"; }
  int priority() const override { return 2; }  // Higher than naive
  bool available() const override { return true; }

private:
  // Threshold for switching to Karatsuba (in digits)
  static constexpr int KARATSUBA_THRESHOLD = 50;
  
  // Naive implementation for delegation
  BigNumberNaive naive_;
  
  // Helper functions
  void normalize(std::vector<BigDigit>& a);
  void multiplyKaratsubaRecursive(std::vector<BigDigit>& a, const std::vector<BigDigit>& b);
  void splitAt(const std::vector<BigDigit>& src, int pos, std::vector<BigDigit>& low, std::vector<BigDigit>& high);
};

} // namespace bign

