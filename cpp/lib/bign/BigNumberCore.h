#pragma once
#include <variant>
#include <memory>
#include <vector>
#include "IBigNumber.h"
#include "BigDigit.h"
#include "BigNumberNaive.h"

namespace bign {

// Implementation type enum
enum BigImpl {
  BIG_AUTO,      // Auto-detect best
  BIG_NAIVE      // Basic CPU (always available)
};

// Implementation variant - currently only naive
using BigNumVariant = std::variant<
  std::unique_ptr<BigNumberNaive>
>;

// Orchestrator class - handles implementation selection
class BigNumberCore {
protected:
  BigNumVariant impl_;
  BigImpl active_type_;
  std::vector<BigDigit> digits_;
  
  // Factory methods (implemented in .cpp)
  static BigNumVariant createBest();
  static BigNumVariant create(BigImpl type);
  
  // Visitor helper for operations
  template<typename Op>
  void visitImpl(Op&& op) {
    std::visit([&](auto& impl_ptr) {
      if(impl_ptr) {
        op(*impl_ptr);
      }
    }, impl_);
  }
  
public:
  BigNumberCore();
  BigNumberCore(BigImpl type);
  BigNumberCore(const BigNumberCore& other);
  BigNumberCore(BigNumberCore&& other) noexcept;
  virtual ~BigNumberCore() = default;
  
  BigNumberCore& operator=(const BigNumberCore& other);
  BigNumberCore& operator=(BigNumberCore&& other) noexcept;
  
  // Implementation selection
  void setImplementation(BigImpl type);
  BigImpl getImplementation() const { return active_type_; }
  const char* getImplementationName() const;
  
  // Access to digits (for derived classes and implementations)
  const std::vector<BigDigit>& getDigits() const { return digits_; }
  std::vector<BigDigit>& getDigits() { return digits_; }
  void setDigits(const std::vector<BigDigit>& d) { digits_ = d; }
  
  // Core operations (delegate to implementation)
  void add(const BigNumberCore& n);
  void subtract(const BigNumberCore& n);
  void multiply(const BigNumberCore& n);
  void divide(const BigNumberCore& n, BigNumberCore& quotient, BigNumberCore& remainder) const;
  
  // Comparison
  int compare(const BigNumberCore& n) const;
  
  // Bitwise operations
  void bitwiseAnd(const BigNumberCore& n);
  void bitwiseOr(const BigNumberCore& n);
  void bitwiseXor(const BigNumberCore& n);
  void bitwiseNot();
  void shiftLeft(int bits);
  void shiftRight(int bits);
};

} // namespace bign
