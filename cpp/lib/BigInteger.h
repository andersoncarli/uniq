#pragma once
#include "BigCardinal.h"
namespace uniq {

class BigInteger : public BigCardinal {
  i8 signal;
  
public:
  BigInteger() : BigCardinal(), signal(1) {}
  BigInteger(digit n) : BigCardinal(n), signal(1) {}
  BigInteger(BigDigit n) : BigCardinal(n), signal(1) {}
  BigInteger(const BigCardinal& c) : BigCardinal(c), signal(1) {}
  BigInteger(const BigInteger& other) : BigCardinal(other), signal(other.signal) {}
  BigInteger(BigInteger&& other) noexcept : BigCardinal(move(other)), signal(other.signal) {
    other.signal = 1;
  }
  BigInteger(const string s, int base=10, string map=DIGITS) : BigCardinal(), signal(1) {
    assert(base>1 && base <= (int)map.length() && "BigInteger() invalid base");
    if(s.empty() || (s.length() == 1 && s[0] == '-')) {
      digits.clear();
      digits.push_back(BigDigit(0));
      signal = 1;
      return;
    }
    bool negative = (s[0] == '-');
    string num_str = negative ? s.substr(1) : s;
    if(num_str.empty()) {
      digits.clear();
      digits.push_back(BigDigit(0));
      signal = 1;
      return;
    }
    digits.clear(); // Clear the default zero from BigCardinal()
    BigDigit d(num_str, base, [&](digit ov, DigitOp op)->digit { 
      digits.push_back(ov); 
      return 0;
    }, map);
    digits.push_back(d);
    signal = negative ? -1 : 1;
  }

  string format(int base=10, string map=DIGITS) const {
    assert(base>1 && base <= (int)map.length() && "format() invalid base");
    if(digits.empty() || isZero()) return string(1, map[0]);
    ostringstream ss;
    if(signal < 0) ss << "-";
    for(int i=0; i < digits.size(); i++)
      ss << digits[i].format(base, map);
    string result = ss.str();
    while(result.length() > 1 && result[0] == map[0]) {
      result = result.substr(1);
    }
    if(result == "-" + string(1, map[0])) result = string(1, map[0]);
    return result;
  }
  
  // Sign helper functions ====================================================
  int sign() const { 
    if(isZero()) return 0;
    return signal > 0 ? 1 : -1;
  }
  
  bool isPositive() const { return signal > 0 && !isZero(); }
  bool isNegative() const { return signal < 0; }
  
  BigInteger abs() const {
    BigInteger r(*this);
    r.signal = 1;
    return r;
  }
  
  BigInteger negate() const {
    BigInteger r(*this);
    if(!isZero()) r.signal = -r.signal;
    return r;
  }

  // Arithmetic operations (signed) ============================================
  BigInteger& operator += (const BigInteger& n) {
    if(n.isZero()) return *this;
    if(isZero()) { *this = n; return *this; }
    
    if(signal == n.signal) {
      static_cast<BigCardinal&>(*this) += static_cast<const BigCardinal&>(n);
      return *this;
    }
    
    i8 original_signal = signal;
    BigCardinal abs_this = abs();
    BigCardinal abs_n = n.abs();
    if(abs_this >= abs_n) {
      *this = abs_this;
      subtractAbs(abs_n);
      signal = original_signal;
    } else {
      *this = abs_n;
      subtractAbs(abs_this);
      signal = n.signal;
    }
    if(isZero()) signal = 1;
    return *this;
  }

  BigInteger operator + (const BigInteger& n) const { 
    BigInteger r(*this); 
    return r += n; 
  }

  BigInteger& operator ++ () { 
    BigInteger one(1);
    *this += one;
    return *this; 
  }

  BigInteger operator ++ (int) { 
    BigInteger tmp = *this; 
    ++(*this);
    return tmp; 
  }

