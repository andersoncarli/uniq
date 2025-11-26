#pragma once
#include "IBigNumber.h"
#include <vector>

namespace bign {

// Naive CPU implementation - always available
class BigNumberNaive : public IBigNumber {
public:
  void add(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void subtract(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void multiply(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void divide(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b,
              std::vector<BigDigit>& quotient, std::vector<BigDigit>& remainder) override;
  
  int compare(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b) const override;
  
  void bitwiseAnd(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void bitwiseOr(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void bitwiseXor(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) override;
  void bitwiseNot(std::vector<BigDigit>& a) override;
  void shiftLeft(std::vector<BigDigit>& a, int bits) override;
  void shiftRight(std::vector<BigDigit>& a, int bits) override;
  
  const char* name() const override { return "BigNumberNaive"; }
  int priority() const override { return 1; }
  bool available() const override { return true; }
};

} // namespace bign
