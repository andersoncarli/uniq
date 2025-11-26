#pragma once
#include <variant>
#include <string>
#include <iostream>
#include <stdexcept>
#include "BigCardinal.h"
#include "BigInteger.h"
#include "BigDecimal.h"
#include "numtypes.h"

namespace bign {

// BigNumber - User-facing wrapper with type selection via std::variant
class BigNumber {
private:
  std::variant<BigCardinal, BigInteger, BigDecimal> value_;
  
  // Helper to get the underlying value
  template<typename T>
  T& getAs() {
    return std::get<T>(value_);
  }
  
  template<typename T>
  const T& getAs() const {
    return std::get<T>(value_);
  }

public:
  // Constructors - auto-detect type
  BigNumber() : value_(BigCardinal(0)) {}
  BigNumber(digit n) : value_(BigCardinal(n)) {}
  BigNumber(BigDigit d) : value_(BigCardinal(d)) {}  // Disambiguate BigDigit
  explicit BigNumber(i64 n) {
    if(n < 0) {
      BigInteger bi((digit)(-n));
      bi.signal = -1;
      value_ = bi;
    } else {
      value_ = BigCardinal((digit)n);
    }
  }
  BigNumber(const BigCardinal& c) : value_(c) {}
  BigNumber(const BigInteger& i) : value_(i) {}
  BigNumber(const BigDecimal& d) : value_(d) {}
  BigNumber(int n) : BigNumber((digit)n) {}  // Disambiguate int
  BigNumber(const std::string& s, int base=10, std::string map=DIGITS) {
    if(s.find('.') != std::string::npos) {
      value_ = BigDecimal(s);
    } else if(s.empty() || s[0] == '-') {
      value_ = BigInteger(s, base, map);
    } else {
      value_ = BigCardinal(s, base, map);
    }
  }
  
  // Copy and move constructors
  BigNumber(const BigNumber& other) = default;
  BigNumber(BigNumber&& other) noexcept = default;
  BigNumber& operator=(const BigNumber& other) = default;
  BigNumber& operator=(BigNumber&& other) noexcept = default;
  
  // Type queries
  bool isCardinal() const { return std::holds_alternative<BigCardinal>(value_); }
  bool isInteger() const { return std::holds_alternative<BigInteger>(value_); }
  bool isDecimal() const { return std::holds_alternative<BigDecimal>(value_); }
  
  // Type accessors
  BigCardinal asCardinal() const {
    if(isCardinal()) return std::get<BigCardinal>(value_);
    const auto& i = std::get<BigInteger>(value_);
    if(i.isNegative()) throw std::runtime_error("Cannot convert negative Integer to Cardinal");
    return BigCardinal(i);
  }
  
  BigInteger asInteger() const {
    if(isInteger()) return std::get<BigInteger>(value_);
    if(isDecimal()) {
      const auto& d = std::get<BigDecimal>(value_);
      return BigInteger(static_cast<const BigInteger&>(d));
    }
    return BigInteger(std::get<BigCardinal>(value_));
  }
  
  BigDecimal asDecimal() const {
    if(isDecimal()) return std::get<BigDecimal>(value_);
    if(isInteger()) return BigDecimal(std::get<BigInteger>(value_));
    return BigDecimal(std::get<BigCardinal>(value_));
  }
  
  // Operations - auto-promote types
  BigNumber& operator+=(const BigNumber& n) {
    *this = *this + n;
    return *this;
  }
  
  BigNumber operator+(const BigNumber& n) const {
    if(isDecimal() || n.isDecimal()) {
      BigDecimal a = asDecimal();
      BigDecimal b = n.asDecimal();
      return BigNumber(a + b);
    }
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a + b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a + b);
  }
  
  BigNumber& operator-=(const BigNumber& n) {
    *this = *this - n;
    return *this;
  }
  
  BigNumber operator-(const BigNumber& n) const {
    if(isDecimal() || n.isDecimal()) {
      BigDecimal a = asDecimal();
      BigDecimal b = n.asDecimal();
      return BigNumber(a - b);
    }
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a - b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    if(a < b) {
      // Promote to Integer for negative result
      BigInteger ai(a);
      BigInteger bi(b);
      return BigNumber(ai - bi);
    }
    return BigNumber(a - b);
  }
  
  BigNumber& operator*=(const BigNumber& n) {
    *this = *this * n;
    return *this;
  }
  
