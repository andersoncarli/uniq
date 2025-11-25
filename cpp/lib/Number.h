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
  Number(const Number& other) : digits(other.digits), signal(other.signal), originalLength(other.originalLength) {}
  Number(Number&& other) noexcept : digits(move(other.digits)), signal(other.signal), originalLength(other.originalLength) {
    other.digits = vector<Digit>(1, Digit(0));
    other.signal = 1;
    other.originalLength = 0;
  }
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

  void subtract(Digit d, int i=0) {
    if(i >= digits.size()) {
      throw FlowException(1, SUB);
    }
    digit borrow = 0;
    digits[i].value = digits[i].sub(d, borrow);
    if(borrow) {
      if(i+1 >= digits.size()) throw FlowException(borrow, SUB);
      subtract(Digit(borrow), i+1);
    }
  }

  Number& operator -= (const Number& n) {
    if(*this < n) throw FlowException(1, SUB);
    for (int i=0; i < n.size(); i++) 
      subtract(n[i], i);
    while(digits.size() > 1 && digits.back() == Digit(0)) digits.pop_back();
    return *this;
  }

  Number operator - (const Number& n) const { 
    Number r(*this); 
    return r -= n; 
  }

  Number& operator -- () { 
    subtract(Digit(1)); 
    return *this; 
  }

  Number operator -- (int) { 
    Number tmp = *this; 
    subtract(Digit(1)); 
    return tmp; 
  }

  Number& operator *= (const Number& n) {
    if(n == Number(0) || *this == Number(0)) {
      *this = Number(0);
      return *this;
    }
    if(n == Number(1)) {
      return *this;
    }
    // Algorithm: multiply(a[0..p-1], b[0..q-1])
    // product[0..p+q-1] - allocate space for result
    Number product(0);
    product.digits.resize(size() + n.size(), Digit(0));
    
    // for b_i = 0 to q-1 (all digits in b)
    for(int bi = 0; bi < n.size(); bi++) {
      digit carry = 0;
      // for a_i = 0 to p-1 (all digits in a)
      for(int ai = 0; ai < size(); ai++) {
        // product[ai + bi] += carry + a[ai] * b[bi]
        digit prod_ov = 0;
        digit prod = digits[ai].mul(n.digits[bi].value, prod_ov);
        // Add carry to product: total = prod + carry
        uoverflow total = (uoverflow)prod + (uoverflow)carry;
        digit total_digit = (digit)total;
        digit total_ov = total >> BITS(digit);
        // Add to product[ai + bi]
        digit sum_ov = 0;
        digit sum = product.digits[ai + bi].add(Digit(total_digit), sum_ov);
        product.digits[ai + bi].value = sum;
        // carry = overflow from (prod + carry) + overflow from addition to product
        carry = prod_ov + total_ov + sum_ov;
      }
      // product[bi + size()] = carry (final carry from inner loop)
      if(carry) {
        int final_idx = bi + size();
        if(final_idx >= product.size()) {
          product.digits.push_back(Digit(carry));
        } else {
          product.add(Digit(carry), final_idx);
        }
      }
    }
    // Remove leading zeros
    while(product.digits.size() > 1 && product.digits.back() == Digit(0)) 
      product.digits.pop_back();
    *this = product;
    return *this;
  }

  Number operator * (const Number& n) const { 
    Number r(*this); 
    return r *= n; 
  }

  pair<Number, Number> divide(const Number& n) const {
    if(n == Number(0)) throw FlowException(1, DIV);
    if(*this < n) return make_pair(Number(0), *this);
    if(n == Number(1)) return make_pair(*this, Number(0));
    
    Number quotient(0);
    Number remainder(*this);
    
    while(remainder >= n) {
      Number temp = n;
      Number multiple(1);
      while((temp * Number(2)) <= remainder) {
        temp = temp * Number(2);
        multiple = multiple * Number(2);
      }
      remainder = remainder - temp;
      quotient = quotient + multiple;
    }
    
    return make_pair(quotient, remainder);
  }

  Number& operator /= (const Number& n) {
    *this = divide(n).first;
    return *this;
  }

  Number operator / (const Number& n) const { 
    Number r(*this); 
    return r /= n; 
  }

  Number& operator %= (const Number& n) {
    *this = divide(n).second;
    return *this;
  }

  Number operator % (const Number& n) const { 
    Number r(*this); 
    return r %= n; 
  }

  Number& operator <<= (const Number& n) {
    if(n == Number(0)) return *this;
    Number power_of_2(1);
    for(Number i(0); i < n; ++i) {
      power_of_2 = power_of_2 * Number(2);
    }
    *this = *this * power_of_2;
    return *this;
  }

  Number operator << (const Number& n) const { 
    Number r(*this); 
    return r <<= n; 
  }

  Number& operator >>= (const Number& n) {
    if(n == Number(0)) return *this;
    Number power_of_2(1);
    for(Number i(0); i < n; ++i) {
      power_of_2 = power_of_2 * Number(2);
    }
    *this = *this / power_of_2;
    return *this;
  }

  Number operator >> (const Number& n) const { 
    Number r(*this); 
    return r >>= n; 
  }

  Number& operator &= (const Number& n) {
    int max_size = size() > n.size() ? size() : n.size();
    digits.resize(max_size, Digit(0));
    for(int i = 0; i < max_size; i++) {
      digit val1 = i < size() ? digits[i].value : 0;
      digit val2 = i < n.size() ? n.digits[i].value : 0;
      digits[i].value = val1 & val2;
    }
    while(digits.size() > 1 && digits.back() == Digit(0)) digits.pop_back();
    return *this;
  }

  Number operator & (const Number& n) const { 
    Number r(*this); 
    return r &= n; 
  }

  Number& operator |= (const Number& n) {
    int max_size = size() > n.size() ? size() : n.size();
    digits.resize(max_size, Digit(0));
    for(int i = 0; i < max_size; i++) {
      digit val1 = i < size() ? digits[i].value : 0;
      digit val2 = i < n.size() ? n.digits[i].value : 0;
      digits[i].value = val1 | val2;
    }
    while(digits.size() > 1 && digits.back() == Digit(0)) digits.pop_back();
    return *this;
  }

  Number operator | (const Number& n) const { 
    Number r(*this); 
    return r |= n; 
  }

  Number& operator ^= (const Number& n) {
    int max_size = size() > n.size() ? size() : n.size();
    digits.resize(max_size, Digit(0));
    for(int i = 0; i < max_size; i++) {
      digit val1 = i < size() ? digits[i].value : 0;
      digit val2 = i < n.size() ? n.digits[i].value : 0;
      digits[i].value = val1 ^ val2;
    }
    while(digits.size() > 1 && digits.back() == Digit(0)) digits.pop_back();
    return *this;
  }

  Number operator ^ (const Number& n) const { 
    Number r(*this); 
    return r ^= n; 
  }

  Number operator ~ () const {
    Number r(*this);
    for(int i = 0; i < r.size(); i++) {
      r.digits[i].value = ~r.digits[i].value;
    }
    while(r.digits.size() > 1 && r.digits.back() == Digit(0)) r.digits.pop_back();
    return r;
  }

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

  Number& operator = (Number&& other) noexcept {
    if(this != &other) {
      digits = move(other.digits);
      signal = other.signal;
      originalLength = other.originalLength;
      other.digits = vector<Digit>(1, Digit(0));
      other.signal = 1;
      other.originalLength = 0;
    }
    return *this;
  }

  bool isZero() const { return size() == 1 && digits[0] == Digit(0); }
  bool isOne() const { return size() == 1 && digits[0] == Digit(1); }
};

