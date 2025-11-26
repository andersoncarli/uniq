#pragma once
#include "numtypes.h"
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include <cassert>
#include <climits>

namespace bign {

typedef uinteger digit;
// integer type is available from numtypes.h in global namespace

enum DigitOp{ ADD, SUB, MUL, DIV, MOD, SHR, SHL };
const std::string OpNames [] = { "add", "sub", "mul", "div", "mod", "shr", "shl" };

struct FlowException : public std::exception {
  digit value; 
  DigitOp op;
  std::string msg;
  
  FlowException(digit v, DigitOp op_) : value(v), op(op_) {
    msg = OpNames[op] + ":" + "overflow:" + std::to_string(value);
  }

  const char * what () const throw () {
    return msg.c_str(); 
  }
};

typedef std::function<digit(const digit, const DigitOp)> callback;

inline digit flow(const digit o, const DigitOp op) { if(o) throw FlowException(o, op); return o; };

const std::string DIGITS = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ><";

struct BigDigit {
  digit value=0;

  BigDigit(digit d=0) { value=d; }
  
  BigDigit(const std::string &s, const int base=10, callback cb=flow, std::string map=DIGITS){ 
    assert(base>1 && base <= (int)map.length() && "BigDigit() invalid base");
    BigDigit v(0);
    for (auto c : s) {
      if(c==' ') continue;
      auto d = map.find(c); 
      assert(d != std::string::npos && d < (digit)base && "BigDigit() invalid character for base");
      try { 
        v = v*BigDigit(base)+ BigDigit(d) ; 
      } catch(const FlowException& e) { 
        cb(v.value, e.op); v = BigDigit(d); }; 
    };
    value = v.value;
  }

  std::string format(int base=10, std::string map=DIGITS) const {
    assert(base>1 && base <= (int)map.length() && "format() invalid base");
    if(value == 0) return std::string(1, map[0]);
    std::string s="";    
    digit v=value;
    while(v) { s += map[v % base]; v /= base; };
    std::reverse(s.begin(), s.end());
    return s;
  }
  
  inline digit size() const {
    digit i = 0, n=value; if(!n) return 0;
    for(digit k = __WORDSIZE/2; k>0; k /=2) 
      if (n >= (digit(1) << k)) { i += k; n >>= k; };
    return i+1;
  }

  inline bool getbit(digit k) const { return (value & (digit(1) << k)) != 0; }
  inline void setbit(digit k, bool v=1) { if(v) value |= (digit(1) << k); else value &= ~(digit(1) << k); }
  int distance(digit n) { int r=0, x=value^n; while (x > 0) { r += x & 1; x >>= 1; } return r; }

  inline digit add(digit d, digit &ov) { auto r=(uoverflow) value + d; ov=r>>BITS(digit); return r; };
  inline digit mul(digit d, digit &ov) { auto r=(uoverflow) value * d; ov=r>>BITS(digit); return r; };
  inline digit div(digit d, digit &ov) { auto r=(digit) value / d; ov=value % d; return r; };
  inline digit mod(digit d, digit &ov) { auto r=(digit) value % d; ov=value / d; return r; };
  inline digit sub(digit d, digit &ov) { auto r=(digit) value - d; ov=(d > value) ? d - value : 0; return r; };
  inline digit shl(digit d, digit &ov) { auto r=(uoverflow) value << d; ov=r >> BITS(digit); return r; };
  inline digit shr(digit d, digit &ov) { auto mask=((1 << d)-1); ov=value & mask; auto r=value >> d; return r;};
  
  inline digit rotl(int n) { return (value<<n) | (value>>(BITS(digit)-n)); }
  inline digit rotr(int n) { return (value>>n) | (value<<(BITS(digit)-n)); }
  inline digit log2() const { digit s = size(); return s > 0 ? s-1 : 0; }
  
  void operator  += (digit d) { digit o, v = add(d, o); flow(o, ADD); value = v; }
  void operator  -= (digit d) { digit o, v = sub(d, o); flow(o, SUB); value = v; }
  void operator  *= (digit d) { digit o, v = mul(d, o); flow(o, MUL); value = v; }
  void operator  /= (digit d) { digit o, v = div(d, o); flow(o, DIV); value = v; }
  void operator  %= (digit d) { digit o, v = mod(d, o); value = v; } 
  void operator >>= (digit d) { digit o, v = shr(d, o); flow(o, SHR); value = v; }
  void operator <<= (digit d) { digit o, v = shl(d, o); flow(o, SHL); value = v; }
  
  bool operator == (const BigDigit& d) const { return value == d.value; }
  bool operator != (const BigDigit& d) const { return value != d.value; }
  bool operator <  (const BigDigit& d) const { return value < d.value; }
  bool operator >  (const BigDigit& d) const { return value > d.value; }
  bool operator <= (const BigDigit& d) const { return value <= d.value; }
  bool operator >= (const BigDigit& d) const { return value >= d.value; }
  
  BigDigit operator + (const BigDigit& d) const { BigDigit r(*this); r += d.value; return r; }
  BigDigit operator - (const BigDigit& d) const { BigDigit r(*this); r -= d.value; return r; }
  BigDigit operator * (const BigDigit& d) const { BigDigit r(*this); r *= d.value; return r; }
  BigDigit operator / (const BigDigit& d) const { BigDigit r(*this); r /= d.value; return r; }
  BigDigit operator % (const BigDigit& d) const { BigDigit r(*this); r %= d.value; return r; }
};

} // namespace bign

