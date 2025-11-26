#pragma once
#include <variant>
#include <string>
#include <iostream>
#include <stdexcept>
#include "BigCardinal.h"
#include "BigInteger.h"
// TODO: BigDecimal when implemented
// #include "BigDecimal.h"

namespace bign {

// BigNumber - User-facing wrapper with type selection via std::variant
// Provides auto-promotion and type flexibility
class BigNumber {
private:
  std::variant<BigCardinal, BigInteger> value_;
  // TODO: Add BigDecimal when implemented
  // std::variant<BigCardinal, BigInteger, BigDecimal> value_;
  
  // Helper to get active type
  template<typename T>
  bool holds() const {
    return std::holds_alternative<T>(value_);
  }
  
  template<typename T>
  T& get() {
    return std::get<T>(value_);
  }
  
  template<typename T>
  const T& get() const {
    return std::get<T>(value_);
  }
  
public:
  // Constructors - auto-detect type
  BigNumber() : value_(BigCardinal(0)) {}
  BigNumber(digit n) : value_(BigCardinal(n)) {}
  BigNumber(i64 n) {
    if(n < 0) {
      value_ = BigInteger(static_cast<digit>(-n));
      std::get<BigInteger>(value_).signal = -1;
    } else {
      value_ = BigCardinal(static_cast<digit>(n));
    }
  }
  BigNumber(const std::string& s) {
    // Check if it's a decimal (has '.')
    if(s.find('.') != std::string::npos) {
      // TODO: Create BigDecimal when implemented
      // For now, treat as integer
      value_ = BigInteger(s);
    } else if(s[0] == '-') {
      value_ = BigInteger(s);
    } else {
      value_ = BigCardinal(s);
    }
  }
  
  // Type-specific constructors
  BigNumber(const BigCardinal& c) : value_(c) {}
  BigNumber(const BigInteger& i) : value_(i) {}
  
  // Copy and move
  BigNumber(const BigNumber& other) : value_(other.value_) {}
  BigNumber(BigNumber&& other) noexcept : value_(std::move(other.value_)) {}
  
  BigNumber& operator=(const BigNumber& other) {
    value_ = other.value_;
    return *this;
  }
  
  BigNumber& operator=(BigNumber&& other) noexcept {
    value_ = std::move(other.value_);
    return *this;
  }
  
  // Type queries
  bool isCardinal() const { return holds<BigCardinal>(); }
  bool isInteger() const { return holds<BigInteger>(); }
  
  // Type accessors (with promotion)
  BigCardinal asCardinal() const {
    if(isCardinal()) return get<BigCardinal>();
    if(isInteger()) {
      const auto& i = get<BigInteger>();
      if(i.sign() >= 0) return BigCardinal(i);
      throw std::runtime_error("Cannot convert negative integer to cardinal");
    }
    throw std::runtime_error("Unknown type");
  }
  
  BigInteger asInteger() const {
    if(isInteger()) return get<BigInteger>();
    if(isCardinal()) return BigInteger(get<BigCardinal>());
    throw std::runtime_error("Unknown type");
  }
  
  // Operations with auto-promotion
  BigNumber operator+(const BigNumber& n) const {
    // Promote to Integer if either is Integer
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a + b);
    }
    // Both Cardinal
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a + b);
  }
  
  BigNumber operator-(const BigNumber& n) const {
    // Always use Integer for subtraction (can result in negative)
    BigInteger a = asInteger();
    BigInteger b = n.asInteger();
    return BigNumber(a - b);
  }
  
  BigNumber operator*(const BigNumber& n) const {
    // Promote to Integer if either is Integer
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a * b);
    }
    // Both Cardinal
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a * b);
  }
  
  BigNumber operator/(const BigNumber& n) const {
    // Promote to Integer if either is Integer
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a / b);
    }
    // Both Cardinal
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a / b);
  }
  
  BigNumber operator%(const BigNumber& n) const {
    // Promote to Integer if either is Integer
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a % b);
    }
    // Both Cardinal
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a % b);
  }
  
  // Comparisons
  bool operator==(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      return asInteger() == n.asInteger();
    }
    return asCardinal() == n.asCardinal();
  }
  
  bool operator<(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      return asInteger() < n.asInteger();
    }
    return asCardinal() < n.asCardinal();
  }
  
  bool operator>(const BigNumber& n) const { return n < *this; }
  bool operator<=(const BigNumber& n) const { return !(n < *this); }
  bool operator>=(const BigNumber& n) const { return !(*this < n); }
  bool operator!=(const BigNumber& n) const { return !(*this == n); }
  
  // Format
  std::string format(int base = 10) const {
    if(isInteger()) return get<BigInteger>().format(base);
    return get<BigCardinal>().format(base);
  }
  
  // Utility
  bool isZero() const {
    if(isInteger()) return get<BigInteger>().isZero();
    return get<BigCardinal>().isZero();
  }
  
  bool isOne() const {
    if(isInteger()) return get<BigInteger>().isOne();
    return get<BigCardinal>().isOne();
  }
};

std::ostream& operator<<(std::ostream& os, const BigNumber& n) {
  return os << n.format();
}

} // namespace bign
