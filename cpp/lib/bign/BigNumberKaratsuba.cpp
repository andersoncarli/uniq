#include "BigNumberKaratsuba.h"
#include <algorithm>
#include <cassert>

namespace bign {

void BigNumberKaratsuba::normalize(std::vector<BigDigit>& a) {
  while(a.size() > 1 && a.back() == BigDigit(0)) {
    a.pop_back();
  }
}

void BigNumberKaratsuba::splitAt(const std::vector<BigDigit>& src, int pos, 
                                  std::vector<BigDigit>& low, std::vector<BigDigit>& high) {
  int size = (int)src.size();
  low.clear();
  high.clear();
  
  if(pos >= size) {
    low = src;
    high.push_back(BigDigit(0));
    return;
  }
  
  if(pos <= 0) {
    low.push_back(BigDigit(0));
    high = src;
    return;
  }
  
  low.resize(pos, BigDigit(0));
  high.resize(size - pos, BigDigit(0));
  
  for(int i = 0; i < pos; i++) {
    low[i] = src[i];
  }
  
  for(int i = pos; i < size; i++) {
    high[i - pos] = src[i];
  }
  
  normalize(low);
  normalize(high);
}

void BigNumberKaratsuba::multiplyKaratsubaRecursive(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  // Base case: use naive multiplication for small numbers
  int a_size = (int)a.size();
  int b_size = (int)b.size();
  
  if(a_size < KARATSUBA_THRESHOLD || b_size < KARATSUBA_THRESHOLD) {
    naive_.multiply(a, b);
    return;
  }
  
  // Make sure a is at least as long as b
  // If b is longer, swap them by making a copy and recursing
  if(a_size < b_size) {
    std::vector<BigDigit> a_copy = a;
    std::vector<BigDigit> b_copy = b;
    multiplyKaratsubaRecursive(b_copy, a_copy);
    a = b_copy;
    return;
  }
  
  // Split point: half of the larger number (a)
  int m = (a_size + 1) / 2;
  
  // Split a into a0 (low) and a1 (high)
  std::vector<BigDigit> a0, a1;
  splitAt(a, m, a0, a1);
  
  // Split b into b0 (low) and b1 (high)
  // b might be shorter than a, so handle that
  std::vector<BigDigit> b0, b1;
  if(b_size <= m) {
    b0 = b;
    b1.push_back(BigDigit(0));
  } else {
    splitAt(b, m, b0, b1);
  }
  
  // Compute z0 = a0 * b0
  std::vector<BigDigit> z0 = a0;
  multiplyKaratsubaRecursive(z0, b0);
  
  // Compute z2 = a1 * b1
  std::vector<BigDigit> z2 = a1;
  multiplyKaratsubaRecursive(z2, b1);
  
  // Compute a0 + a1 and b0 + b1
  std::vector<BigDigit> a_sum = a0;
  naive_.add(a_sum, a1);
  
  std::vector<BigDigit> b_sum = b0;
  naive_.add(b_sum, b1);
  
  // Compute z1 = (a0 + a1) * (b0 + b1) - z0 - z2
  std::vector<BigDigit> z1 = a_sum;
  multiplyKaratsubaRecursive(z1, b_sum);
  
  // z1 = z1 - z0 - z2
  naive_.subtract(z1, z0);
  naive_.subtract(z1, z2);
  
  // Result = z2 * B^(2m) + z1 * B^m + z0
  // First, shift z2 left by 2m digits
  std::vector<BigDigit> result = z2;
  for(int i = 0; i < 2 * m; i++) {
    result.insert(result.begin(), BigDigit(0));
  }
  
  // Add z1 shifted left by m digits
  std::vector<BigDigit> z1_shifted = z1;
  for(int i = 0; i < m; i++) {
    z1_shifted.insert(z1_shifted.begin(), BigDigit(0));
  }
  naive_.add(result, z1_shifted);
  
  // Add z0
  naive_.add(result, z0);
  
  normalize(result);
  a = result;
}

void BigNumberKaratsuba::multiply(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  if(a.empty() || b.empty()) {
    a.clear();
    a.push_back(BigDigit(0));
    return;
  }
  
  // Use Karatsuba for large numbers, naive for small numbers
  multiplyKaratsubaRecursive(a, b);
}

// Delegate all other operations to naive implementation
void BigNumberKaratsuba::add(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  naive_.add(a, b);
}

void BigNumberKaratsuba::subtract(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  naive_.subtract(a, b);
}

void BigNumberKaratsuba::divide(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b,
                                 std::vector<BigDigit>& quotient, std::vector<BigDigit>& remainder) {
  naive_.divide(a, b, quotient, remainder);
}

int BigNumberKaratsuba::compare(const std::vector<BigDigit>& a, const std::vector<BigDigit>& b) const {
  return naive_.compare(a, b);
}

void BigNumberKaratsuba::bitwiseAnd(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  naive_.bitwiseAnd(a, b);
}

void BigNumberKaratsuba::bitwiseOr(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  naive_.bitwiseOr(a, b);
}

void BigNumberKaratsuba::bitwiseXor(std::vector<BigDigit>& a, const std::vector<BigDigit>& b) {
  naive_.bitwiseXor(a, b);
}

void BigNumberKaratsuba::bitwiseNot(std::vector<BigDigit>& a) {
  naive_.bitwiseNot(a);
}

void BigNumberKaratsuba::shiftLeft(std::vector<BigDigit>& a, int bits) {
  naive_.shiftLeft(a, bits);
}

void BigNumberKaratsuba::shiftRight(std::vector<BigDigit>& a, int bits) {
  naive_.shiftRight(a, bits);
}

} // namespace bign

