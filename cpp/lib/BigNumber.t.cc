#include "BigNumber.h"
#include "test.h"
namespace uniq {

TEST(BigNumber){
  // basic construction =======================================================
  BigNumber n0; CHECK(n0.size() == 1); CHECK(n0 == BigNumber(0));
  BigNumber n1(42); CHECK(n1.size() == 1);
  BigNumber n2(BigDigit(100)); CHECK(n2.size() == 1);
  
  // string construction ======================================================
  string A = "100000000000000000011000000000000000000210000000000000000003";
  BigNumber a(A);
  CHECK(a == BigNumber("8H78Mi9W0018H78Mi9W0028H78Mi9W003", 64));
  
  // round-trip value equality (prerequisite test) ===========================
  const string B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  BigNumber rt1(B64, 64, B64);
  BigNumber rt2(rt1.format(64, B64), 64, B64);
  CHECK(rt1 == rt2); // Numeric value should be preserved
  
  // simple round-trip tests =================================================
  CHECK(BigNumber("AB", 64, B64) == BigNumber("B", 64, B64)); // Leading zero lost
  CHECK(BigNumber("0123", 10) == BigNumber("123", 10)); // Base 10 leading zero
  CHECK(BigNumber("A", 64, B64).format(64, B64) == "A"); // Single zero char
  
  // B64 round-trip with leading zero =========================================
  CHECK(B64 == "A"+BigNumber(B64,64,B64).format(64,B64));

  // formatting ===============================================================
  BigNumber f1(255); CHECK(f1.format(16) == "ff");
  BigNumber f2(10); CHECK(f2.format(2) == "1010");
  CHECK(BigNumber(0).format() == "0");
  
  // addition ================================================================
  string B = "fffffffffffffff0fffffffffffffff1fffffffffffffff2";
  BigNumber b(B,16);
  BigNumber c(b);
  c += b;
  CHECK(c.size() > b.size() || c != b);
  
  BigNumber d = b + b;
  CHECK(d.size() >= b.size());
  
  BigNumber e(1);
  ++e; CHECK(e == BigNumber(2));
  e += BigNumber(3); CHECK(e == BigNumber(5));
  
  // comparison ==============================================================
  CHECK(BigNumber(5) == BigNumber(5));
  CHECK(BigNumber(5) != BigNumber(3));
  CHECK(BigNumber(10) > BigNumber(5));
  CHECK(BigNumber(5) < BigNumber(10));
  
  // assignment ==============================================================
  BigNumber x(10);
  x = 20; CHECK(x == BigNumber(20));
  BigNumber y(30);
  x = y; CHECK(x == BigNumber(30));
  
  // bits calculation ========================================================
  CHECK(BigNumber(0).bits() == 0);
  CHECK(BigNumber(1).bits() == 1);
  CHECK(BigNumber(255).bits() == 8);
  
  // subtraction ============================================================
  CHECK(BigNumber(10) - BigNumber(3) == BigNumber(7));
  CHECK(BigNumber(5) - BigNumber(5) == BigNumber(0));
  CHECK(BigNumber(100) - BigNumber(50) == BigNumber(50));
  BigNumber sub1(20); sub1 -= BigNumber(7); CHECK(sub1 == BigNumber(13));
  BigNumber sub2(15); --sub2; CHECK(sub2 == BigNumber(14));
  BigNumber sub3(10); BigNumber sub4 = sub3--; CHECK(sub4 == BigNumber(10) && sub3 == BigNumber(9));
  
  // multiplication =========================================================
  CHECK(BigNumber(6) * BigNumber(7) == BigNumber(42));
  CHECK(BigNumber(100) * BigNumber(0) == BigNumber(0));
  CHECK(BigNumber(100) * BigNumber(1) == BigNumber(100));
  CHECK(BigNumber(0) * BigNumber(100) == BigNumber(0));
  BigNumber mul1(5); mul1 *= BigNumber(4); CHECK(mul1 == BigNumber(20));
  CHECK(BigNumber(255) * BigNumber(2) == BigNumber(510));
  
  // division ===============================================================
  CHECK(BigNumber(100) / BigNumber(4) == BigNumber(25));
  CHECK(BigNumber(5) / BigNumber(5) == BigNumber(1));
  CHECK(BigNumber(10) / BigNumber(3) == BigNumber(3));
  BigNumber div1(50); div1 /= BigNumber(5); CHECK(div1 == BigNumber(10));
  CHECK_EXCEPTION(BigNumber(10) / BigNumber(0));
  
  // modulo ==================================================================
  CHECK(BigNumber(100) % BigNumber(7) == BigNumber(2));
  CHECK(BigNumber(10) % BigNumber(10) == BigNumber(0));
  CHECK(BigNumber(15) % BigNumber(4) == BigNumber(3));
  BigNumber mod1(100); mod1 %= BigNumber(7); CHECK(mod1 == BigNumber(2));
  
  // large number operations ================================================
  BigNumber large1("1000000000000000000");
  BigNumber large2("999999999999999999");
  CHECK((large1 - large2) == BigNumber(1));
  CHECK(BigNumber(1000000) * BigNumber(1000000) == BigNumber("1000000000000"));
  
  // edge cases ==============================================================
  CHECK(BigNumber(1) - BigNumber(1) == BigNumber(0));
  CHECK(BigNumber(0) - BigNumber(0) == BigNumber(0));
  CHECK(BigNumber(1) * BigNumber(0) == BigNumber(0));
  CHECK(BigNumber(0) * BigNumber(1) == BigNumber(0));
  CHECK(BigNumber(1) / BigNumber(1) == BigNumber(1));
  CHECK(BigNumber(0) / BigNumber(5) == BigNumber(0));
  CHECK(BigNumber(1) % BigNumber(1) == BigNumber(0));
  
  // shift operations ========================================================
  CHECK(BigNumber(8) << BigNumber(2) == BigNumber(32));
  CHECK(BigNumber(32) >> BigNumber(2) == BigNumber(8));
  BigNumber shift1(16); shift1 <<= BigNumber(1); CHECK(shift1 == BigNumber(32));
  BigNumber shift2(16); shift2 >>= BigNumber(1); CHECK(shift2 == BigNumber(8));
  CHECK(BigNumber(8) << BigNumber(0) == BigNumber(8));
  CHECK(BigNumber(8) >> BigNumber(0) == BigNumber(8));
  
  // bitwise operations =======================================================
  CHECK((BigNumber(5) & BigNumber(3)) == BigNumber(1));
  CHECK((BigNumber(5) | BigNumber(3)) == BigNumber(7));
  CHECK((BigNumber(5) ^ BigNumber(3)) == BigNumber(6));
  BigNumber bw1(5); bw1 &= BigNumber(3); CHECK(bw1 == BigNumber(1));
  BigNumber bw2(5); bw2 |= BigNumber(2); CHECK(bw2 == BigNumber(7));
  BigNumber bw3(5); bw3 ^= BigNumber(3); CHECK(bw3 == BigNumber(6));
  CHECK((~BigNumber(0)) != BigNumber(0));
  
  // copy and move semantics ================================================
  BigNumber orig(42);
  BigNumber copied(orig);
  CHECK(copied == BigNumber(42));
  BigNumber moved(move(orig));
  CHECK(moved == BigNumber(42));
  BigNumber assign_test(10);
  assign_test = move(moved);
  CHECK(assign_test == BigNumber(42));
  
  // utility functions =======================================================
  CHECK(pow(BigNumber(2), BigNumber(3)) == BigNumber(8));
  CHECK(pow(BigNumber(5), BigNumber(2)) == BigNumber(25));
  CHECK(pow(BigNumber(10), BigNumber(0)) == BigNumber(1));
  CHECK(gcd(BigNumber(48), BigNumber(18)) == BigNumber(6));
  CHECK(gcd(BigNumber(17), BigNumber(13)) == BigNumber(1));
  CHECK(lcm(BigNumber(12), BigNumber(18)) == BigNumber(36));
  CHECK(BigNumber(0).isZero());
  CHECK(BigNumber(1).isOne());
  CHECK(!BigNumber(2).isZero());
  CHECK(!BigNumber(2).isOne());
};

TEST(SignedArithmetic) {
  // Negative number construction =============================================
  BigNumber n1("-123");
  CHECK(n1.sign() == -1);
  CHECK(n1.isNegative());
  CHECK(!n1.isPositive());
  CHECK(n1.format() == "-123");
  
  BigNumber n2("-0");
  CHECK(n2.sign() == 0);
  CHECK(n2.format() == "0");
  
  // Sign helper functions ====================================================
  BigNumber pos(42);
  BigNumber neg("-42");
  
  CHECK(pos.sign() == 1);
  CHECK(neg.sign() == -1);
  CHECK(pos.isPositive());
  CHECK(!pos.isNegative());
  CHECK(!neg.isPositive());
  CHECK(neg.isNegative());
  
  CHECK(pos.abs() == BigNumber(42));
  CHECK(neg.abs() == BigNumber(42));
  CHECK(neg.negate() == BigNumber(42));
  CHECK(pos.negate().sign() == -1);
  CHECK(pos.negate().abs() == BigNumber(42));
  
  // Signed addition =========================================================
  CHECK(BigNumber(5) + BigNumber(3) == BigNumber(8));
  CHECK(BigNumber("-5") + BigNumber("-3") == BigNumber("-8"));
  CHECK(BigNumber(5) + BigNumber("-3") == BigNumber(2));
  CHECK(BigNumber("-5") + BigNumber(3) == BigNumber("-2"));
  CHECK(BigNumber(3) + BigNumber("-5") == BigNumber("-2"));
  CHECK(BigNumber("-3") + BigNumber(5) == BigNumber(2));
  
  BigNumber a1(10);
  a1 += BigNumber("-3");
  CHECK(a1 == BigNumber(7));
  
  BigNumber a2("-10");
  a2 += BigNumber(3);
  CHECK(a2 == BigNumber("-7"));
  
  BigNumber a3(5);
  a3 += BigNumber("-10");
  CHECK(a3 == BigNumber("-5"));
  
  // Signed subtraction ======================================================
  CHECK(BigNumber(5) - BigNumber(3) == BigNumber(2));
  CHECK(BigNumber(3) - BigNumber(5) == BigNumber("-2"));
  CHECK(BigNumber("-5") - BigNumber("-3") == BigNumber("-2"));
  CHECK(BigNumber("-3") - BigNumber("-5") == BigNumber(2));
  CHECK(BigNumber(5) - BigNumber("-3") == BigNumber(8));
  CHECK(BigNumber("-5") - BigNumber(3) == BigNumber("-8"));
  
  BigNumber s1(10);
  s1 -= BigNumber(3);
  CHECK(s1 == BigNumber(7));
  
  BigNumber s2(3);
  s2 -= BigNumber(10);
  CHECK(s2 == BigNumber("-7"));
  
  BigNumber s3("-10");
  s3 -= BigNumber("-3");
  CHECK(s3 == BigNumber("-7"));
  
  // Signed multiplication ===================================================
  CHECK(BigNumber(5) * BigNumber(3) == BigNumber(15));
  CHECK(BigNumber("-5") * BigNumber(3) == BigNumber("-15"));
  CHECK(BigNumber(5) * BigNumber("-3") == BigNumber("-15"));
  CHECK(BigNumber("-5") * BigNumber("-3") == BigNumber(15));
  
  BigNumber m1(6);
  m1 *= BigNumber("-7");
  CHECK(m1 == BigNumber("-42"));
  
  BigNumber m2("-6");
  m2 *= BigNumber("-7");
  CHECK(m2 == BigNumber(42));
  
  CHECK(BigNumber(0) * BigNumber("-5") == BigNumber(0));
  CHECK(BigNumber("-5") * BigNumber(0) == BigNumber(0));
  
  // Signed division =========================================================
  CHECK(BigNumber(15) / BigNumber(3) == BigNumber(5));
  CHECK(BigNumber("-15") / BigNumber(3) == BigNumber("-5"));
  CHECK(BigNumber(15) / BigNumber("-3") == BigNumber("-5"));
  CHECK(BigNumber("-15") / BigNumber("-3") == BigNumber(5));
  
  BigNumber d1(20);
  d1 /= BigNumber("-4");
  CHECK(d1 == BigNumber("-5"));
  
  BigNumber d2("-20");
  d2 /= BigNumber("-4");
  CHECK(d2 == BigNumber(5));
  
  // Signed modulo ===========================================================
  CHECK(BigNumber(17) % BigNumber(5) == BigNumber(2));
  CHECK(BigNumber("-17") % BigNumber(5) == BigNumber("-2"));
  CHECK(BigNumber(17) % BigNumber("-5") == BigNumber(2));
  CHECK(BigNumber("-17") % BigNumber("-5") == BigNumber("-2"));
  
  BigNumber mod1(100);
  mod1 %= BigNumber("-7");
  CHECK(mod1 == BigNumber(2));
  
  BigNumber mod2("-100");
  mod2 %= BigNumber(7);
  CHECK(mod2 == BigNumber("-2"));
  
  // Signed comparisons ======================================================
  CHECK(BigNumber("-5") < BigNumber(5));
  CHECK(BigNumber("-10") < BigNumber("-5"));
  CHECK(BigNumber(5) > BigNumber("-5"));
  CHECK(BigNumber("-5") > BigNumber("-10"));
  
  CHECK(BigNumber("-5") <= BigNumber(5));
  CHECK(BigNumber("-5") <= BigNumber("-5"));
  CHECK(BigNumber(5) >= BigNumber("-5"));
  CHECK(BigNumber("-5") >= BigNumber("-10"));
  
  CHECK(BigNumber("-5") != BigNumber(5));
  CHECK(BigNumber("-5") == BigNumber("-5"));
  
  // Increment/decrement with signs ==========================================
  BigNumber inc1(5);
  ++inc1;
  CHECK(inc1 == BigNumber(6));
  
  BigNumber inc2("-5");
  ++inc2;
  CHECK(inc2 == BigNumber("-4"));
  
  BigNumber dec1(5);
  --dec1;
  CHECK(dec1 == BigNumber(4));
  
  BigNumber dec2("-5");
  --dec2;
  CHECK(dec2 == BigNumber("-6"));
  
  BigNumber dec3(1);
  --dec3;
  CHECK(dec3 == BigNumber(0));
  
  BigNumber dec4("-1");
  ++dec4;
  CHECK(dec4 == BigNumber(0));
  
  // Format with negative numbers ============================================
  CHECK(BigNumber("-123").format() == "-123");
  CHECK(BigNumber("-255").format(16) == "-ff");
  CHECK(BigNumber("-10").format(2) == "-1010");
  
  // Large signed operations =================================================
  BigNumber large_pos("1000000000000000000");
  BigNumber large_neg("-1000000000000000000");
  
  CHECK(large_pos + large_neg == BigNumber(0));
  CHECK(large_pos - large_neg == BigNumber("2000000000000000000"));
  CHECK(large_neg - large_pos == BigNumber("-2000000000000000000"));
  BigNumber neg_two("-2");
  CHECK(large_pos * neg_two == large_neg * BigNumber(2));
  
  // Edge cases ==============================================================
  CHECK(BigNumber(0).sign() == 0);
  CHECK(BigNumber(0).abs() == BigNumber(0));
  CHECK(BigNumber(0).negate() == BigNumber(0));
  
  BigNumber neg_one("-1");
  CHECK(BigNumber(1) + neg_one == BigNumber(0));
  CHECK(neg_one + BigNumber(1) == BigNumber(0));
  CHECK(BigNumber(1) - BigNumber(1) == BigNumber(0));
  CHECK(neg_one - neg_one == BigNumber(0));
};

// PI1000 test is defined in BigInteger.t.cc (BigNumber is an alias for BigInteger)
// TEST(PI1000) { ... }

} // namespace uniq

