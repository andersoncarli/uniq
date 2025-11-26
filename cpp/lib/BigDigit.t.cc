#include "BigDigit.h"
#include "test.h"
namespace uniq {

TEST(BigDigit){
  
  CHECK(sizeof(BigDigit) == sizeof(digit));
  // operators =================================================================
  BigDigit d(4);       CHECK(d == 4); CHECK(d.size()== 3);
  d=d-1;            CHECK(d == 3); CHECK(d.size()== 2);
  digit x=d+1; d=x; CHECK(x == 4);
  d--;              CHECK(d == 3);
  ++d;              CHECK(d == 4);
  --d;              CHECK(d == 3);
  
  // bit operations ===========================================================
  BigDigit b(5); CHECK(b.getbit(0) == 1); CHECK(b.getbit(1) == 0); CHECK(b.getbit(2) == 1);
  b.setbit(1, 1); CHECK(b == 7);
  b.setbit(0, 0); CHECK(b == 6);
  CHECK(b[0] == 0); CHECK(b[1] == 1); CHECK(b[2] == 1);
  
  // shift operations ==========================================================
  BigDigit s(8); s >>= 2; CHECK(s == 2);
  s <<= 3; CHECK(s == 16);
  BigDigit s2(16); CHECK((s2 >> digit(2)) == 4);
  BigDigit s3(4); CHECK((s3 << digit(2)) == 16);
  
  // distance ==================================================================
  CHECK(BigDigit(7).distance(5) == 1);
  CHECK(BigDigit(15).distance(0) == 4);

  // formating =================================================================
  CHECK(BigDigit(0).format() == "0"); // default map starts with "0"
  CHECK(BigDigit(0).format(16) == "0"); // default map
  CHECK(BigDigit(0).format(2) == "0"); // default map
  const string B64map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  CHECK(BigDigit(0).format(64, B64map) == "A"); // custom map
  
  const string B10 = "10000000000000000001";
  const string B16 = "8ac7230489e80001";
  const string B64 = "8H78Mi9W001";
 
  BigDigit big(B10); CHECK(big.size()== 64);
  CHECK(big.format() == B10);
  CHECK(big.format(16) == B16);
  CHECK(big.format(64) == B64);
  CHECK(BigDigit(B64,64) == big);
  
  BigDigit h(SIZE_MAX);
  CHECK(h.format(16) == "ffffffffffffffff");
  CHECK(h.format(64) == "f<<<<<<<<<<");
  
  CHECK(BigDigit(255).format(16) == "ff");
  CHECK(BigDigit(10).format(2) == "1010");

  // arithmetic operators =====================================================
  BigDigit a1(10); a1 += 5; CHECK(a1 == 15);
  BigDigit a2(20); a2 -= 7; CHECK(a2 == 13);
  BigDigit a3(6); a3 *= 7; CHECK(a3 == 42);
  BigDigit a4(100); a4 /= 4; CHECK(a4 == 25);
  BigDigit a5(100); a5 %= 7; CHECK(a5 == 2);
  
  CHECK((BigDigit(10) + 5) == 15);
  CHECK((BigDigit(20) - 7) == 13);
  CHECK((BigDigit(6) * 7) == 42);
  CHECK((BigDigit(100) / 4) == 25);
  CHECK((BigDigit(100) % 7) == 2);
  
  // bitwise operators =========================================================
  BigDigit bw1(5); bw1 &= 3; CHECK(bw1 == 1);
  BigDigit bw2(5); bw2 |= 2; CHECK(bw2 == 7);
  BigDigit bw3(5); bw3 ^= 3; CHECK(bw3 == 6);
  
  // exceptions =================================================================
  BigDigit ex(3);
  try { ex -= 4;       }  catch(FlowException& e) { CHECK(e.value == 1); };
  try { ex += SIZE_MAX; } catch(FlowException& e) { CHECK(e.value == 1); };
  try { ex /= 5;        } catch(FlowException& e) { CHECK(e.value == 3); };
  try { ex *= SIZE_MAX; } catch(FlowException& e) { CHECK(e.value == 2); };
  try { ex <<= 64;      } catch(FlowException& e) { CHECK(ex.value == 3); };
  try { ex >>= 2;       } catch(FlowException& e) { CHECK(ex.value == 3); };

  CHECK(ex == 3);
  
  // size and log2 =============================================================
  CHECK(BigDigit(0).size() == 0);
  CHECK(BigDigit(1).size() == 1);
  CHECK(BigDigit(2).size() == 2);
  CHECK(BigDigit(4).size() == 3);
  CHECK(BigDigit(8).size() == 4);
  CHECK(BigDigit(0).log2() == 0);
  CHECK(BigDigit(1).log2() == 0);
  CHECK(BigDigit(4).log2() == 2);
  
  // rotation ===================================================================
  CHECK(BigDigit(1).rotl(1) == (digit(1) << 1 | digit(1) >> (BITS(digit)-1)));
  CHECK(BigDigit(1).rotr(1) == (digit(1) >> 1 | digit(1) << (BITS(digit)-1)));
  
  // post-increment/decrement return values ====================================
  BigDigit d1(5); CHECK(d1++ == 5 && d1 == 6);
  BigDigit d2(5); CHECK(d2-- == 5 && d2 == 4);
  BigDigit d3(10); BigDigit d4 = d3++; CHECK(d4 == 10 && d3 == 11);
  BigDigit d5(10); BigDigit d6 = d5--; CHECK(d6 == 10 && d5 == 9);
  
  // edge cases =================================================================
  BigDigit e1(SIZE_MAX); CHECK(e1.size() > 0);
  BigDigit e2(SIZE_MAX-1); CHECK(e2.size() > 0);
  CHECK(BigDigit(0).format(2) == "0");
  CHECK(BigDigit(1).format(2) == "1");
  CHECK(BigDigit(SIZE_MAX).format(16).length() > 0);
};

} // namespace uniq

