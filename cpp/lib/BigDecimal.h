#pragma once
#include "BigInteger.h"
namespace uniq {

// BigDecimal - Fixed-point decimal arithmetic (base 10)
// Ideal for: Financial calculations, exact decimal representation
// 
// Implementation approach:
// - Store as BigInteger with implicit decimal point position
// - All operations maintain exact decimal precision
// - No rounding errors for decimal numbers

class BigDecimal : public BigInteger {
  int decimal_places; // Number of decimal places (fixed-point position)
  
public:
  BigDecimal() : BigInteger(), decimal_places(0) {}
  BigDecimal(const BigInteger& n, int places = 0) : BigInteger(n), decimal_places(places) {}
  // TODO: Implement constructors and operations
  // BigDecimal(const string s); // Parse "123.456" format
  // BigDecimal& operator += (const BigDecimal& n);
  // BigDecimal& operator -= (const BigDecimal& n);
  // BigDecimal& operator *= (const BigDecimal& n);
  // BigDecimal& operator /= (const BigDecimal& n);
  // BigDecimal round(int places) const;
  // BigDecimal truncate(int places) const;
  // string format(int places = -1) const;
  
  int getDecimalPlaces() const { return decimal_places; }
  void setDecimalPlaces(int places) { decimal_places = places; }
};

// ostream& operator<<(ostream& os, const BigDecimal& n) { return os << n.format(); }

} // namespace uniq
