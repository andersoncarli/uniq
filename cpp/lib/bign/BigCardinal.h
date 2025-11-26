#pragma once
#include "BigDigit.h"
#include "BigNumberCore.h"
#include <string>
#include <sstream>
#include <algorithm>
#include <cassert>
namespace bign {

class BigCardinal : public BigNumberCore {
protected:
  // Helper methods for backward compatibility (now use base class)
  void subtractAbs(const BigCardinal& n) {
    subtract(n);
  }

public:
  BigCardinal() : BigNumberCore() {}
  BigCardinal(digit n) : BigNumberCore() {
    digits_.clear();
    digits_.push_back(BigDigit(n));
  }
  BigCardinal(BigDigit n) : BigNumberCore() {
    digits_.clear();
    digits_.push_back(n);
  }
  BigCardinal(const BigCardinal& other) : BigNumberCore(other) {}
  BigCardinal(BigCardinal&& other) noexcept : BigNumberCore(std::move(other)) {}
  BigCardinal(const std::string s, int base=10, std::string map=DIGITS) : BigNumberCore() {
    assert(base>1 && base <= (int)map.length() && "BigCardinal() invalid base");
    digits_.clear();
    if(s.empty()) {
      digits_.push_back(BigDigit(0));
      return;
    }
    BigDigit d(s, base, [&](digit ov, DigitOp /*op*/)->digit { 
      digits_.push_back(ov); 
      return 0;
    }, map);
    digits_.push_back(d);
  }

  inline int size() const { return BigNumberCore::size(); }
  int bits() const { 
    if(digits_.empty()) return 0;
    int r=digits_.back().size();
    int s=digits_.size()-1;
    return s ? r + (s*BITS(digit)) : r;
  }

  std::string format(int base=10, std::string map=DIGITS) const {
    assert(base>1 && base <= (int)map.length() && "format() invalid base");
    if(digits_.empty() || isZero()) return std::string(1, map[0]);
    std::ostringstream ss;
    for(int i=0; i < (int)digits_.size(); i++)
      ss << digits_[i].format(base, map);
    std::string result = ss.str();
    while(result.length() > 1 && result[0] == map[0]) {
      result = result.substr(1);
    }
    return result;
  }

  // Arithmetic operations (unsigned) ==========================================
  BigCardinal& operator += (const BigCardinal& n) { 
    if(n.isZero()) return *this;
    if(isZero()) { *this = n; return *this; }
    add(n);
    return *this;
  }

  BigCardinal operator + (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r += n; 
  }

  BigCardinal& operator ++ () { 
    BigCardinal one(1);
    add(one);
    return *this; 
  }

  BigCardinal operator ++ (int) { 
    BigCardinal tmp = *this; 
    ++(*this);
    return tmp; 
  }

  BigCardinal& operator -= (const BigCardinal& n) {
    if(n.isZero()) return *this;
    if(compare(n) < 0) throw FlowException(1, SUB);
    subtract(n);
    return *this;
  }

  BigCardinal operator - (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r -= n; 
  }

  BigCardinal& operator -- () { 
    if(isZero()) throw FlowException(1, SUB);
    BigCardinal one(1);
    subtract(one);
    return *this; 
  }

  BigCardinal operator -- (int) { 
    BigCardinal tmp = *this; 
    --(*this);
    return tmp; 
  }

  BigCardinal& operator *= (const BigCardinal& n) {
    if(n.isZero() || isZero()) {
      *this = BigCardinal(0);
      return *this;
    }
    if(n.isOne()) {
      return *this;
    }
    multiply(n);
    return *this;
  }

  BigCardinal operator * (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r *= n; 
  }

  // Helper: Get digit at position (0 = least significant, size()-1 = most significant)
  digit getDigitAt(int pos) const {
    if(pos < 0 || pos >= size()) return 0;
    return digits_[pos].value;
  }
  
  std::pair<BigCardinal, BigCardinal> divide(const BigCardinal& n) const {
    BigCardinal quotient, remainder;
    BigCardinal temp = *this;
    temp.BigNumberCore::divide(n, quotient, remainder);
    return std::make_pair(quotient, remainder);
  }

  BigCardinal& operator /= (const BigCardinal& n) {
    *this = divide(n).first;
    return *this;
  }

  BigCardinal operator / (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r /= n; 
  }

  BigCardinal& operator %= (const BigCardinal& n) {
    *this = divide(n).second;
    return *this;
  }

  BigCardinal operator % (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r %= n; 
  }

  // Shift operations ==========================================================
  BigCardinal& operator <<= (const BigCardinal& n) {
    if(n.isZero() || isZero()) return *this;
    if(n.bits() > 1000) throw FlowException(1, SHL);
    
    digit shift_bits = 0;
    if(n.size() == 1 && n.digits_[0] < BigDigit(BITS(digit))) {
      shift_bits = n.digits_[0].value;
    } else {
      BigCardinal power_of_2(1);
      for(BigCardinal i(0); i < n; ++i) {
        power_of_2 = power_of_2 * BigCardinal(2);
        if(power_of_2.bits() > bits() + 1000) throw FlowException(1, SHL);
      }
      *this = *this * power_of_2;
      return *this;
    }
    
    if(shift_bits == 0) return *this;
    shiftLeft((int)shift_bits);
    return *this;
  }