ostream& operator<<(ostream& os, const Number& n) { return os << n.format(); }

// utilities & math
Number pow(const Number& base, const Number& exp) {
  if(exp.isZero()) return Number(1);
  if(exp.isOne()) return base;
  if(exp % Number(2) == Number(0)) {
    Number half = pow(base, exp / Number(2));
    return half * half;
  }
  return base * pow(base, exp - Number(1));
}

Number gcd(const Number& a, const Number& b) {
  if(b.isZero()) return a;
  return gcd(b, a % b);
}

Number lcm(const Number& a, const Number& b) {
  return (a * b) / gcd(a, b);
}

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
  
  // subtraction ============================================================
  CHECK(Number(10) - Number(3) == Number(7));
  CHECK(Number(5) - Number(5) == Number(0));
  CHECK(Number(100) - Number(50) == Number(50));
  Number sub1(20); sub1 -= Number(7); CHECK(sub1 == Number(13));
  Number sub2(15); --sub2; CHECK(sub2 == Number(14));
  Number sub3(10); Number sub4 = sub3--; CHECK(sub4 == Number(10) && sub3 == Number(9));
  
  // multiplication =========================================================
  CHECK(Number(6) * Number(7) == Number(42));
  CHECK(Number(100) * Number(0) == Number(0));
  CHECK(Number(100) * Number(1) == Number(100));
  CHECK(Number(0) * Number(100) == Number(0));
  Number mul1(5); mul1 *= Number(4); CHECK(mul1 == Number(20));
  CHECK(Number(255) * Number(2) == Number(510));
  
  // division ===============================================================
  CHECK(Number(100) / Number(4) == Number(25));
  CHECK(Number(5) / Number(5) == Number(1));
  CHECK(Number(10) / Number(3) == Number(3));
  Number div1(50); div1 /= Number(5); CHECK(div1 == Number(10));
  CHECK_EXCEPTION(Number(10) / Number(0));
  
  // modulo ==================================================================
  CHECK(Number(100) % Number(7) == Number(2));
  CHECK(Number(10) % Number(10) == Number(0));
  CHECK(Number(15) % Number(4) == Number(3));
  Number mod1(100); mod1 %= Number(7); CHECK(mod1 == Number(2));
  
  // large number operations ================================================
  Number large1("1000000000000000000");
  Number large2("999999999999999999");
  CHECK((large1 - large2) == Number(1));
  CHECK(Number(1000000) * Number(1000000) == Number("1000000000000"));
  
  // edge cases ==============================================================
  CHECK(Number(1) - Number(1) == Number(0));
  CHECK(Number(0) - Number(0) == Number(0));
  CHECK(Number(1) * Number(0) == Number(0));
  CHECK(Number(0) * Number(1) == Number(0));
  CHECK(Number(1) / Number(1) == Number(1));
  CHECK(Number(0) / Number(5) == Number(0));
  CHECK(Number(1) % Number(1) == Number(0));
  
  // shift operations ========================================================
  CHECK(Number(8) << Number(2) == Number(32));
  CHECK(Number(32) >> Number(2) == Number(8));
  Number shift1(16); shift1 <<= Number(1); CHECK(shift1 == Number(32));
  Number shift2(16); shift2 >>= Number(1); CHECK(shift2 == Number(8));
  CHECK(Number(8) << Number(0) == Number(8));
  CHECK(Number(8) >> Number(0) == Number(8));
  
  // bitwise operations =======================================================
  CHECK((Number(5) & Number(3)) == Number(1));
  CHECK((Number(5) | Number(3)) == Number(7));
  CHECK((Number(5) ^ Number(3)) == Number(6));
  Number bw1(5); bw1 &= Number(3); CHECK(bw1 == Number(1));
  Number bw2(5); bw2 |= Number(2); CHECK(bw2 == Number(7));
  Number bw3(5); bw3 ^= Number(3); CHECK(bw3 == Number(6));
  CHECK((~Number(0)) != Number(0));
  
  // copy and move semantics ================================================
  Number orig(42);
  Number copied(orig);
  CHECK(copied == Number(42));
  Number moved(move(orig));
  CHECK(moved == Number(42));
  Number assign_test(10);
  assign_test = move(moved);
  CHECK(assign_test == Number(42));
  
  // utility functions =======================================================
  CHECK(pow(Number(2), Number(3)) == Number(8));
  CHECK(pow(Number(5), Number(2)) == Number(25));
  CHECK(pow(Number(10), Number(0)) == Number(1));
  CHECK(gcd(Number(48), Number(18)) == Number(6));
  CHECK(gcd(Number(17), Number(13)) == Number(1));
  CHECK(lcm(Number(12), Number(18)) == Number(36));
  CHECK(Number(0).isZero());
  CHECK(Number(1).isOne());
  CHECK(!Number(2).isZero());
  CHECK(!Number(2).isOne());
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
  
  Number pi(PI1000);
  CHECK(pi.size() > 1);
  CHECK(pi.bits() > 1000);
  
  // Round-trip test: format and parse back should be equal
  Number pi_roundtrip(pi.format());
  CHECK(pi_roundtrip == pi);
  
  Number pi2 = pi + Number(0);
  CHECK(pi2 == pi);
  
  Number pi_times_2 = pi * Number(2);
  Number pi_plus_pi = pi + pi;
  CHECK(pi_times_2 == pi_plus_pi);
  
  Number pi_div_2 = pi_times_2 / Number(2);
  CHECK(pi_div_2 == pi);
  
  Number one(1);
  CHECK(pi > one);
  CHECK(pi != one);
}

} // namespace uniq