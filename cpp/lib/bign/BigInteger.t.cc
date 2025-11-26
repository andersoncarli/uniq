#include "BigInteger.h"
#include "test.h"
namespace bign {

TEST(BigInteger){
  // basic construction =======================================================
  BigInteger n0; CHECK(n0.size() == 1); CHECK(n0 == BigInteger(0));
  BigInteger n1(42); CHECK(n1.size() == 1);
  BigInteger n2(BigDigit(100)); CHECK(n2.size() == 1);
  
  // string construction ======================================================
  string A = "100000000000000000011000000000000000000210000000000000000003";
  BigInteger a(A);
  CHECK(a == BigInteger("8H78Mi9W0018H78Mi9W0028H78Mi9W003", 64));
  
  // Negative number construction =============================================
  BigInteger n3("-123");
  CHECK(n3.sign() == -1);
  CHECK(n3.isNegative());
  CHECK(!n3.isPositive());
  CHECK(n3.format() == "-123");
  
  BigInteger n4("-0");
  CHECK(n4.sign() == 0);
  CHECK(n4.format() == "0");
  
  // Sign helper functions ====================================================
  BigInteger pos(42);
  BigInteger neg("-42");
  
  CHECK(pos.sign() == 1);
  CHECK(neg.sign() == -1);
  CHECK(pos.isPositive());
  CHECK(!pos.isNegative());
  CHECK(!neg.isPositive());
  CHECK(neg.isNegative());
  
  CHECK(pos.abs() == BigInteger(42));
  CHECK(neg.abs() == BigInteger(42));
  CHECK(neg.negate() == BigInteger(42));
  CHECK(pos.negate().sign() == -1);
  CHECK(pos.negate().abs() == BigInteger(42));
  
  // Signed addition =========================================================
  CHECK(BigInteger(5) + BigInteger(3) == BigInteger(8));
  CHECK(BigInteger("-5") + BigInteger("-3") == BigInteger("-8"));
  CHECK(BigInteger(5) + BigInteger("-3") == BigInteger(2));
  CHECK(BigInteger("-5") + BigInteger(3) == BigInteger("-2"));
  CHECK(BigInteger(3) + BigInteger("-5") == BigInteger("-2"));
  CHECK(BigInteger("-3") + BigInteger(5) == BigInteger(2));
  
  BigInteger a1(10);
  a1 += BigInteger("-3");
  CHECK(a1 == BigInteger(7));
  
  BigInteger a2("-10");
  a2 += BigInteger(3);
  CHECK(a2 == BigInteger("-7"));
  
  BigInteger a3(5);
  a3 += BigInteger("-10");
  CHECK(a3 == BigInteger("-5"));
  
  // Signed subtraction ======================================================
  CHECK(BigInteger(5) - BigInteger(3) == BigInteger(2));
  CHECK(BigInteger(3) - BigInteger(5) == BigInteger("-2"));
  CHECK(BigInteger("-5") - BigInteger("-3") == BigInteger("-2"));
  CHECK(BigInteger("-3") - BigInteger("-5") == BigInteger(2));
  CHECK(BigInteger(5) - BigInteger("-3") == BigInteger(8));
  CHECK(BigInteger("-5") - BigInteger(3) == BigInteger("-8"));
  
  BigInteger s1(10);
  s1 -= BigInteger(3);
  CHECK(s1 == BigInteger(7));
  
  BigInteger s2(3);
  s2 -= BigInteger(10);
  CHECK(s2 == BigInteger("-7"));
  
  BigInteger s3("-10");
  s3 -= BigInteger("-3");
  CHECK(s3 == BigInteger("-7"));
  
  // Signed multiplication ===================================================
  CHECK(BigInteger(5) * BigInteger(3) == BigInteger(15));
  CHECK(BigInteger("-5") * BigInteger(3) == BigInteger("-15"));
  CHECK(BigInteger(5) * BigInteger("-3") == BigInteger("-15"));
  CHECK(BigInteger("-5") * BigInteger("-3") == BigInteger(15));
  
  BigInteger m1(6);
  m1 *= BigInteger("-7");
  CHECK(m1 == BigInteger("-42"));
  
  BigInteger m2("-6");
  m2 *= BigInteger("-7");
  CHECK(m2 == BigInteger(42));
  
  CHECK(BigInteger(0) * BigInteger("-5") == BigInteger(0));
  CHECK(BigInteger("-5") * BigInteger(0) == BigInteger(0));
  
  // Signed division =========================================================
  CHECK(BigInteger(15) / BigInteger(3) == BigInteger(5));
  CHECK(BigInteger("-15") / BigInteger(3) == BigInteger("-5"));
  CHECK(BigInteger(15) / BigInteger("-3") == BigInteger("-5"));
  CHECK(BigInteger("-15") / BigInteger("-3") == BigInteger(5));
  
  BigInteger d1(20);
  d1 /= BigInteger("-4");
  CHECK(d1 == BigInteger("-5"));
  
  BigInteger d2("-20");
  d2 /= BigInteger("-4");
  CHECK(d2 == BigInteger(5));
  
  // Signed modulo ===========================================================
  CHECK(BigInteger(17) % BigInteger(5) == BigInteger(2));
  CHECK(BigInteger("-17") % BigInteger(5) == BigInteger("-2"));
  CHECK(BigInteger(17) % BigInteger("-5") == BigInteger(2));
  CHECK(BigInteger("-17") % BigInteger("-5") == BigInteger("-2"));
  
  BigInteger mod1(100);
  mod1 %= BigInteger("-7");
  CHECK(mod1 == BigInteger(2));
  
  BigInteger mod2("-100");
  mod2 %= BigInteger(7);
  CHECK(mod2 == BigInteger("-2"));
  
  // Signed comparisons ======================================================
  CHECK(BigInteger("-5") < BigInteger(5));
  CHECK(BigInteger("-10") < BigInteger("-5"));
  CHECK(BigInteger(5) > BigInteger("-5"));
  CHECK(BigInteger("-5") > BigInteger("-10"));
  
  CHECK(BigInteger("-5") <= BigInteger(5));
  CHECK(BigInteger("-5") <= BigInteger("-5"));
  CHECK(BigInteger(5) >= BigInteger("-5"));
  CHECK(BigInteger("-5") >= BigInteger("-10"));
  
  CHECK(BigInteger("-5") != BigInteger(5));
  CHECK(BigInteger("-5") == BigInteger("-5"));
  
  // Increment/decrement with signs ==========================================
  BigInteger inc1(5);
  ++inc1;
  CHECK(inc1 == BigInteger(6));
  
  BigInteger inc2("-5");
  ++inc2;
  CHECK(inc2 == BigInteger("-4"));
  
  BigInteger dec1(5);
  --dec1;
  CHECK(dec1 == BigInteger(4));
  
  BigInteger dec2("-5");
  --dec2;
  CHECK(dec2 == BigInteger("-6"));
  
  BigInteger dec3(1);
  --dec3;
  CHECK(dec3 == BigInteger(0));
  
  BigInteger dec4("-1");
  ++dec4;
  CHECK(dec4 == BigInteger(0));
  
  // Format with negative numbers ============================================
  CHECK(BigInteger("-123").format() == "-123");
  CHECK(BigInteger("-255").format(16) == "-ff");
  CHECK(BigInteger("-10").format(2) == "-1010");
  
  // Large signed operations =================================================
  BigInteger large_pos("1000000000000000000");
  BigInteger large_neg("-1000000000000000000");
  
  CHECK(large_pos + large_neg == BigInteger(0));
  CHECK(large_pos - large_neg == BigInteger("2000000000000000000"));
  CHECK(large_neg - large_pos == BigInteger("-2000000000000000000"));
  BigInteger neg_two("-2");
  CHECK(large_pos * neg_two == large_neg * BigInteger(2));
  
  // Edge cases ==============================================================
  CHECK(BigInteger(0).sign() == 0);
  CHECK(BigInteger(0).abs() == BigInteger(0));
  CHECK(BigInteger(0).negate() == BigInteger(0));
  
  BigInteger neg_one("-1");
  CHECK(BigInteger(1) + neg_one == BigInteger(0));
  CHECK(neg_one + BigInteger(1) == BigInteger(0));
  CHECK(BigInteger(1) - BigInteger(1) == BigInteger(0));
  CHECK(neg_one - neg_one == BigInteger(0));
  
  // Utility functions =======================================================
  CHECK(pow(BigInteger(2), BigInteger(3)) == BigInteger(8));
  CHECK(pow(BigInteger(5), BigInteger(2)) == BigInteger(25));
  CHECK(pow(BigInteger(10), BigInteger(0)) == BigInteger(1));
  CHECK(gcd(BigInteger(48), BigInteger(18)) == BigInteger(6));
  CHECK(gcd(BigInteger(17), BigInteger(13)) == BigInteger(1));
  CHECK(lcm(BigInteger(12), BigInteger(18)) == BigInteger(36));
};

TEST(PI1000) {
  const string PI1000 = 
    "31415926535897932384626433832795028841971693993751058209749445923078164062"
    "86208998628034825342117067982148086513282306647093844609550582231725359408"
    "12848111745028410270193852110555964462294895493038196442881097566593344612"
    "84756482337867831652712019091456485669234603486104543266482133936072602491"
    "41273724587006606315588174881520920962829254091715364367892590360011330530"
    "54882046652138414695194151160943305727036575959195309218611738193261179310"
    "51185480744623799627495673518857527248912279381830119491298336733624406566"
    "43086021394946395224737190702179860943702770539217176293176752384674818467"
    "66940513200056812714526356082778577134275778960917363717872146844090122495"
    "34301465495853710507922796892589235420199561121290219608640344181598136297"
    "74771309960518707211349999998372978049951059731732816096318595024459455346"
    "90830264252230825334468503526193118817101000313783875288658753320838142061"
    "71776691473035982534904287554687311595628638823537875937519577818577805321"
    "71226806613001927876611195909216420198";
  
  BigInteger pi(PI1000);
  CHECK(pi.size() > 1);
  CHECK(pi.bits() > 1000);
  
  BigInteger pi_roundtrip(pi.format());
  CHECK(pi_roundtrip == pi);
  
  BigInteger pi2 = pi + BigInteger(0);
  CHECK(pi2 == pi);
  
  BigInteger pi_times_2 = pi * BigInteger(2);
  BigInteger pi_plus_pi = pi + pi;
  CHECK(pi_times_2 == pi_plus_pi);
  
  BigInteger pi_div_2 = pi_times_2 / BigInteger(2);
  CHECK(pi_div_2 == pi);
  
  BigInteger one(1);
  CHECK(pi > one);
  CHECK(pi != one);
}

} // namespace bign

