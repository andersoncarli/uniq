#pragma once
#include "BigInteger.h"
namespace uniq {

// BigFloat - Floating-point arithmetic with arbitrary precision
// Ideal for: Scientific computing, wide range of values
//
// Implementation approach:
// - Store as: sign, mantissa (BigInteger), exponent (integer)
// - Base 2 or base 10 representation
// - Variable precision
// - Exponential notation support

class BigFloat {
  i8 sign;
  BigInteger mantissa; // Normalized mantissa
  i64 exponent;        // Exponent (base 2 or 10)
  int precision;        // Number of significant digits
  
public:
  BigFloat() : sign(1), mantissa(0), exponent(0), precision(0) {}
  // TODO: Implement constructors and operations
  // BigFloat(const BigInteger& n);
  // BigFloat(const string s); // Parse "1.23e10" format
  // BigFloat& operator += (const BigFloat& n);
  // BigFloat& operator -= (const BigFloat& n);
  // BigFloat& operator *= (const BigFloat& n);
  // BigFloat& operator /= (const BigFloat& n);
  // BigFloat round(int precision) const;
  // string format(int precision = -1) const;
  // string formatScientific() const;
  
  int getPrecision() const { return precision; }
  void setPrecision(int p) { precision = p; }
};

// ostream& operator<<(ostream& os, const BigFloat& n) { return os << n.format(); }

} // namespace uniq