  BigCardinal operator << (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r <<= n; 
  }

  BigCardinal& operator >>= (const BigCardinal& n) {
    if(n.isZero() || isZero()) return *this;
    
    digit shift_bits = 0;
    if(n.size() == 1 && n.digits_[0] < BigDigit(BITS(digit))) {
      shift_bits = n.digits_[0].value;
    } else {
      BigCardinal power_of_2(1);
      for(BigCardinal i(0); i < n; ++i) {
        power_of_2 = power_of_2 * BigCardinal(2);
      }
      *this = *this / power_of_2;
      return *this;
    }
    
    if(shift_bits == 0) return *this;
    shiftRight((int)shift_bits);
    return *this;
  }

  BigCardinal operator >> (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r >>= n; 
  }

  // Bitwise operations ========================================================
  BigCardinal& operator &= (const BigCardinal& n) {
    bitwiseAnd(n);
    return *this;
  }

  BigCardinal operator & (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r &= n; 
  }

  BigCardinal& operator |= (const BigCardinal& n) {
    bitwiseOr(n);
    return *this;
  }

  BigCardinal operator | (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r |= n; 
  }

  BigCardinal& operator ^= (const BigCardinal& n) {
    bitwiseXor(n);
    return *this;
  }

  BigCardinal operator ^ (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r ^= n; 
  }

  BigCardinal operator ~ () const {
    BigCardinal r(*this);
    r.bitwiseNot();
    return r;
  }

  // Comparisons (unsigned) ===================================================
  bool operator == (const BigCardinal& n) const { 
    return compare(n) == 0;
  }
  bool operator == (digit d) const { return *this == BigCardinal(d); }
  bool operator != (const BigCardinal& n) const { return !(*this == n); }
  bool operator != (digit d) const { return *this != BigCardinal(d); }
  bool operator <  (const BigCardinal& n) const {
    return compare(n) < 0;
  }
  bool operator <  (digit d) const { return *this < BigCardinal(d); }
  bool operator >  (const BigCardinal& n) const { return n < *this; }
  bool operator >  (digit d) const { return BigCardinal(d) < *this; }
  bool operator <= (const BigCardinal& n) const { return !(n < *this); }
  bool operator <= (digit d) const { return !(BigCardinal(d) < *this); }
  bool operator >= (const BigCardinal& n) const { return !(*this < n); }
  bool operator >= (digit d) const { return !(*this < BigCardinal(d)); }

  BigDigit operator [](const int i) const { return digits_[i]; }

  BigCardinal& operator = (const BigCardinal& n) { 
    BigNumberCore::operator=(n);
    return *this;
  }

  BigCardinal& operator = (const digit d) { 
    digits_.clear(); 
    digits_.push_back(BigDigit(d)); 
    return *this;
  }

  BigCardinal& operator = (BigCardinal&& other) noexcept {
    if(this != &other) {
      BigNumberCore::operator=(std::move(other));
    }
    return *this;
  }

  bool isZero() const { return BigNumberCore::isZero(); }
  bool isOne() const { return BigNumberCore::isOne(); }
};

std::ostream& operator<<(std::ostream& os, const BigCardinal& n) { return os << n.format(); }

// Utility functions (unsigned) ===============================================
BigCardinal pow(const BigCardinal& base, const BigCardinal& exp) {
  if(exp.isZero()) return BigCardinal(1);
  if(exp.isOne()) return base;
  if(exp % BigCardinal(2) == BigCardinal(0)) {
    BigCardinal half = pow(base, exp / BigCardinal(2));
    return half * half;
  }
  return base * pow(base, exp - BigCardinal(1));
}

BigCardinal gcd(const BigCardinal& a, const BigCardinal& b) {
  if(b.isZero()) return a;
  return gcd(b, a % b);
}

BigCardinal lcm(const BigCardinal& a, const BigCardinal& b) {
  return (a * b) / gcd(a, b);
}

BigCardinal sqrt(const BigCardinal& n) {
  if(n.isZero()) return BigCardinal(0);
  if(n.isOne()) return BigCardinal(1);
  
  BigCardinal low(1), high(n), result(0);
  while(low <= high) {
    BigCardinal mid = (low + high) / BigCardinal(2);
    BigCardinal square = mid * mid;
    if(square == n) return mid;
    if(square < n) {
      result = mid;
      low = mid + BigCardinal(1);
    } else {
      high = mid - BigCardinal(1);
    }
  }
  return result;
}

BigCardinal mod_pow(const BigCardinal& base, const BigCardinal& exp, const BigCardinal& mod) {
  if(mod.isZero()) throw FlowException(1, MOD);
  if(mod.isOne()) return BigCardinal(0);
  if(exp.isZero()) return BigCardinal(1);
  if(exp.isOne()) return base % mod;
  
  BigCardinal result(1);
  BigCardinal b = base % mod;
  BigCardinal e = exp;
  
  while(!e.isZero()) {
    if((e % BigCardinal(2)) == BigCardinal(1)) {
      result = (result * b) % mod;
    }
    e = e / BigCardinal(2);
    b = (b * b) % mod;
  }
  return result;
}

} // namespace bign