  BigInteger& operator -= (const BigInteger& n) {
    if(n.isZero()) return *this;
    if(isZero()) { *this = n.negate(); return *this; }
    
    if(signal != n.signal) {
      i8 original_signal = signal;
      BigCardinal abs_this = abs();
      BigCardinal abs_n = n.abs();
      static_cast<BigCardinal&>(*this) = abs_this + abs_n;
      signal = original_signal;
      return *this;
    }
    
    i8 original_signal = signal;
    BigCardinal abs_this = abs();
    BigCardinal abs_n = n.abs();
    if(abs_this >= abs_n) {
      *this = abs_this;
      subtractAbs(abs_n);
      signal = original_signal;
    } else {
      *this = abs_n;
      subtractAbs(abs_this);
      signal = -original_signal;
    }
    if(isZero()) signal = 1;
    return *this;
  }

  BigInteger operator - (const BigInteger& n) const { 
    BigInteger r(*this); 
    return r -= n; 
  }

  BigInteger& operator -- () { 
    BigInteger one(1);
    *this -= one;
    return *this; 
  }

  BigInteger operator -- (int) { 
    BigInteger tmp = *this; 
    --(*this);
    return tmp; 
  }

  BigInteger& operator *= (const BigInteger& n) {
    if(n.isZero() || isZero()) {
      *this = BigInteger(0);
      return *this;
    }
    if(n.isOne()) {
      return *this;
    }
    if(n == BigInteger("-1")) {
      signal = -signal;
      return *this;
    }
    
    i8 result_signal = (signal * n.signal > 0) ? 1 : -1;
    BigCardinal abs_this = abs();
    BigCardinal abs_n = n.abs();
    
    static_cast<BigCardinal&>(*this) = abs_this;
    static_cast<BigCardinal&>(*this) *= abs_n;
    signal = result_signal;
    return *this;
  }

  BigInteger operator * (const BigInteger& n) const { 
    BigInteger r(*this); 
    return r *= n; 
  }

  pair<BigInteger, BigInteger> divide(const BigInteger& n) const {
    if(n.isZero()) throw FlowException(1, DIV);
    if(isZero()) return make_pair(BigInteger(0), BigInteger(0));
    if(n.abs() == BigCardinal(1)) {
      BigInteger result(*this);
      result.signal = signal * n.signal;
      return make_pair(result, BigInteger(0));
    }
    
    i8 quotient_signal = (signal * n.signal > 0) ? 1 : -1;
    i8 remainder_signal = signal;
    
    BigCardinal abs_this = abs();
    BigCardinal abs_n = n.abs();
    
    if(abs_this < abs_n) {
      BigInteger remainder(*this);
      return make_pair(BigInteger(0), remainder);
    }
    
    auto [quotient_card, remainder_card] = abs_this.divide(abs_n);
    BigInteger quotient(quotient_card);
    BigInteger remainder(remainder_card);
    
    quotient.signal = quotient_signal;
    remainder.signal = remainder_signal;
    if(remainder.isZero()) remainder.signal = 1;
    
    return make_pair(quotient, remainder);
  }

  BigInteger& operator /= (const BigInteger& n) {
    *this = divide(n).first;
    return *this;
  }

  BigInteger operator / (const BigInteger& n) const { 
    BigInteger r(*this); 
    return r /= n; 
  }

  BigInteger& operator %= (const BigInteger& n) {
    *this = divide(n).second;
    return *this;
  }

  BigInteger operator % (const BigInteger& n) const { 
    BigInteger r(*this); 
    return r %= n; 
  }

  // Shift operations (inherit from Cardinal, no sign changes) ===================
  using BigCardinal::operator <<=;
  using BigCardinal::operator <<;
  using BigCardinal::operator >>=;
  using BigCardinal::operator >>;

