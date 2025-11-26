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

struct BigDigit { // ======================================================== BigDigit
  digit value=0;

  BigDigit(digit d=0) { value=d; }
  
  BigDigit(const string &s, const int base=10, callback cb=flow, string map=DIGITS){ 
    assert(base>1 && base <= (int)map.length() && "BigDigit() invalid base");
    BigDigit v(0);
    for (auto c : s) {
      if(c==' ') continue;
      auto d = map.find(c); 
      assert(d != string::npos && d < (digit)base && "BigDigit() invalid character for base");
      try { 
        v = v*BigDigit(base)+ BigDigit(d) ; 
      } catch(const FlowException& e) { 
        cb(v.value, e.op); v = BigDigit(d); }; 
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

  BigDigit operator +  (digit n) const { BigDigit r = *this; r += n;  return r; }
  BigDigit operator -  (digit n) const { BigDigit r = *this; r -= n;  return r; }
  BigDigit operator *  (digit n) const { BigDigit r = *this; r *= n;  return r; }
  BigDigit operator /  (digit n) const { BigDigit r = *this; r /= n;  return r; }
  BigDigit operator %  (digit n) const { BigDigit r = *this; r %= n;  return r; }
  BigDigit operator >> (digit n) const { BigDigit r = *this; r >>= n; return r; }
  BigDigit operator << (digit n) const { BigDigit r = *this; r <<= n; return r; }
  
  BigDigit operator +  (int n) const { return *this + digit(n); }
  BigDigit operator -  (int n) const { return *this - digit(n); }
  BigDigit operator *  (int n) const { return *this * digit(n); }
  BigDigit operator /  (int n) const { return *this / digit(n); }
  BigDigit operator %  (int n) const { return *this % digit(n); }

  void operator &=  (digit n) { value=value & n; }
  void operator |=  (digit n) { value=value | n; }
  void operator ^=  (digit n) { value=value ^ n; }

  BigDigit& operator -- () { --value; return *this; }
  BigDigit operator -- (int) { BigDigit tmp = *this; --value; return tmp; }
  BigDigit& operator ++ () { ++value; return *this; }
  BigDigit operator ++ (int) { BigDigit tmp = *this; ++value; return tmp; }

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
  BigDigit operator &  (digit n) const { return BigDigit(value & n); }
  BigDigit operator |  (digit n) const { return BigDigit(value | n); }
  BigDigit operator ^  (digit n) const { return BigDigit(value ^ n); }
  BigDigit operator ~  () const { return BigDigit(~value); }

  bool operator [](digit k) const {return getbit(k); }
  // bool& operator [](digit k) {return setbit(k); }

  void operator = (digit d) { value=d; }
  void operator = (BigDigit d) { value=d.value; }
  
  operator digit() const { return value; }
  // operator string() const { return format(64); }
};

ostream& operator<<(ostream& os, const BigDigit& d) { return os << d.format(); }

digit factorial(BigDigit n) { return n <= BigDigit(1) ? 1 : factorial(n-BigDigit(1))*n; }

}; // namespace uniq

