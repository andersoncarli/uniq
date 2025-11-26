#include "BigCardinal.h"
#include "test.h"
namespace uniq {

TEST(BigCardinal){
  // basic construction =======================================================
  BigCardinal n0; CHECK(n0.size() == 1); CHECK(n0 == BigCardinal(0));
  BigCardinal n1(42); CHECK(n1.size() == 1);
  BigCardinal n2(BigDigit(100)); CHECK(n2.size() == 1);
  
  // string construction ======================================================
  string A = "100000000000000000011000000000000000000210000000000000000003";
  BigCardinal a(A);
  CHECK(a == BigCardinal("8H78Mi9W0018H78Mi9W0028H78Mi9W003", 64));
  
  // round-trip value equality ===============================================
  const string B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  BigCardinal rt1(B64, 64, B64);
  BigCardinal rt2(rt1.format(64, B64), 64, B64);
  CHECK(rt1 == rt2);
  
  // simple round-trip tests =================================================
  CHECK(BigCardinal("AB", 64, B64) == BigCardinal("B", 64, B64));
  CHECK(BigCardinal("0123", 10) == BigCardinal("123", 10));
  CHECK(BigCardinal("A", 64, B64).format(64, B64) == "A");
  
  // B64 round-trip with leading zero =========================================
  CHECK(B64 == "A"+BigCardinal(B64,64,B64).format(64,B64));

  // formatting ===============================================================
  BigCardinal f1(255); CHECK(f1.format(16) == "ff");
  BigCardinal f2(10); CHECK(f2.format(2) == "1010");
  CHECK(BigCardinal(0).format() == "0");
  
  // addition ================================================================
  string B = "fffffffffffffff0fffffffffffffff1fffffffffffffff2";
  BigCardinal b(B,16);
  BigCardinal c(b);
  c += b;
  CHECK(c.size() > b.size() || c != b);
  
  BigCardinal d = b + b;
  CHECK(d.size() >= b.size());
  
  BigCardinal e(1);
  ++e; CHECK(e == BigCardinal(2));
  e += BigCardinal(3); CHECK(e == BigCardinal(5));
  
  // comparison ==============================================================
  CHECK(BigCardinal(5) == BigCardinal(5));
  CHECK(BigCardinal(5) != BigCardinal(3));
  CHECK(BigCardinal(10) > BigCardinal(5));
  CHECK(BigCardinal(5) < BigCardinal(10));
  
  // assignment ==============================================================
  BigCardinal x(10);
  x = 20; CHECK(x == BigCardinal(20));
  BigCardinal y(30);
  x = y; CHECK(x == BigCardinal(30));
  
  // bits calculation ========================================================
  CHECK(BigCardinal(0).bits() == 0);
  CHECK(BigCardinal(1).bits() == 1);
  CHECK(BigCardinal(255).bits() == 8);
  
  // subtraction ============================================================
  CHECK(BigCardinal(10) - BigCardinal(3) == BigCardinal(7));
  CHECK(BigCardinal(5) - BigCardinal(5) == BigCardinal(0));
  CHECK(BigCardinal(100) - BigCardinal(50) == BigCardinal(50));
  BigCardinal sub1(20); sub1 -= BigCardinal(7); CHECK(sub1 == BigCardinal(13));
  BigCardinal sub2(15); --sub2; CHECK(sub2 == BigCardinal(14));
  BigCardinal sub3(10); BigCardinal sub4 = sub3--; CHECK(sub4 == BigCardinal(10) && sub3 == BigCardinal(9));
  
  // multiplication =========================================================
  CHECK(BigCardinal(6) * BigCardinal(7) == BigCardinal(42));
  CHECK(BigCardinal(100) * BigCardinal(0) == BigCardinal(0));
  CHECK(BigCardinal(100) * BigCardinal(1) == BigCardinal(100));
  CHECK(BigCardinal(0) * BigCardinal(100) == BigCardinal(0));
  BigCardinal mul1(5); mul1 *= BigCardinal(4); CHECK(mul1 == BigCardinal(20));
  CHECK(BigCardinal(255) * BigCardinal(2) == BigCardinal(510));
  
  // division ===============================================================
  CHECK(BigCardinal(100) / BigCardinal(4) == BigCardinal(25));
  CHECK(BigCardinal(5) / BigCardinal(5) == BigCardinal(1));
  CHECK(BigCardinal(10) / BigCardinal(3) == BigCardinal(3));
  BigCardinal div1(50); div1 /= BigCardinal(5); CHECK(div1 == BigCardinal(10));
  CHECK_EXCEPTION(BigCardinal(10) / BigCardinal(0));
  
  // modulo ==================================================================
  CHECK(BigCardinal(100) % BigCardinal(7) == BigCardinal(2));
  CHECK(BigCardinal(10) % BigCardinal(10) == BigCardinal(0));
  CHECK(BigCardinal(15) % BigCardinal(4) == BigCardinal(3));
  BigCardinal mod1(100); mod1 %= BigCardinal(7); CHECK(mod1 == BigCardinal(2));
  
  // large number operations ================================================
  BigCardinal large1("1000000000000000000");
  BigCardinal large2("999999999999999999");
  CHECK((large1 - large2) == BigCardinal(1));
  CHECK(BigCardinal(1000000) * BigCardinal(1000000) == BigCardinal("1000000000000"));
  
  // edge cases ==============================================================
  CHECK(BigCardinal(1) - BigCardinal(1) == BigCardinal(0));
  CHECK(BigCardinal(0) - BigCardinal(0) == BigCardinal(0));
  CHECK(BigCardinal(1) * BigCardinal(0) == BigCardinal(0));
  CHECK(BigCardinal(0) * BigCardinal(1) == BigCardinal(0));
  CHECK(BigCardinal(1) / BigCardinal(1) == BigCardinal(1));
  CHECK(BigCardinal(0) / BigCardinal(5) == BigCardinal(0));
  CHECK(BigCardinal(1) % BigCardinal(1) == BigCardinal(0));
  
  // shift operations ========================================================
  CHECK(BigCardinal(8) << BigCardinal(2) == BigCardinal(32));
  CHECK(BigCardinal(32) >> BigCardinal(2) == BigCardinal(8));
  BigCardinal shift1(16); shift1 <<= BigCardinal(1); CHECK(shift1 == BigCardinal(32));
  BigCardinal shift2(16); shift2 >>= BigCardinal(1); CHECK(shift2 == BigCardinal(8));
  CHECK(BigCardinal(8) << BigCardinal(0) == BigCardinal(8));
  CHECK(BigCardinal(8) >> BigCardinal(0) == BigCardinal(8));
  
  // bitwise operations =======================================================
  CHECK((BigCardinal(5) & BigCardinal(3)) == BigCardinal(1));
  CHECK((BigCardinal(5) | BigCardinal(3)) == BigCardinal(7));
  CHECK((BigCardinal(5) ^ BigCardinal(3)) == BigCardinal(6));
  BigCardinal bw1(5); bw1 &= BigCardinal(3); CHECK(bw1 == BigCardinal(1));
  BigCardinal bw2(5); bw2 |= BigCardinal(2); CHECK(bw2 == BigCardinal(7));
  BigCardinal bw3(5); bw3 ^= BigCardinal(3); CHECK(bw3 == BigCardinal(6));
  CHECK((~BigCardinal(0)) != BigCardinal(0));
  
  // copy and move semantics ================================================
  BigCardinal orig(42);
  BigCardinal copied(orig);
  CHECK(copied == BigCardinal(42));
  BigCardinal moved(move(orig));
  CHECK(moved == BigCardinal(42));
  BigCardinal assign_test(10);
  assign_test = move(moved);
  CHECK(assign_test == BigCardinal(42));
  
  // utility functions =======================================================
  CHECK(pow(BigCardinal(2), BigCardinal(3)) == BigCardinal(8));
  CHECK(pow(BigCardinal(5), BigCardinal(2)) == BigCardinal(25));
  CHECK(pow(BigCardinal(10), BigCardinal(0)) == BigCardinal(1));
  CHECK(gcd(BigCardinal(48), BigCardinal(18)) == BigCardinal(6));
  CHECK(gcd(BigCardinal(17), BigCardinal(13)) == BigCardinal(1));
  CHECK(lcm(BigCardinal(12), BigCardinal(18)) == BigCardinal(36));
  CHECK(BigCardinal(0).isZero());
  CHECK(BigCardinal(1).isOne());
  CHECK(!BigCardinal(2).isZero());
  CHECK(!BigCardinal(2).isOne());
};

} // namespace uniq

