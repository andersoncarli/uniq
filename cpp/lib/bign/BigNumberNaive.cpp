#include "BigNumberNaive.h"
#include "BigDigit.h"
#include <algorithm>
#include <stdexcept>

namespace bign {

void BigNumberNaive::add(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  if(a.empty()) a.push_back(BigDigit(0));
  if(b.empty()) return;
  
  a.resize(std::max(a.size(), b.size()), BigDigit(0));
  for(size_t i = 0; i < b.size(); i++) {
    digit ov = 0;
    a[i].value = a[i].add(b[i].value, ov);
    if(ov) {
      if(i + 1 >= a.size()) a.push_back(BigDigit(ov));
      else {
        size_t j = i + 1;
        while(ov && j < a.size()) {
          digit next_ov = 0;
          a[j].value = a[j].add(ov, next_ov);
          ov = next_ov;
          j++;
        }
        if(ov) a.push_back(BigDigit(ov));
      }
    }
  }
}

void BigNumberNaive::subtract(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  if(a.empty() || b.empty()) return;
  
  for(size_t i = 0; i < b.size(); i++) {
    if(i >= a.size()) throw FlowException(1, SUB);
    digit borrow = 0;
    a[i].value = a[i].sub(b[i].value, borrow);
    if(borrow) {
      if(i + 1 >= a.size()) throw FlowException(borrow, SUB);
      size_t j = i + 1;
      while(borrow && j < a.size()) {
        digit next_borrow = 0;
        a[j].value = a[j].sub(borrow, next_borrow);
        borrow = next_borrow;
        j++;
      }
      if(borrow) throw FlowException(borrow, SUB);
    }
  }
  while(a.size() > 1 && a.back() == BigDigit(0)) a.pop_back();
}

void BigNumberNaive::multiply(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  if(a.empty() || b.empty()) {
    a.clear();
    a.push_back(BigDigit(0));
    return;
  }
  
  std::vector<BigDigit> product;
  product.resize(a.size() + b.size(), BigDigit(0));
  
  for(size_t bi = 0; bi < b.size(); bi++) {
    digit carry = 0;
    for(size_t ai = 0; ai < a.size(); ai++) {
      digit prod_ov = 0;
      digit prod = a[ai].mul(b[bi].value, prod_ov);
      uoverflow total = (uoverflow)prod + (uoverflow)carry;
      digit total_digit = (digit)total;
      digit total_ov = total >> BITS(digit);
      digit sum_ov = 0;
      digit sum = product[ai + bi].add(total_digit, sum_ov);
      product[ai + bi].value = sum;
      carry = prod_ov + total_ov + sum_ov;
    }
    if(carry) {
      size_t final_idx = bi + a.size();
      if(final_idx >= product.size()) {
        product.push_back(BigDigit(carry));
      } else {
        digit ov = 0;
        product[final_idx].value = product[final_idx].add(carry, ov);
        if(ov) product.push_back(BigDigit(ov));
      }
    }
  }
  while(product.size() > 1 && product.back() == BigDigit(0)) product.pop_back();
  a = product;
}

void BigNumberNaive::divide(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b,
                            std::vector<BigDigit>& quotient, std::vector<BigDigit>& remainder) {
  bool divisor_zero = true;
  for(const auto& d : b) {
    if(d.value != 0) {
      divisor_zero = false;
      break;
    }
  }
  if(divisor_zero || b.empty()) throw FlowException(1, DIV);
  
  bool dividend_zero = true;
  for(const auto& d : a) {
    if(d.value != 0) {
      dividend_zero = false;
      break;
    }
  }
  if(dividend_zero || a.empty()) {
    quotient.clear();
    quotient.push_back(BigDigit(0));
    remainder.clear();
    remainder.push_back(BigDigit(0));
    return;
  }
  
  remainder = a;
  quotient.clear();
  quotient.push_back(BigDigit(0));
  
  if(a.size() < b.size()) {
    return;
  }
  
  if(b.size() == 1 && a.size() == 1) {
    digit q = a[0].value / b[0].value;
    digit r = a[0].value % b[0].value;
    quotient[0] = BigDigit(q);
    remainder.clear();
    remainder.push_back(BigDigit(r));
    return;
  }
  
  // TODO: Implement full long division algorithm
}

int BigNumberNaive::compare(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b) const {
  if(a.size() != b.size()) return a.size() < b.size() ? -1 : 1;
  for(int i = a.size() - 1; i >= 0; i--) {
    if(a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
  }
  return 0;
}

void BigNumberNaive::bitwiseAnd(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  size_t max_size = std::max(a.size(), b.size());
  a.resize(max_size, BigDigit(0));
  for(size_t i = 0; i < max_size; i++) {
    digit val1 = i < a.size() ? a[i].value : 0;
    digit val2 = i < b.size() ? b[i].value : 0;
    a[i].value = val1 & val2;
  }
  while(a.size() > 1 && a.back() == BigDigit(0)) a.pop_back();
}

void BigNumberNaive::bitwiseOr(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  size_t max_size = std::max(a.size(), b.size());
  a.resize(max_size, BigDigit(0));
  for(size_t i = 0; i < max_size; i++) {
    digit val1 = i < a.size() ? a[i].value : 0;
    digit val2 = i < b.size() ? b[i].value : 0;
    a[i].value = val1 | val2;
  }
  while(a.size() > 1 && a.back() == BigDigit(0)) a.pop_back();
}

void BigNumberNaive::bitwiseXor(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  size_t max_size = std::max(a.size(), b.size());
  a.resize(max_size, BigDigit(0));
  for(size_t i = 0; i < max_size; i++) {
    digit val1 = i < a.size() ? a[i].value : 0;
    digit val2 = i < b.size() ? b[i].value : 0;
    a[i].value = val1 ^ val2;
  }
  while(a.size() > 1 && a.back() == BigDigit(0)) a.pop_back();
}

void BigNumberNaive::bitwiseNot(std::vector<BigDigit>& a) {
  for(size_t i = 0; i < a.size(); i++) {
    a[i].value = ~a[i].value;
  }
  while(a.size() > 1 && a.back() == BigDigit(0)) a.pop_back();
}

void BigNumberNaive::shiftLeft(std::vector<BigDigit>& a, int bits) {
  if(a.empty() || bits == 0) return;
  
  digit carry = 0;
  for(size_t i = 0; i < a.size(); i++) {
    digit ov = 0;
    digit shifted = a[i].shl(bits, ov);
    a[i].value = shifted | carry;
    carry = ov;
  }
  if(carry) a.push_back(BigDigit(carry));
}

void BigNumberNaive::shiftRight(std::vector<BigDigit>& a, int bits) {
  if(a.empty() || bits == 0) return;
  
  for(int i = a.size() - 1; i >= 0; i--) {
    digit ov = 0;
    a[i].value = a[i].shr(bits, ov);
    if(i > 0) {
      digit carry_bits = ov << (BITS(digit) - bits);
      a[i-1].value |= carry_bits;
    }
  }
  while(a.size() > 1 && a.back() == BigDigit(0)) a.pop_back();
}

} // namespace bign