  BigNumber operator*(const BigNumber& n) const {
    if(isDecimal() || n.isDecimal()) {
      BigDecimal a = asDecimal();
      BigDecimal b = n.asDecimal();
      return BigNumber(a * b);
    }
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a * b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a * b);
  }
  
  BigNumber& operator/=(const BigNumber& n) {
    *this = *this / n;
    return *this;
  }
  
  BigNumber operator/(const BigNumber& n) const {
    if(isDecimal() || n.isDecimal()) {
      BigDecimal a = asDecimal();
      BigDecimal b = n.asDecimal();
      return BigNumber(a / b);
    }
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a / b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a / b);
  }
  
  BigNumber& operator%=(const BigNumber& n) {
    *this = *this % n;
    return *this;
  }
  
  BigNumber operator%(const BigNumber& n) const {
    // Modulo only makes sense for integers
    if(isDecimal() || n.isDecimal()) {
      // For decimals, convert to integer
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a % b);
    }
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a % b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a % b);
  }
  
  // Increment/Decrement
  BigNumber& operator++() {
    if(isDecimal()) {
      BigDecimal d = asDecimal();
      d += BigDecimal(1);
      value_ = d;
    } else if(isInteger()) {
      getAs<BigInteger>()++;
    } else {
      getAs<BigCardinal>()++;
    }
    return *this;
  }
  
  BigNumber operator++(int) {
    BigNumber tmp = *this;
    ++(*this);
    return tmp;
  }
  
  BigNumber& operator--() {
    if(isDecimal()) {
      BigDecimal d = asDecimal();
      d -= BigDecimal(1);
      value_ = d;
    } else if(isInteger()) {
      getAs<BigInteger>()--;
    } else {
      // Promote to Integer for negative results
      BigInteger i = asInteger();
      i--;
      value_ = i;
    }
    return *this;
  }
  
  BigNumber operator--(int) {
    BigNumber tmp = *this;
    --(*this);
    return tmp;
  }
  
  // Shift operations
  BigNumber operator<<(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a << b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a << b);
  }
  
  BigNumber operator>>(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a >> b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a >> b);
  }
  
  BigNumber& operator<<=(const BigNumber& n) {
    *this = *this << n;
    return *this;
  }
  
  BigNumber& operator>>=(const BigNumber& n) {
    *this = *this >> n;
    return *this;
  }
  
  BigNumber& operator>>=(int bits) {
    if(isInteger()) {
      getAs<BigInteger>().shiftRight(bits);
    } else {
      getAs<BigCardinal>().shiftRight(bits);
    }
    return *this;
  }
  
  // Bitwise operations
  BigNumber& operator&=(const BigNumber& n) {
    *this = *this & n;
    return *this;
  }
  
  BigNumber operator&(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a & b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a & b);
  }
  
  BigNumber& operator|=(const BigNumber& n) {
    *this = *this | n;
    return *this;
  }
  
  BigNumber operator|(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a | b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a | b);
  }
  
  BigNumber& operator^=(const BigNumber& n) {
    *this = *this ^ n;
    return *this;
  }
  
  BigNumber operator^(const BigNumber& n) const {
    if(isInteger() || n.isInteger()) {
      BigInteger a = asInteger();
      BigInteger b = n.asInteger();
      return BigNumber(a ^ b);
    }
    BigCardinal a = asCardinal();
    BigCardinal b = n.asCardinal();
    return BigNumber(a ^ b);
  }
  
  BigNumber operator~() const {
    if(isInteger()) {
      return BigNumber(~asInteger());
    }
    return BigNumber(~asCardinal());
  }
  
  // Comparisons
  bool operator==(const BigNumber& n) const {
    // Compare values regardless of type
    if(isZero() && n.isZero()) return true;
    if(isDecimal() || n.isDecimal()) {
      return asDecimal() == n.asDecimal();
    }
    if(isInteger() && n.isInteger()) {
      return asInteger() == n.asInteger();
    }
    if(isCardinal() && n.isCardinal()) {
      return asCardinal() == n.asCardinal();
    }
    // Cross-type comparison: compare as integers (more efficient than decimal)
    return asInteger() == n.asInteger();
  }
  
  bool operator<(const BigNumber& n) const {
    if(isDecimal() || n.isDecimal()) {
      return asDecimal() < n.asDecimal();
    }
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
  std::string format(int base=10, std::string map=DIGITS) const {
    if(isDecimal()) return asDecimal().format();
    if(isInteger()) return asInteger().format(base, map);
    return asCardinal().format(base, map);
  }
  
  // Utility
  int size() const {
    if(isDecimal()) return asDecimal().size();
    if(isInteger()) return asInteger().size();
    return asCardinal().size();
  }
  
  int bits() const {
    if(isDecimal()) {
      const auto& d = asDecimal();
      return static_cast<const BigInteger&>(d).bits();
    }
    if(isInteger()) return asInteger().bits();
    return asCardinal().bits();
  }
  
  bool isZero() const {
    if(isDecimal()) return asDecimal().isZero();
    if(isInteger()) return asInteger().isZero();
    return asCardinal().isZero();
  }
  
  bool isOne() const {
    if(isDecimal()) {
      const auto& d = asDecimal();
      return d == BigDecimal("1.0") || d == BigDecimal("1");
    }
    if(isInteger()) return asInteger().isOne();
    return asCardinal().isOne();
  }
  
  BigNumber negate() const {
    if(isDecimal()) {
      BigDecimal d = asDecimal();
      BigInteger negated = static_cast<const BigInteger&>(d).negate();
      return BigNumber(BigDecimal(negated, d.getDecimalPlaces()));
    }
    if(isInteger()) {
      BigInteger i = asInteger();
      BigInteger negated = i.negate();
      return BigNumber(negated);
    }
    // Promote Cardinal to Integer and negate
    BigInteger i = asInteger();
    BigInteger negated = i.negate();
    return BigNumber(negated);
  }
  
  BigNumber abs() const {
    if(isDecimal()) {
      BigDecimal d = asDecimal();
      if(d.isNegative()) {
        BigDecimal negated = static_cast<const BigInteger&>(d).abs();
        return BigNumber(BigDecimal(negated, d.getDecimalPlaces()));
      }
      return *this;
    }
    if(isInteger()) return BigNumber(asInteger().abs());
    return *this;  // Cardinal is already positive
  }
  
  int sign() const {
    if(isDecimal()) {
      const auto& d = asDecimal();
      return static_cast<const BigInteger&>(d).sign();
    }
    if(isInteger()) return asInteger().sign();
    if(isZero()) return 0;
    return 1;  // Cardinal is positive
  }
  
  bool isPositive() const {
    if(isDecimal()) {
      const auto& d = asDecimal();
      return !d.isZero() && static_cast<const BigInteger&>(d).isPositive();
    }
    if(isInteger()) return asInteger().isPositive();
    return !isZero();  // Cardinal is positive if non-zero
  }
  
  bool isNegative() const {
    if(isDecimal()) {
      const auto& d = asDecimal();
      return static_cast<const BigInteger&>(d).isNegative();
    }
    if(isInteger()) return asInteger().isNegative();
    return false;  // Cardinal is never negative
  }
};

