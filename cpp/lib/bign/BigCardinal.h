#pragma once
#include "BigDigit.h"
#include "BigNumberCore.h"
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <utility>

namespace bign {

// Forward declaration for friend
class BigNumberNaive;

class BigCardinal : public BigNumberCore {
  friend class BigNumberNaive;
protected:
  using BigNumberCore::digits_;
  
  void add(digit d, int i=0, const char* msg=""){ 
    if(i >= digits_.size()) {
      digits_.push_back(BigDigit(d));
      return;
    }
    digit ov=0;
    digits_[i].value = digits_[i].add(d, ov);
    if(ov) add(ov, i+1, msg);
  }
  
  void subtractAbs(const BigCardinal& n) {
    for (int i=0; i < n.size(); i++) {
      if(i >= digits_.size()) {
        throw FlowException(1, SUB);
      }
      digit borrow = 0;
      digits_[i].value = digits_[i].sub(n.digits_[i].value, borrow);
      if(borrow) {
        if(i+1 >= digits_.size()) throw FlowException(borrow, SUB);
        subtractAbsDigit(BigDigit(borrow), i+1);
      }
    }
    while(digits_.size() > 1 && digits_.back() == BigDigit(0)) digits_.pop_back();
  }
  
  void subtractAbsDigit(BigDigit d, int i=0) {
    if(i >= digits_.size()) throw FlowException(1, SUB);
    digit borrow = 0;
    digits_[i].value = digits_[i].sub(d.value, borrow);
    if(borrow) {
      if(i+1 >= digits_.size()) throw FlowException(borrow, SUB);
      subtractAbsDigit(BigDigit(borrow), i+1);
    }
  }

public:
  BigCardinal() : BigNumberCore() {
    digits_.clear();
    digits_.push_back(BigDigit(0));
  }
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
    if(s.empty()) {
      digits_.push_back(BigDigit(0));
      return;
    }
    BigDigit d(s, base, [&](digit ov, DigitOp op)->digit { 
      digits_.push_back(ov); 
      return 0;
    }, map);
    digits_.push_back(d);
  }

