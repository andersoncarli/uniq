#pragma once
#include <variant>
#include <memory>
#include <vector>
#include "BigDigit.h"
#include "IBigNumber.h"
#include "BigNumberNaive.h"  // Need full definition for variant
#include "BigNumberKaratsuba.h"  // Karatsuba multiplication

namespace bign {

// Implementation type enum
enum BigImpl {
  BIG_AUTO,      // Auto-detect best
  BIG_NAIVE,     // Basic CPU
  BIG_KARATSUBA, // Karatsuba multiplication
  // Future: BIG_AVX2, BIG_CUDA, etc.
};

// Implementation variant - includes BigNumberNaive and BigNumberKaratsuba
using BigNumVariant = std::variant<std::unique_ptr<BigNumberNaive>, std::unique_ptr<BigNumberKaratsuba>>;

// Main orchestrator class - handles implementation selection
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
  
  template<typename Op>
  void visitImpl(Op&& op) const {
    std::visit([&](const auto& impl_ptr) {
      if(impl_ptr) {
        op(*impl_ptr);
      }
    }, impl_);
  }

public:
  // Constructors
  BigNumberCore();  // Auto-detect best
  BigNumberCore(BigImpl type);  // Explicit selection
  BigNumberCore(const BigNumberCore& other);
  BigNumberCore(BigNumberCore&& other) noexcept;
  
  // Assignment
  BigNumberCore& operator=(const BigNumberCore& other);
  BigNumberCore& operator=(BigNumberCore&& other) noexcept;
  
  // Implementation selection
  void setImplementation(BigImpl type);
  BigImpl getImplementation() const { return active_type_; }
  const char* getImplementationName() const;
  
  // Core operations (delegate to implementation)
  void add(const BigNumberCore& n);
  void subtract(const BigNumberCore& n);
  void multiply(const BigNumberCore& n);
  void divide(const BigNumberCore& n, BigNumberCore& quotient, BigNumberCore& remainder);
  
  // Comparison
  int compare(const BigNumberCore& n) const;
  
  // Bitwise operations
  void bitwiseAnd(const BigNumberCore& n);
  void bitwiseOr(const BigNumberCore& n);
  void bitwiseXor(const BigNumberCore& n);
  void bitwiseNot();
  void shiftLeft(int bits);
  void shiftRight(int bits);
  
  // Accessors
  const std::vector<BigDigit>& getDigits() const { return digits_; }
  std::vector<BigDigit>& getDigits() { return digits_; }
  int size() const { return (int)digits_.size(); }
  bool isZero() const { return digits_.empty() || (digits_.size() == 1 && digits_[0] == BigDigit(0)); }
  bool isOne() const { return digits_.size() == 1 && digits_[0] == BigDigit(1); }
};

} // namespace bign

