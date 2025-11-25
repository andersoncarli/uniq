#pragma once
#include "Digit.h"
namespace uniq {

class Number {
  vector<Digit> digits;
  i8 signal;
  int originalLength; // track original string length for formatting
public:
  Number() : digits(1, Digit(0)), signal(1), originalLength(0) {}
  Number(digit n) : digits(1, Digit(n)), signal(1), originalLength(0) {}
  Number(Digit n) : digits(1, n), signal(1), originalLength(0) {}
  Number(const string s, int base=10, string map=DIGITS) : originalLength(s.length()) {
    Digit d(s, base, [&](digit ov, DigitOp op)->digit { 
      digits.push_back(ov); 
      return 0;
    }, map);
    digits.push_back(d);
  }

  inline int size() const { return digits.size(); }
  int bits() const { 
    if(digits.empty()) return 0;
    int r=digits.back().size(); // Most significant digit (last in vector)
    int s=digits.size()-1;
    return s ? r + (s*BITS(digit)) : r;
  }

  string format(int base=10, string map=DIGITS) const {
    if(digits.empty()) return string(1, map[0]);
    ostringstream ss;
    // Digits are stored: [0]=most significant, [n]=least significant
    // Format from most to least significant for display
    for(int i=0; i < digits.size(); i++)
      ss << digits[i].format(base, map);
    string result = ss.str();
    // Remove leading zeros (characters matching map[0]) - but keep at least one character
    while(result.length() > 1 && result[0] == map[0]) {
      result = result.substr(1);
    }
    return result;
  }

  // operators =================================================================
  // Number operator += (digit d) { add(d); }
  // loopLeft()

  void add(Digit d, int i=0, const char* msg=""){ 
    if(i >= digits.size()) {
      digits.push_back(Digit(d));
      return;
    }

    digit ov=0;
    digits[i].value = digits[i].add(d, ov);
    if(ov) add(Digit(ov), i+1, msg);
  }

  Number& operator += (const Number& n) { 
    for (int i=0; i < n.size(); i++) 
      add(n[i], i);
    return *this;
  }

  Number operator + (const Number& n) const { 
    Number r(*this); 
    return r += n; 
  }

  Number& operator ++ () { add(Digit(1)); return *this; }

  bool operator == (const Number& n) const { 
    if(size() != n.size()) return false;
    for (int i=0; i < size(); i++)
      if(digits[i] != n.digits[i]) return false;
    return true; 
  }
  bool operator != (const Number& n) const { return !(*this == n); }
  bool operator <  (const Number& n) const {
    if(size() != n.size()) return size() < n.size();
    for (int i=size()-1; i >= 0; i--)
      if(digits[i] != n.digits[i]) return digits[i] < n.digits[i];
    return false;
  }
  bool operator >  (const Number& n) const { return n < *this; }
  bool operator <= (const Number& n) const { return !(n < *this); }
  bool operator >= (const Number& n) const { return !(*this < n); }

  Digit operator [](const int i) const { return digits[i]; }

  Number& operator = (const Number& n) { 
    digits = n.digits; 
    signal = n.signal; 
    return *this;
  }

  Number& operator = (const digit d) { 
    digits.clear(); 
    digits.push_back(Digit(d)); 
    signal=1; 
    return *this;
  }
};

ostream& operator<<(ostream& os, const Number& n) { return os << n.format(); }

// utilities & math
// Number factorial(Number n) { return n <= 1 ? n : factorial(n - 1) * n; }

TEST(Number){
  // basic construction =======================================================
  Number n0; CHECK(n0.size() == 1); CHECK(n0 == Number(0));
  Number n1(42); CHECK(n1.size() == 1);
  Number n2(Digit(100)); CHECK(n2.size() == 1);
  
  // string construction ======================================================
  string A = "100000000000000000011000000000000000000210000000000000000003";
  Number a(A);
  CHECK(a == Number("8H78Mi9W0018H78Mi9W0028H78Mi9W003", 64));
  
  // round-trip value equality (prerequisite test) ===========================
  const string B64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  Number rt1(B64, 64, B64);
  Number rt2(rt1.format(64, B64), 64, B64);
  CHECK(rt1 == rt2); // Numeric value should be preserved
  
  // simple round-trip tests =================================================
  CHECK(Number("AB", 64, B64) == Number("B", 64, B64)); // Leading zero lost
  CHECK(Number("0123", 10) == Number("123", 10)); // Base 10 leading zero
  CHECK(Number("A", 64, B64).format(64, B64) == "A"); // Single zero char
  
  // B64 round-trip with leading zero =========================================
  CHECK(B64 == "A"+Number(B64,64,B64).format(64,B64));

  // formatting ===============================================================
  Number f1(255); CHECK(f1.format(16) == "ff");
  Number f2(10); CHECK(f2.format(2) == "1010");
  CHECK(Number(0).format() == "0");
  
  // addition ================================================================
  string B = "fffffffffffffff0fffffffffffffff1fffffffffffffff2";
  Number b(B,16);
  Number c(b);
  c += b;
  CHECK(c.size() > b.size() || c != b);
  
  Number d = b + b;
  CHECK(d.size() >= b.size());
  
  Number e(1);
  ++e; CHECK(e == Number(2));
  e += Number(3); CHECK(e == Number(5));
  
  // comparison ==============================================================
  CHECK(Number(5) == Number(5));
  CHECK(Number(5) != Number(3));
  CHECK(Number(10) > Number(5));
  CHECK(Number(5) < Number(10));
  
  // assignment ==============================================================
  Number x(10);
  x = 20; CHECK(x == Number(20));
  Number y(30);
  x = y; CHECK(x == Number(30));
  
  // bits calculation ========================================================
  CHECK(Number(0).bits() == 0);
  CHECK(Number(1).bits() == 1);
  CHECK(Number(255).bits() == 8);
};
/*

/*
// multiply(a[1..p], b[1..q], base)                            // Operands
// containing rightmost digits at index 1
//   product=[1..p+q]                                        // Allocate
space
//   for result for b_i=1 to q                                          //
for
//   all digits in b
//     carry=0
//     for a_i=1 to p                                        // for all
digits
//     in a
//       product[a_i + b_i - 1] += carry + a[a_i] * b[b_i]
//       carry=product[a_i + b_i - 1] / base
//       product[a_i + b_i - 1]=product[a_i + b_i - 1] mod base
//     product[b_i + p]=carry                               // last Number
//     comes from final carry
//   return product


  //   operator string() const { return to_string(value); }
*/
// PI1000 =
//     "31415926535897932384626433832795028841971693993751058209749445923078164062"
//     "86208998628034825342117067982148086513282306647093844609550582231725359408"
//     "12848111745028410270193852110555964462294895493038196442881097566593344612"
//     "84756482337867831652712019091456485669234603486104543266482133936072602491"
//     "41273724587006606315588174881520920962829254091715364367892590360011330530"
//     "54882046652138414695194151160943305727036575959195309218611738193261179310"
//     "51185480744623799627495673518857527248912279381830119491298336733624406566"
//     "43086021394946395224737190702179860943702770539217176293176752384674818467"
//     "66940513200056812714526356082778577134275778960917363717872146844090122495"
//     "34301465495853710507922796892589235420199561121290219608640344181598136297"
//     "74771309960518707211349999998372978049951059731732816096318595024459455346"
//     "90830264252230825334468503526193118817101000313783875288658753320838142061"
//     "71776691473035982534904287554687311595628638823537875937519577818577805321"
//     "71226806613001927876611195909216420198"

} // namespace uniq