  inline int size() const { return digits_.size(); }
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
    for(int i=0; i < digits_.size(); i++)
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
    for (int i=0; i < n.size(); i++) 
      add(n.digits_[i].value, i);
    return *this;
  }

  BigCardinal operator + (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r += n; 
  }

  BigCardinal& operator ++ () { 
    add(1); 
    return *this; 
  }

  BigCardinal operator ++ (int) { 
    BigCardinal tmp = *this; 
    ++(*this);
    return tmp; 
  }

  BigCardinal& operator -= (const BigCardinal& n) {
    if(n.isZero()) return *this;
    if(*this < n) throw FlowException(1, SUB);
    subtractAbs(n);
    return *this;
  }

  BigCardinal operator - (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r -= n; 
  }

  BigCardinal& operator -- () { 
    if(isZero()) throw FlowException(1, SUB);
    subtractAbsDigit(BigDigit(1), 0);
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
    
    BigCardinal product(0);
    product.digits_.resize(size() + n.size(), BigDigit(0));
    
    for(int bi = 0; bi < n.size(); bi++) {
      digit carry = 0;
      for(int ai = 0; ai < size(); ai++) {
        digit prod_ov = 0;
        digit prod = digits_[ai].mul(n.digits_[bi].value, prod_ov);
        uoverflow total = (uoverflow)prod + (uoverflow)carry;
        digit total_digit = (digit)total;
        digit total_ov = total >> BITS(digit);
        digit sum_ov = 0;
        digit sum = product.digits_[ai + bi].add(total_digit, sum_ov);
        product.digits_[ai + bi].value = sum;
        carry = prod_ov + total_ov + sum_ov;
      }
      if(carry) {
        int final_idx = bi + size();
        if(final_idx >= product.size()) {
          product.digits_.push_back(BigDigit(carry));
        } else {
          product.add(carry, final_idx);
        }
      }
    }
    while(product.digits_.size() > 1 && product.digits_.back() == BigDigit(0)) 
      product.digits_.pop_back();
    *this = product;
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
    if(n.isZero()) throw FlowException(1, DIV);
    if(isZero()) return std::make_pair(BigCardinal(0), BigCardinal(0));
    if(n.isOne()) return std::make_pair(*this, BigCardinal(0));
    
    if(*this < n) {
      return std::make_pair(BigCardinal(0), *this);
    }
    
    // Long division: work from most significant to least significant
    // digits_[0] = LSB, digits_[size()-1] = MSB (little-endian)
    BigCardinal remainder = *this;
    int div_size = n.size();
    
    // Quotient will have at most (remainder.size() - div_size + 1) digits
    int quot_size = remainder.size() - div_size + 1;
    if(quot_size <= 0) quot_size = 1;
    
    BigCardinal quotient(0);
    quotient.digits_.resize(quot_size, BigDigit(0));
    
    // Process each quotient digit from MSB (quot_size-1) to LSB (0)
    for(int q_idx = quot_size - 1; q_idx >= 0; q_idx--) {
      // Get the part of remainder we're working with
      // We need to look at digits starting from q_idx
      int rem_start = q_idx;
      int rem_end = std::min(rem_start + div_size + 1, (int)remainder.size());
      
      if(rem_end <= rem_start) continue;
      
      // Extract the window of remainder digits (MSB aligned)
      BigCardinal rem_part(0);
      rem_part.digits_.resize(rem_end - rem_start, BigDigit(0));
      for(int i = rem_start; i < rem_end; i++) {
        rem_part.digits_[i - rem_start].value = remainder.digits_[i].value;
      }
      
      // Compare: if rem_part < n, this quotient digit is 0
      if(rem_part < n) continue;
      
      // Estimate quotient digit using top 2 digits
      digit div_top = n.digits_[div_size - 1].value;
      if(div_top == 0 && div_size > 1) div_top = n.digits_[div_size - 2].value;
      
      int rem_part_size = rem_part.size();
      uoverflow rem_top = 0;
      if(rem_part_size > 0) {
        rem_top = rem_part.digits_[rem_part_size - 1].value;
        if(rem_part_size > 1) {
          rem_top = (rem_top << BITS(digit)) | rem_part.digits_[rem_part_size - 2].value;
        }
      }
      
      if(div_top == 0) continue;
      
      // Estimate quotient digit
      // Use simple division for estimation, then refine
      uoverflow q_est = rem_top / (uoverflow)div_top;
      digit max_digit = (digit)-1;
      if(q_est > (uoverflow)max_digit) q_est = max_digit;
      digit q_digit = (digit)q_est;
      
      if(q_digit == 0) continue;
      
      // Try different values of q_digit until we find one that works
      bool success = false;
      for(int attempt = 0; attempt < 10 && !success; attempt++) {
        digit test_q = q_digit;
        if(attempt > 0 && attempt % 2 == 1 && test_q < max_digit) {
          test_q = q_digit + (attempt + 1) / 2;
        } else if(attempt > 0 && attempt % 2 == 0 && test_q > 0) {
          test_q = q_digit - attempt / 2;
        }
        if(test_q == 0) break;
        
        // Multiply n by test_q
        BigCardinal product(0);
        product.digits_.resize(div_size + 1, BigDigit(0));
        
        digit carry = 0;
        for(int i = 0; i < div_size; i++) {
          uoverflow prod = (uoverflow)n.digits_[i].value * (uoverflow)test_q;
          digit prod_digit = (digit)prod;
          digit prod_ov = prod >> BITS(digit);
          uoverflow total = (uoverflow)prod_digit + (uoverflow)carry;
          digit total_digit = (digit)total;
          digit total_ov = total >> BITS(digit);
          product.digits_[i].value = total_digit;
          carry = prod_ov + total_ov;
        }
        if(carry) product.digits_[div_size].value = carry;
        while(product.digits_.size() > 1 && product.digits_.back() == BigDigit(0)) 
          product.digits_.pop_back();
        
        // Shift product left by q_idx positions
        if(q_idx > 0) {
          product.digits_.insert(product.digits_.begin(), q_idx, BigDigit(0));
        }
        
        // Check if we can subtract
        if(remainder >= product) {
          remainder.subtractAbs(product);
          quotient.digits_[q_idx].value = test_q;
          success = true;
          break;
        }
      }
    }
    
    // Remove leading zeros
    while(quotient.digits_.size() > 1 && quotient.digits_.back() == BigDigit(0)) 
      quotient.digits_.pop_back();
    while(remainder.digits_.size() > 1 && remainder.digits_.back() == BigDigit(0)) 
      remainder.digits_.pop_back();
    
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
    
    digit carry = 0;
    for(int i = 0; i < size(); i++) {
      digit ov = 0;
      digit shifted = digits_[i].shl(shift_bits, ov);
      digits_[i].value = shifted | carry;
      carry = ov;
    }
    if(carry) digits_.push_back(BigDigit(carry));
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
    
    for(int i = size() - 1; i >= 0; i--) {
      digit ov = 0;
      digits_[i].value = digits_[i].shr(shift_bits, ov);
      if(i > 0) {
        digit carry_bits = ov << (BITS(digit) - shift_bits);
        digits_[i-1].value |= carry_bits;
      }
    }
    while(digits_.size() > 1 && digits_.back() == BigDigit(0)) digits_.pop_back();
    return *this;
  }

  BigCardinal operator >> (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r >>= n; 
  }

  // Bitwise operations ========================================================
  BigCardinal& operator &= (const BigCardinal& n) {
    int max_size = size() > n.size() ? size() : n.size();
    digits_.resize(max_size, BigDigit(0));
    for(int i = 0; i < max_size; i++) {
      digit val1 = i < size() ? digits_[i].value : 0;
      digit val2 = i < n.size() ? n.digits_[i].value : 0;
      digits_[i].value = val1 & val2;
    }
    while(digits_.size() > 1 && digits_.back() == BigDigit(0)) digits_.pop_back();
    return *this;
  }

  BigCardinal operator & (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r &= n; 
  }

  BigCardinal& operator |= (const BigCardinal& n) {
    int max_size = size() > n.size() ? size() : n.size();
    digits_.resize(max_size, BigDigit(0));
    for(int i = 0; i < max_size; i++) {
      digit val1 = i < size() ? digits_[i].value : 0;
      digit val2 = i < n.size() ? n.digits_[i].value : 0;
      digits_[i].value = val1 | val2;
    }
    while(digits_.size() > 1 && digits_.back() == BigDigit(0)) digits_.pop_back();
    return *this;
  }

  BigCardinal operator | (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r |= n; 
  }

  BigCardinal& operator ^= (const BigCardinal& n) {
    int max_size = size() > n.size() ? size() : n.size();
    digits_.resize(max_size, BigDigit(0));
    for(int i = 0; i < max_size; i++) {
      digit val1 = i < size() ? digits_[i].value : 0;
      digit val2 = i < n.size() ? n.digits_[i].value : 0;
      digits_[i].value = val1 ^ val2;
    }
    while(digits_.size() > 1 && digits_.back() == BigDigit(0)) digits_.pop_back();
    return *this;
  }

  BigCardinal operator ^ (const BigCardinal& n) const { 
    BigCardinal r(*this); 
    return r ^= n; 
  }

  BigCardinal operator ~ () const {
    BigCardinal r(*this);
    for(int i = 0; i < r.size(); i++) {
      r.digits_[i].value = ~r.digits_[i].value;
    }
    while(r.digits_.size() > 1 && r.digits_.back() == BigDigit(0)) r.digits_.pop_back();
    return r;
  }

  // Comparisons (unsigned) ===================================================
  bool operator == (const BigCardinal& n) const { 
    if(size() != n.size()) return false;
    for (int i=0; i < size(); i++)
      if(digits_[i] != n.digits_[i]) return false;
    return true; 
  }
  bool operator == (digit d) const { return *this == BigCardinal(d); }
  bool operator != (const BigCardinal& n) const { return !(*this == n); }
  bool operator != (digit d) const { return *this != BigCardinal(d); }
  bool operator <  (const BigCardinal& n) const {
    if(size() != n.size()) return size() < n.size();
    for (int i=size()-1; i >= 0; i--)
      if(digits_[i] != n.digits_[i]) return digits_[i] < n.digits_[i];
    return false;
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
    static_cast<BigNumberCore&>(*this) = static_cast<const BigNumberCore&>(n);
    return *this;
  }

  BigCardinal& operator = (const digit d) { 
    digits_.clear(); 
    digits_.push_back(BigDigit(d)); 
    return *this;
  }

  BigCardinal& operator = (BigCardinal&& other) noexcept {
    if(this != &other) {
      static_cast<BigNumberCore&>(*this) = std::move(static_cast<BigNumberCore&>(other));
    }
    return *this;
  }

  bool isZero() const { return size() == 1 && digits_[0] == BigDigit(0); }
  bool isOne() const { return size() == 1 && digits_[0] == BigDigit(1); }
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

