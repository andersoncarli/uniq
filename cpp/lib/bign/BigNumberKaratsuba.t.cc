#include "BigCardinal.h"
#include "BigNumberCore.h"
#include "test.h"

namespace bign {

TEST(KaratsubaMultiplication) {
  // Test that Karatsuba produces same results as naive
  BigCardinal a1(12345);
  a1.setImplementation(BIG_NAIVE);
  BigCardinal b1(67890);
  b1.setImplementation(BIG_NAIVE);
  BigCardinal c1 = a1 * b1;
  
  BigCardinal a2(12345);
  a2.setImplementation(BIG_KARATSUBA);
  BigCardinal b2(67890);
  b2.setImplementation(BIG_KARATSUBA);
  BigCardinal c2 = a2 * b2;
  
  CHECK(c1 == c2);
  
  // Test with larger numbers (should trigger Karatsuba)
  string large1 = "12345678901234567890123456789012345678901234567890";
  string large2 = "98765432109876543210987654321098765432109876543210";
  
  BigCardinal l1_naive(large1);
  l1_naive.setImplementation(BIG_NAIVE);
  BigCardinal l2_naive(large2);
  l2_naive.setImplementation(BIG_NAIVE);
  BigCardinal prod_naive = l1_naive * l2_naive;
  
  BigCardinal l1_karatsuba(large1);
  l1_karatsuba.setImplementation(BIG_KARATSUBA);
  BigCardinal l2_karatsuba(large2);
  l2_karatsuba.setImplementation(BIG_KARATSUBA);
  BigCardinal prod_karatsuba = l1_karatsuba * l2_karatsuba;
  
  CHECK(prod_naive == prod_karatsuba);
  
  // Test edge cases
  BigCardinal zero(0);
  zero.setImplementation(BIG_KARATSUBA);
  BigCardinal one(1);
  one.setImplementation(BIG_KARATSUBA);
  CHECK(zero * one == BigCardinal(0));
  CHECK(one * zero == BigCardinal(0));
  CHECK(one * one == BigCardinal(1));
  
  // Test with different sizes
  BigCardinal small(42);
  small.setImplementation(BIG_KARATSUBA);
  BigCardinal large(large1);
  large.setImplementation(BIG_KARATSUBA);
  BigCardinal mixed = small * large;
  BigCardinal mixed2 = large * small;
  CHECK(mixed == mixed2);
}

TEST(KaratsubaAutoSelection) {
  // Test that BIG_AUTO selects Karatsuba
  BigCardinal a;
  a.setImplementation(BIG_AUTO);
  CHECK(a.getImplementation() == BIG_AUTO || a.getImplementation() == BIG_KARATSUBA);
  
  // Test explicit selection
  BigCardinal b(100);
  b.setImplementation(BIG_KARATSUBA);
  CHECK(b.getImplementation() == BIG_KARATSUBA);
  CHECK(string(b.getImplementationName()).find("Karatsuba") != string::npos);
}

TEST(KaratsubaCorrectness) {
  // Test various multiplication cases
  BigCardinal a(123);
  a.setImplementation(BIG_KARATSUBA);
  BigCardinal b(456);
  b.setImplementation(BIG_KARATSUBA);
  BigCardinal c = a * b;
  CHECK(c == BigCardinal(123 * 456));
  
  // Test powers of 2
  BigCardinal two(2);
  two.setImplementation(BIG_KARATSUBA);
  BigCardinal four = two * two;
  CHECK(four == BigCardinal(4));
  
  BigCardinal eight = four * two;
  CHECK(eight == BigCardinal(8));
  
  // Test with very large result
  string s1 = "999999999999999999999999999999";
  string s2 = "888888888888888888888888888888";
  BigCardinal x1(s1);
  x1.setImplementation(BIG_KARATSUBA);
  BigCardinal x2(s2);
  x2.setImplementation(BIG_KARATSUBA);
  BigCardinal x3 = x1 * x2;
  
  // Verify it's not zero and has reasonable size
  CHECK(!x3.isZero());
  CHECK(x3.size() > 1);
}

} // namespace bign

