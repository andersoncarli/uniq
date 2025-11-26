#pragma once
#include "uniq.h"
namespace uniq{

typedef uinteger digit;

enum DigitOp{ ADD, SUB, MUL, DIV, MOD, SHR, SHL };
const string OpNames [] = { "add", "sub", "mul", "div", "mod", "shr", "shl" };

struct FlowException : public exception // ============== custom exception
{
  digit value; 
  DigitOp op;
  string msg;
  
  FlowException(digit v, DigitOp op_) : value(v), op(op_) {
    msg = OpNames[op] + ":" + "overflow:" + to_string(value);
  }

	const char * what () const throw () {
    return msg.c_str(); 
  }
};

typedef function<digit(const digit, const DigitOp)> callback;

inline digit flow(const digit o, const DigitOp op) { if(o) throw FlowException(o, op); return o; };

const string DIGITS = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ><";

struct Digit { // ======================================================== Digit
  digit value=0;

  Digit(digit d=0) { value=d; }
  
  Digit(const string &s, const int base=10, callback cb=flow, string map=DIGITS){ 
    assert(base>1 && base <= (int)map.length() && "Digit() invalid base");
    Digit v(0);
    for (auto c : s) {
      if(c==' ') continue;
      auto d = map.find(c); 
      assert(d != string::npos && d < (digit)base && "Digit() invalid character for base");
      try { 
        v = v*Digit(base)+ Digit(d) ; 
      } catch(const FlowException& e) { 
        cb(v.value, e.op); v = Digit(d); }; 
    };
    value = v.value;
  }

  string format(int base=10, string map=DIGITS) const {
    assert(base>1 && base <= (int)map.length() && "format() invalid base");
    if(value == 0) return string(1, map[0]);
    string s="";    
    digit v=value;
    while(v) { s += map[v % base]; v /= base; };
    reverse(s.begin(), s.end());
    return s;
  }
  
  inline void flowR(int bits, callback cb){ 
    digit v = value; digit mask = 1 << bits; 
    while(v) { 
      cb(v & mask, SHR); 
      v >>= bits; 
    }; 
  }
  // inline void flowL(int bits, callback cb){ digit v = value; digit mask = 1 << bits; while(v) { cb( v & mask ); v >>= bits; }; }

  inline digit size() const { // msb
    digit i = 0, n=value; if(!n) return 0;
    for(digit k = __WORDSIZE/2; k>0; k /=2) 
      if (n >= (digit(1) << k)) { i += k; n >>= k; };
    return i+1;
 }

  // union byteint { byte b[sizeof digit]; digit i; };
  // digit rehash(digit d){
  //   char a,b,c,d;
  //   digit r;
  //   //h = Hash(Reverse(not h.AsInt));
  //   // digit h = Hash(!d);
  //   a = h[0]; b = h[1]; c = h[2]; d = h[3];
  //   r[0] = b ^ c ^ d;
  //   r[1] = c ^ d ^ a;
  //   r[2] = d ^ a ^ b;
  //   r[3] = a ^ b ^ c;
  //   };

  // inline digit count(){ return __builtin_popcount(value);} // count of bits 1
  // inline digit parity(){ return __builtin_parity(value);} 
  // inline digit clz(){ return __builtin_clz(value);} // count leading zeros
  // inline digit ctz(){ return __builtin_ctz(value);} // count trailing zeros
  // inline digit size() { return __WORDSIZE-clz(); }
  inline bool getbit(digit k) const { return (value & (digit(1) << k)) != 0; }
  inline void setbit(digit k, bool v=1) { if(v) value |= (digit(1) << k); else value &= ~(digit(1) << k); }
  // inline bool getbits(digit position, digit count) { }
  int distance(digit n) { int r=0, x=value^n; while (x > 0) { r += x & 1; x >>= 1; } return r; }

  inline digit add(digit d, digit &ov) { auto r=(uoverflow) value + d; ov=r>>BITS(digit); return r; };
  inline digit mul(digit d, digit &ov) { auto r=(uoverflow) value * d; ov=r>>BITS(digit); return r; };
  inline digit div(digit d, digit &ov) { auto r=(digit) value / d; ov=value % d; return r; };
  inline digit mod(digit d, digit &ov) { auto r=(digit) value % d; ov=value / d; return r; };
  inline digit sub(digit d, digit &ov) { auto r=(digit) value - d; ov=(d > value) ? d - value : 0; return r; };
  inline digit shl(digit d, digit &ov) { auto r=(uoverflow) value << d; ov=r >> BITS(digit); return r; };
  inline digit shr(digit d, digit &ov) { auto mask=((1 << d)-1); ov=value & mask; auto r=value >> d; return r;};
  