  // Bitwise operations (inherit from Cardinal) ================================
  using BigCardinal::operator &=;
  using BigCardinal::operator &;
  using BigCardinal::operator |=;
  using BigCardinal::operator |;
  using BigCardinal::operator ^=;
  using BigCardinal::operator ^;
  using BigCardinal::operator ~;

  // Comparisons (signed) =====================================================
  bool operator == (const BigInteger& n) const { 
    if(isZero() && n.isZero()) return true;
    if(signal != n.signal) return false;
    return static_cast<const BigCardinal&>(*this) == static_cast<const BigCardinal&>(n);
  }
  bool operator == (digit d) const { return *this == BigInteger(d); }
  bool operator != (const BigInteger& n) const { return !(*this == n); }
  bool operator != (digit d) const { return *this != BigInteger(d); }
  bool operator <  (const BigInteger& n) const {
    if(signal < 0 && n.signal > 0) return true;
    if(signal > 0 && n.signal < 0) return false;
    if(signal == n.signal && signal < 0) {
      // Both negative: larger absolute value is smaller
      return static_cast<const BigCardinal&>(n) < static_cast<const BigCardinal&>(*this);
    }
    // Both positive or zero: compare normally
    return static_cast<const BigCardinal&>(*this) < static_cast<const BigCardinal&>(n);
  }
  bool operator <  (digit d) const { return *this < BigInteger(d); }
  bool operator >  (const BigInteger& n) const { return n < *this; }
  bool operator >  (digit d) const { return BigInteger(d) < *this; }
  bool operator <= (const BigInteger& n) const { return !(n < *this); }
  bool operator <= (digit d) const { return !(BigInteger(d) < *this); }
  bool operator >= (const BigInteger& n) const { return !(*this < n); }
  bool operator >= (digit d) const { return !(*this < BigInteger(d)); }

  BigInteger& operator = (const BigInteger& n) { 
    static_cast<BigCardinal&>(*this) = static_cast<const BigCardinal&>(n);
    signal = n.signal; 
    return *this;
  }

  BigInteger& operator = (const digit d) { 
    static_cast<BigCardinal&>(*this) = d;
    signal = 1; 
    return *this;
  }

  BigInteger& operator = (BigInteger&& other) noexcept {
    if(this != &other) {
      static_cast<BigCardinal&>(*this) = move(static_cast<BigCardinal&>(other));
      signal = other.signal;
      other.signal = 1;
    }
    return *this;
  }
};

ostream& operator<<(ostream& os, const BigInteger& n) { return os << n.format(); }

// Utility functions (signed) ==================================================
BigInteger pow(const BigInteger& base, const BigInteger& exp) {
  if(exp.isZero()) return BigInteger(1);
  if(exp.isOne()) return base;
  if(exp.abs() % BigCardinal(2) == BigCardinal(0)) {
    BigInteger half = pow(base, exp.abs() / BigCardinal(2));
    return half * half;
  }
  return base * pow(base, exp.abs() - BigCardinal(1));
}

BigInteger gcd(const BigInteger& a, const BigInteger& b) {
  if(b.isZero()) return a.abs();
  return gcd(b.abs(), (a % b).abs());
}

BigInteger lcm(const BigInteger& a, const BigInteger& b) {
  return (a.abs() * b.abs()) / gcd(a, b);
}

BigInteger sqrt(const BigInteger& n) {
  if(n.isZero()) return BigInteger(0);
  if(n.isOne()) return BigInteger(1);
  if(n.isNegative()) throw FlowException(1, DIV);
  
  BigCardinal result = sqrt(n.abs());
  return BigInteger(result);
}

BigInteger mod_pow(const BigInteger& base, const BigInteger& exp, const BigInteger& mod) {
  if(mod.isZero()) throw FlowException(1, MOD);
  BigCardinal result = mod_pow(base.abs(), exp.abs(), mod.abs());
  // Sign handling for mod_pow with negative base/exp is complex, using abs for now
  return BigInteger(result);
}

} // namespace uniq

