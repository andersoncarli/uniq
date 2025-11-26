#pragma once
#include <vector>
#include "BigDigit.h"

namespace bign {

// Base interface for all BigNumber implementations
class IBigNumber {
public:
  virtual ~IBigNumber() = default;
  
  // Core arithmetic operations
  virtual void add(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) = 0;
  virtual void subtract(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) = 0;
  virtual void multiply(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) = 0;
  virtual void divide(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b,
                      std::vector<BigDigit>& quotient, std::vector<BigDigit>& remainder) = 0;
  
  // Comparison
  virtual int compare(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b) const = 0;
  
  // Bitwise operations (Cardinal only)
  virtual void bitwiseAnd(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) = 0;
  virtual void bitwiseOr(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) = 0;
  virtual void bitwiseXor(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) = 0;
  virtual void bitwiseNot(std::vector<BigDigit>& a) = 0;
  virtual void shiftLeft(std::vector<BigDigit>& a, int bits) = 0;
  virtual void shiftRight(std::vector<BigDigit>& a, int bits) = 0;
  
  // Metadata
  virtual const char* name() const = 0;
  virtual int priority() const = 0;  // Higher = better (for auto-detection)
  virtual bool available() const = 0;
};

} // namespace bign