  inline digit rotl(integer n) { return (value<<n) | (value>>(BITS(digit)-n)); }
  inline digit rotr(integer n) { return (value>>n) | (value<<(BITS(digit)-n)); }
  inline digit log2() const { digit s = size(); return s > 0 ? s-1 : 0; }
  // operators 
  void operator  += (digit d) { digit o, v = add(d, o); flow(o, ADD); value = v; }
  void operator  -= (digit d) { digit o, v = sub(d, o); flow(o, SUB); value = v; }
  void operator  *= (digit d) { digit o, v = mul(d, o); flow(o, MUL); value = v; }
  void operator  /= (digit d) { digit o, v = div(d, o); flow(o, DIV); value = v; }
  void operator  %= (digit d) { digit o, v = mod(d, o); value = v; } 
  void operator >>= (digit d) { digit o, v = shr(d, o); flow(o, SHR); value = v; }
  void operator <<= (digit d) { digit o, v = shl(d, o); flow(o, SHL); value = v; }

  Digit operator +  (digit n) const { Digit r = *this; r += n;  return r; }
  Digit operator -  (digit n) const { Digit r = *this; r -= n;  return r; }
  Digit operator *  (digit n) const { Digit r = *this; r *= n;  return r; }
  Digit operator /  (digit n) const { Digit r = *this; r /= n;  return r; }
  Digit operator %  (digit n) const { Digit r = *this; r %= n;  return r; }
  Digit operator >> (digit n) const { Digit r = *this; r >>= n; return r; }
  Digit operator << (digit n) const { Digit r = *this; r <<= n; return r; }
  
  Digit operator +  (int n) const { return *this + digit(n); }
  Digit operator -  (int n) const { return *this - digit(n); }
  Digit operator *  (int n) const { return *this * digit(n); }
  Digit operator /  (int n) const { return *this / digit(n); }
  Digit operator %  (int n) const { return *this % digit(n); }

  void operator &=  (digit n) { value=value & n; }
  void operator |=  (digit n) { value=value | n; }
  void operator ^=  (digit n) { value=value ^ n; }

  Digit& operator -- () { --value; return *this; }
  Digit operator -- (int) { Digit tmp = *this; --value; return tmp; }
  Digit& operator ++ () { ++value; return *this; }
  Digit operator ++ (int) { Digit tmp = *this; ++value; return tmp; }

  bool operator == (digit n) const { return value == n; }
  bool operator != (digit n) const { return value != n; }
  bool operator <  (digit n) const { return value <  n; }
  bool operator >  (digit n) const { return value >  n; }
  bool operator <= (digit n) const { return value <= n; }
  bool operator >= (digit n) const { return value >= n; }
  
  bool operator == (int n) const { return value == digit(n); }
  bool operator != (int n) const { return value != digit(n); }
  bool operator <  (int n) const { return value <  digit(n); }
  bool operator >  (int n) const { return value >  digit(n); }
  bool operator <= (int n) const { return value <= digit(n); }
  bool operator >= (int n) const { return value >= digit(n); }
  Digit operator &  (digit n) const { return Digit(value & n); }
  Digit operator |  (digit n) const { return Digit(value | n); }
  Digit operator ^  (digit n) const { return Digit(value ^ n); }
  Digit operator ~  () const { return Digit(~value); }

  bool operator [](digit k) const {return getbit(k); }
  // bool& operator [](digit k) {return setbit(k); }

  void operator = (digit d) { value=d; }
  void operator = (Digit d) { value=d.value; }
  
