#include "BigNumberNaive.h"
#include <algorithm>
#include <cassert>

namespace bign {

void BigNumberNaive::normalize(std::vector<BigDigit>& a) {
  while(a.size() > 1 && a.back() == BigDigit(0)) {
    a.pop_back();
  }
}

void BigNumberNaive::addDigit(std::vector<BigDigit>& a, BigDigit d, int i) {
  if(i >= (int)a.size()) {
    a.push_back(d);
    return;
  }
  digit ov = 0;
  a[i].value = a[i].add(d, ov);
  if(ov) addDigit(a, BigDigit(ov), i + 1);
}

void BigNumberNaive::add(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  for(int i = 0; i < (int)b.size(); i++) {
    addDigit(a, b[i], i);
  }
  normalize(a);
}

void BigNumberNaive::subtractDigit(std::vector<BigDigit>& a, BigDigit d, int i) {
  if(i >= (int)a.size()) {
    throw FlowException(1, SUB);
  }
  digit borrow = 0;
  a[i].value = a[i].sub(d, borrow);
  if(borrow) {
    if(i + 1 >= (int)a.size()) throw FlowException(borrow, SUB);
    subtractDigit(a, BigDigit(borrow), i + 1);
  }
}

void BigNumberNaive::subtract(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  for(int i = 0; i < (int)b.size(); i++) {
    if(i >= (int)a.size()) {
      throw FlowException(1, SUB);
    }
    digit borrow = 0;
    a[i].value = a[i].sub(b[i], borrow);
    if(borrow) {
      if(i + 1 >= (int)a.size()) throw FlowException(borrow, SUB);
      subtractDigit(a, BigDigit(borrow), i + 1);
    }
  }
  normalize(a);
}

void BigNumberNaive::multiply(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  if(a.empty() || b.empty()) {
    a.clear();
    a.push_back(BigDigit(0));
    return;
  }
  
  std::vector<BigDigit> product;
  product.resize(a.size() + b.size(), BigDigit(0));
  
  for(int bi = 0; bi < (int)b.size(); bi++) {
    digit carry = 0;
    for(int ai = 0; ai < (int)a.size(); ai++) {
      digit prod_ov = 0;
      digit prod = a[ai].mul(b[bi].value, prod_ov);
      uoverflow total = (uoverflow)prod + (uoverflow)carry;
      digit total_digit = (digit)total;
      digit total_ov = total >> BITS(digit);
      digit sum_ov = 0;
      digit sum = product[ai + bi].add(BigDigit(total_digit), sum_ov);
      product[ai + bi].value = sum;
      carry = prod_ov + total_ov + sum_ov;
    }
    if(carry) {
      int final_idx = bi + (int)a.size();
      if(final_idx >= (int)product.size()) {
        product.push_back(BigDigit(carry));
      } else {
        addDigit(product, BigDigit(carry), final_idx);
      }
    }
  }
  normalize(product);
  a = product;
}

int BigNumberNaive::compare(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b) const {
  if(a.size() != b.size()) {
    return (a.size() < b.size()) ? -1 : 1;
  }
  for(int i = (int)a.size() - 1; i >= 0; i--) {
    if(a[i] != b[i]) {
      return (a[i] < b[i]) ? -1 : 1;
    }
  }
  return 0;
}

void BigNumberNaive::divide(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b,
                            std::vector<BigDigit>& quotient, std::vector<BigDigit>& remainder) {
  if(b.empty() || (b.size() == 1 && b[0] == BigDigit(0))) {
    throw FlowException(1, DIV);
  }
  
  if(a.empty() || (a.size() == 1 && a[0] == BigDigit(0))) {
    quotient.clear();
    quotient.push_back(BigDigit(0));
    remainder.clear();
    remainder.push_back(BigDigit(0));
    return;
  }
  
  if(b.size() == 1 && b[0] == BigDigit(1)) {
    quotient = a;
    remainder.clear();
    remainder.push_back(BigDigit(0));
    return;
  }
  
  if(compare(a, b) < 0) {
    quotient.clear();
    quotient.push_back(BigDigit(0));
    remainder = a;
    return;
  }
  
  remainder = a;
  int div_size = (int)b.size();
  int quot_size = (int)remainder.size() - div_size + 1;
  if(quot_size <= 0) quot_size = 1;
  
  quotient.clear();
  quotient.resize(quot_size, BigDigit(0));
  
  for(int q_idx = quot_size - 1; q_idx >= 0; q_idx--) {
    int rem_start = q_idx;
    int rem_end = std::min(rem_start + div_size + 1, (int)remainder.size());
    
    if(rem_end <= rem_start) continue;
    
    std::vector<BigDigit> rem_part;
    rem_part.resize(rem_end - rem_start, BigDigit(0));
    for(int i = rem_start; i < rem_end; i++) {
      rem_part[i - rem_start].value = remainder[i].value;
    }
    
    if(compare(rem_part, b) < 0) continue;
    
    digit div_top = b[div_size - 1].value;
    if(div_top == 0 && div_size > 1) div_top = b[div_size - 2].value;
    
    int rem_part_size = (int)rem_part.size();
    uoverflow rem_top = 0;
    if(rem_part_size > 0) {
      rem_top = rem_part[rem_part_size - 1].value;
      if(rem_part_size > 1) {
        rem_top = (rem_top << BITS(digit)) | rem_part[rem_part_size - 2].value;
      }
    }
    
    if(div_top == 0) continue;
    
    uoverflow q_est = rem_top / (uoverflow)div_top;
    digit max_digit = (digit)-1;
    if(q_est > (uoverflow)max_digit) q_est = max_digit;
    digit q_digit = (digit)q_est;
    
    if(q_digit == 0) continue;
    
    bool success = false;
    for(int attempt = 0; attempt < 10 && !success; attempt++) {
      digit test_q = q_digit;
      if(attempt > 0 && attempt % 2 == 1 && test_q < max_digit) {
        test_q = q_digit + (attempt + 1) / 2;
      } else if(attempt > 0 && attempt % 2 == 0 && test_q > 0) {
        test_q = q_digit - attempt / 2;
      }
      if(test_q == 0) break;
      
      std::vector<BigDigit> product;
      product.resize(div_size + 1, BigDigit(0));
      
      digit carry = 0;
      for(int i = 0; i < div_size; i++) {
        uoverflow prod = (uoverflow)b[i].value * (uoverflow)test_q;
        digit prod_digit = (digit)prod;
        digit prod_ov = prod >> BITS(digit);
        uoverflow total = (uoverflow)prod_digit + (uoverflow)carry;
        digit total_digit = (digit)total;
        digit total_ov = total >> BITS(digit);
        product[i].value = total_digit;
        carry = prod_ov + total_ov;
      }
      if(carry) product[div_size].value = carry;
      normalize(product);
      
      if(q_idx > 0) {
        product.insert(product.begin(), q_idx, BigDigit(0));
      }
      
      if(compare(remainder, product) >= 0) {
        subtract(remainder, product);
        quotient[q_idx].value = test_q;
        success = true;
        break;
      }
    }
  }
  
  normalize(quotient);
  normalize(remainder);
}

void BigNumberNaive::bitwiseAnd(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  int max_size = std::max((int)a.size(), (int)b.size());
  a.resize(max_size, BigDigit(0));
  for(int i = 0; i < max_size; i++) {
    digit val1 = i < (int)a.size() ? a[i].value : 0;
    digit val2 = i < (int)b.size() ? b[i].value : 0;
    a[i].value = val1 & val2;
  }
  normalize(a);
}

void BigNumberNaive::bitwiseOr(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  int max_size = std::max((int)a.size(), (int)b.size());
  a.resize(max_size, BigDigit(0));
  for(int i = 0; i < max_size; i++) {
    digit val1 = i < (int)a.size() ? a[i].value : 0;
    digit val2 = i < (int)b.size() ? b[i].value : 0;
    a[i].value = val1 | val2;
  }
  normalize(a);
}

void BigNumberNaive::bitwiseXor(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  int max_size = std::max((int)a.size(), (int)b.size());
  a.resize(max_size, BigDigit(0));
  for(int i = 0; i < max_size; i++) {
    digit val1 = i < (int)a.size() ? a[i].value : 0;
    digit val2 = i < (int)b.size() ? b[i].value : 0;
    a[i].value = val1 ^ val2;
  }
  normalize(a);
}

void BigNumberNaive::bitwiseNot(std::vector<BigDigit>& a) {
  for(int i = 0; i < (int)a.size(); i++) {
    a[i].value = ~a[i].value;
  }
  normalize(a);
}

void BigNumberNaive::shiftLeft(std::vector<BigDigit>& a, int bits) {
  if(a.empty() || bits == 0) return;
  
  if(bits >= (int)BITS(digit)) {
    int digit_shift = bits / BITS(digit);
    int bit_shift = bits % BITS(digit);
    a.insert(a.begin(), digit_shift, BigDigit(0));
    if(bit_shift > 0) {
      digit carry = 0;
      for(int i = digit_shift; i < (int)a.size(); i++) {
        digit ov = 0;
        digit shifted = a[i].shl(bit_shift, ov);
        a[i].value = shifted | carry;
        carry = ov;
      }
      if(carry) a.push_back(BigDigit(carry));
    }
  } else {
    digit carry = 0;
    for(int i = 0; i < (int)a.size(); i++) {
      digit ov = 0;
      digit shifted = a[i].shl(bits, ov);
      a[i].value = shifted | carry;
      carry = ov;
    }
    if(carry) a.push_back(BigDigit(carry));
  }
  normalize(a);
}

void BigNumberNaive::shiftRight(std::vector<BigDigit>& a, int bits) {
  if(a.empty() || bits == 0) return;
  
  if(bits >= (int)BITS(digit)) {
    int digit_shift = bits / BITS(digit);
    if(digit_shift >= (int)a.size()) {
      a.clear();
      a.push_back(BigDigit(0));
      return;
    }
    a.erase(a.begin(), a.begin() + digit_shift);
    int bit_shift = bits % BITS(digit);
    if(bit_shift > 0) {
      for(int i = (int)a.size() - 1; i >= 0; i--) {
        digit ov = 0;
        a[i].value = a[i].shr(bit_shift, ov);
        if(i > 0) {
          digit carry_bits = ov << (BITS(digit) - bit_shift);
          a[i-1].value |= carry_bits;
        }
      }
    }
  } else {
    for(int i = (int)a.size() - 1; i >= 0; i--) {
      digit ov = 0;
      a[i].value = a[i].shr(bits, ov);
      if(i > 0) {
        digit carry_bits = ov << (BITS(digit) - bits);
        a[i-1].value |= carry_bits;
      }
    }
  }
  normalize(a);
}

} // namespace bign