// Utility function overloads for BigNumber
inline BigNumber pow(const BigNumber& base, const BigNumber& exp) {
  if(base.isInteger() || exp.isInteger()) {
    return BigNumber(pow(base.asInteger(), exp.asInteger()));
  }
  return BigNumber(pow(base.asCardinal(), exp.asCardinal()));
}

inline BigNumber gcd(const BigNumber& a, const BigNumber& b) {
  if(a.isInteger() || b.isInteger()) {
    return BigNumber(gcd(a.asInteger(), b.asInteger()));
  }
  return BigNumber(gcd(a.asCardinal(), b.asCardinal()));
}

inline BigNumber lcm(const BigNumber& a, const BigNumber& b) {
  if(a.isInteger() || b.isInteger()) {
    return BigNumber(lcm(a.asInteger(), b.asInteger()));
  }
  return BigNumber(lcm(a.asCardinal(), b.asCardinal()));
}

inline BigNumber sqrt(const BigNumber& n) {
  if(n.isInteger()) {
    return BigNumber(sqrt(n.asInteger()));
  }
  return BigNumber(sqrt(n.asCardinal()));
}

inline BigNumber mod_pow(const BigNumber& base, const BigNumber& exp, const BigNumber& mod) {
  if(base.isInteger() || exp.isInteger() || mod.isInteger()) {
    return BigNumber(mod_pow(base.asInteger(), exp.asInteger(), mod.asInteger()));
  }
  return BigNumber(mod_pow(base.asCardinal(), exp.asCardinal(), mod.asCardinal()));
}

std::ostream& operator<<(std::ostream& os, const BigNumber& n) {
  return os << n.format();
}

} // namespace bign