  operator digit() const { return value; }
  // operator string() const { return format(64); }
};

ostream& operator<<(ostream& os, const Digit& d) { return os << d.format(); }

digit factorial(Digit n) { return n <= Digit(1) ? 1 : factorial(n-Digit(1))*n; }

//* Tests ======================================================================
TEST(Digit){
  
  CHECK(sizeof(Digit) == sizeof(digit));
  // operators =================================================================
  Digit d(4);       CHECK(d == 4); CHECK(d.size()== 3);
  d=d-1;            CHECK(d == 3); CHECK(d.size()== 2);
  digit x=d+1; d=x; CHECK(x == 4);
  d--;              CHECK(d == 3);
  ++d;              CHECK(d == 4);
  --d;              CHECK(d == 3);
  
  // bit operations ===========================================================
  Digit b(5); CHECK(b.getbit(0) == 1); CHECK(b.getbit(1) == 0); CHECK(b.getbit(2) == 1);
  b.setbit(1, 1); CHECK(b == 7);
  b.setbit(0, 0); CHECK(b == 6);
  CHECK(b[0] == 0); CHECK(b[1] == 1); CHECK(b[2] == 1);
  
  // shift operations ==========================================================
  Digit s(8); s >>= 2; CHECK(s == 2);
  s <<= 3; CHECK(s == 16);
  Digit s2(16); CHECK((s2 >> digit(2)) == 4);
  Digit s3(4); CHECK((s3 << digit(2)) == 16);
  
  // distance ==================================================================
  CHECK(Digit(7).distance(5) == 1);
  CHECK(Digit(15).distance(0) == 4);

  // formating =================================================================
  CHECK(Digit(0).format() == "0"); // default map starts with "0"
  CHECK(Digit(0).format(16) == "0"); // default map
  CHECK(Digit(0).format(2) == "0"); // default map
  const string B64map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  CHECK(Digit(0).format(64, B64map) == "A"); // custom map
  
  const string B10 = "10000000000000000001";
  const string B16 = "8ac7230489e80001";
  const string B64 = "8H78Mi9W001";
 
  Digit big(B10); CHECK(big.size()== 64);
  CHECK(big.format() == B10);
  CHECK(big.format(16) == B16);
  CHECK(big.format(64) == B64);
  CHECK(Digit(B64,64) == big);
  
  Digit h(SIZE_MAX);
  CHECK(h.format(16) == "ffffffffffffffff");
  CHECK(h.format(64) == "f<<<<<<<<<<");
  
  CHECK(Digit(255).format(16) == "ff");
  CHECK(Digit(10).format(2) == "1010");

  // arithmetic operators =====================================================
  Digit a1(10); a1 += 5; CHECK(a1 == 15);
  Digit a2(20); a2 -= 7; CHECK(a2 == 13);
  Digit a3(6); a3 *= 7; CHECK(a3 == 42);
  Digit a4(100); a4 /= 4; CHECK(a4 == 25);
  Digit a5(100); a5 %= 7; CHECK(a5 == 2);
  
  CHECK((Digit(10) + 5) == 15);
  CHECK((Digit(20) - 7) == 13);
  CHECK((Digit(6) * 7) == 42);
  CHECK((Digit(100) / 4) == 25);
  CHECK((Digit(100) % 7) == 2);
  
  // bitwise operators =========================================================
  Digit bw1(5); bw1 &= 3; CHECK(bw1 == 1);
  Digit bw2(5); bw2 |= 2; CHECK(bw2 == 7);
  Digit bw3(5); bw3 ^= 3; CHECK(bw3 == 6);
  
  // exceptions =================================================================
  Digit ex(3);
  try { ex -= 4;       }  catch(FlowException& e) { CHECK(e.value == 1); };
  try { ex += SIZE_MAX; } catch(FlowException& e) { CHECK(e.value == 1); };
  try { ex /= 5;        } catch(FlowException& e) { CHECK(e.value == 3); };
  try { ex *= SIZE_MAX; } catch(FlowException& e) { CHECK(e.value == 2); };
  try { ex <<= 64;      } catch(FlowException& e) { CHECK(ex.value == 3); };
  try { ex >>= 2;       } catch(FlowException& e) { CHECK(ex.value == 3); };

  CHECK(ex == 3);
  
  // size and log2 =============================================================
  CHECK(Digit(0).size() == 0);
  CHECK(Digit(1).size() == 1);
  CHECK(Digit(2).size() == 2);
  CHECK(Digit(4).size() == 3);
  CHECK(Digit(8).size() == 4);
  CHECK(Digit(0).log2() == 0);
  CHECK(Digit(1).log2() == 0);
  CHECK(Digit(4).log2() == 2);
  
  // rotation ===================================================================
  CHECK(Digit(1).rotl(1) == (digit(1) << 1 | digit(1) >> (BITS(digit)-1)));
  CHECK(Digit(1).rotr(1) == (digit(1) >> 1 | digit(1) << (BITS(digit)-1)));
  
  // post-increment/decrement return values ====================================
  Digit d1(5); CHECK(d1++ == 5 && d1 == 6);
  Digit d2(5); CHECK(d2-- == 5 && d2 == 4);
  Digit d3(10); Digit d4 = d3++; CHECK(d4 == 10 && d3 == 11);
  Digit d5(10); Digit d6 = d5--; CHECK(d6 == 10 && d5 == 9);
  
  // edge cases =================================================================
  Digit e1(SIZE_MAX); CHECK(e1.size() > 0);
  Digit e2(SIZE_MAX-1); CHECK(e2.size() > 0);
  CHECK(Digit(0).format(2) == "0");
  CHECK(Digit(1).format(2) == "1");
  CHECK(Digit(SIZE_MAX).format(16).length() > 0);
};
}; // namespace uniq