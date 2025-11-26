#pragma once
#include "BigInteger.h"
namespace bign {

// BigDecimal - Fixed-point decimal arithmetic (base 10)
// Ideal for: Financial calculations, exact decimal representation
// 
// Implementation approach:
// - Store as BigInteger with implicit decimal point position
// - For "123.456", store as BigInteger(123456) with decimal_places=3
// - All operations maintain exact decimal precision
// - No rounding errors for decimal numbers

class BigDecimal : public BigInteger {
  int decimal_places; // Number of decimal places (fixed-point position)
  
  // Helper: Normalize decimal places (remove trailing zeros from fractional part)
  void normalize() {
    if(isZero()) {
      decimal_places = 0;
      return;
    }
    if(decimal_places == 0) return;
    
    // Count trailing zeros in the fractional part only
    // We need to check the last decimal_places digits
    BigInteger temp = static_cast<const BigInteger&>(*this);
    if(temp.isNegative()) temp = temp.abs();
    
    // Build divisor for the fractional part
    BigInteger frac_divisor(1);
    for(int i = 0; i < decimal_places; i++) {
      frac_divisor = frac_divisor * BigInteger(10);
    }
    
    // Extract fractional part
    BigInteger frac_part = temp % frac_divisor;
    
    // Count trailing zeros in fractional part
    int trailing_zeros = 0;
    if(frac_part.isZero()) {
      // All fractional digits are zero - remove all decimal places
      trailing_zeros = decimal_places;
    } else {
      BigInteger check = frac_part;
      while(trailing_zeros < decimal_places && !check.isZero() && (check % BigInteger(10)).isZero()) {
        check = check / BigInteger(10);
        trailing_zeros++;
      }
    }
    
    if(trailing_zeros > 0 && trailing_zeros <= decimal_places) {
      BigInteger divisor(1);
      for(int i = 0; i < trailing_zeros; i++) {
        divisor = divisor * BigInteger(10);
      }
      BigInteger new_value = temp / divisor;
      if(static_cast<const BigInteger&>(*this).isNegative()) {
        new_value = new_value.negate();
      }
      static_cast<BigInteger&>(*this) = new_value;
      decimal_places -= trailing_zeros;
    }
  }
  
  // Helper: Align decimal places by scaling to common denominator
  static void alignDecimalPlaces(BigDecimal& a, BigDecimal& b) {
    if(a.decimal_places == b.decimal_places) return;
    
    int max_places = a.decimal_places > b.decimal_places ? a.decimal_places : b.decimal_places;
    
    if(a.decimal_places < max_places) {
      int diff = max_places - a.decimal_places;
      BigInteger scale(1);
      for(int i = 0; i < diff; i++) scale = scale * BigInteger(10);
      BigInteger scaled_value = static_cast<const BigInteger&>(a) * scale;
      // Create BigDecimal without normalizing (set decimal_places directly)
      static_cast<BigInteger&>(a) = scaled_value;
      a.decimal_places = max_places;
    }
    
    if(b.decimal_places < max_places) {
      int diff = max_places - b.decimal_places;
      BigInteger scale(1);
      for(int i = 0; i < diff; i++) scale = scale * BigInteger(10);
      BigInteger scaled_value = static_cast<const BigInteger&>(b) * scale;
      // Create BigDecimal without normalizing (set decimal_places directly)
      static_cast<BigInteger&>(b) = scaled_value;
      b.decimal_places = max_places;
    }
  }
  
public:
  BigDecimal() : BigInteger(), decimal_places(0) {}
  BigDecimal(const BigInteger& n, int places = 0) : BigInteger(n), decimal_places(places) {
    normalize();
  }
  BigDecimal(digit n, int places = 0) : BigInteger(n), decimal_places(places) {}
  
  BigDecimal(const string s) : BigInteger(), decimal_places(0) {
    if(s.empty()) {
      *this = BigDecimal(0);
      return;
    }
    
    bool negative = (s[0] == '-');
    string num_str = negative ? s.substr(1) : s;
    
    if(num_str.empty()) {
      *this = BigDecimal(0);
      return;
    }
    
    size_t dot_pos = num_str.find('.');
    if(dot_pos == string::npos) {
      // No decimal point - integer
      *this = BigDecimal(BigInteger(s), 0);
      return;
    }
    
    string int_part = num_str.substr(0, dot_pos);
    string frac_part = (dot_pos + 1 < num_str.length()) ? num_str.substr(dot_pos + 1) : "";
    
    if(int_part.empty() && frac_part.empty()) {
      *this = BigDecimal(0);
      return;
    }
    
    if(int_part.empty()) int_part = "0";
    
    // If fractional part is empty, treat as integer
    if(frac_part.empty()) {
      *this = BigDecimal(BigInteger(negative ? "-" + int_part : int_part), 0);
      return;
    }
    
    // Combine integer and fractional parts: "123" + "456" = "123456" with 3 decimal places
    string combined = int_part + frac_part;
    decimal_places = frac_part.length();
    
    BigInteger combined_int(combined);
    *this = BigDecimal(combined_int, decimal_places);
    
    if(negative) {
      static_cast<BigInteger&>(*this) = static_cast<BigInteger&>(*this).negate();
    }
    
    normalize();
  }

  // Arithmetic operations ====================================================
  BigDecimal& operator += (const BigDecimal& n) {
    BigDecimal a = *this;
    BigDecimal b = n;
    alignDecimalPlaces(a, b);
    
    static_cast<BigInteger&>(*this) = static_cast<BigInteger&>(a) + static_cast<BigInteger&>(b);
    decimal_places = a.decimal_places;
    // Normalize to remove trailing zeros from fractional part
    normalize();
    return *this;
  }

  BigDecimal operator + (const BigDecimal& n) const {
    BigDecimal r(*this);
    return r += n;
  }

  BigDecimal& operator -= (const BigDecimal& n) {
    BigDecimal a = *this;
    BigDecimal b = n;
    alignDecimalPlaces(a, b);
    
    static_cast<BigInteger&>(*this) = static_cast<BigInteger&>(a) - static_cast<BigInteger&>(b);
    decimal_places = a.decimal_places;
    normalize();
    return *this;
  }

  BigDecimal operator - (const BigDecimal& n) const {
    BigDecimal r(*this);
    return r -= n;
  }

  BigDecimal& operator *= (const BigDecimal& n) {
    static_cast<BigInteger&>(*this) = static_cast<BigInteger&>(*this) * static_cast<const BigInteger&>(n);
    decimal_places = decimal_places + n.decimal_places;
    // Don't normalize here - preserve precision for multiplication results
    return *this;
  }

  BigDecimal operator * (const BigDecimal& n) const {
    BigDecimal r(*this);
    return r *= n;
  }

  BigDecimal& operator /= (const BigDecimal& n) {
    if(n.isZero()) throw FlowException(1, DIV);
    
    // Scale up dividend to maintain precision
    int target_places = decimal_places > n.decimal_places ? decimal_places : n.decimal_places;
    int scale_factor = target_places + 10; // Extra precision for division
    
    BigInteger scaled_this = static_cast<const BigInteger&>(*this);
    BigInteger scale(1);
    for(int i = 0; i < scale_factor; i++) {
      scale = scale * BigInteger(10);
    }
    scaled_this = scaled_this * scale;
    
    BigInteger scaled_n = static_cast<const BigInteger&>(n);
    BigInteger scale_n(1);
    for(int i = 0; i < n.decimal_places; i++) {
      scale_n = scale_n * BigInteger(10);
    }
    scaled_n = scaled_n * scale_n; // Remove n's decimal places
    
    static_cast<BigInteger&>(*this) = scaled_this / scaled_n;
    decimal_places = scale_factor - n.decimal_places;
    normalize();
    return *this;
  }

  BigDecimal operator / (const BigDecimal& n) const {
    BigDecimal r(*this);
    return r /= n;
  }

  // Decimal-specific operations =============================================
  BigDecimal round(int places) const {
    if(places >= decimal_places) return *this;
    if(places < 0) return *this;
    
    BigInteger value = static_cast<const BigInteger&>(*this);
    if(value.isNegative()) value = value.abs();
    
    // Calculate how many digits to remove
    int digits_to_remove = decimal_places - places;
    
    // Build divisor to remove the digits we don't want
    BigInteger remove_scale(1);
    for(int i = 0; i < digits_to_remove; i++) {
      remove_scale = remove_scale * BigInteger(10);
    }
    
    // Extract the digit we're rounding on (the first digit being removed)
    BigInteger temp = value / (remove_scale / BigInteger(10));
    BigInteger rounding_digit = temp % BigInteger(10);
    
    // Remove the digits we don't want
    BigInteger rounded = value / remove_scale;
    
    // Round up if needed
    if(rounding_digit >= BigInteger(5)) {
      rounded = rounded + BigInteger(1);
    }
    
    // Restore sign and create result
    if(static_cast<const BigInteger&>(*this).isNegative()) {
      rounded = rounded.negate();
    }
    
    return BigDecimal(rounded, places);
  }

  BigDecimal truncate(int places) const {
    if(places >= decimal_places) return *this;
    if(places < 0) return *this;
    
    BigInteger value = static_cast<const BigInteger&>(*this);
    
    // Calculate how many digits to remove
    int digits_to_remove = decimal_places - places;
    
    // Build divisor to remove the digits we don't want
    BigInteger remove_scale(1);
    for(int i = 0; i < digits_to_remove; i++) {
      remove_scale = remove_scale * BigInteger(10);
    }
    
    // Truncate by removing the unwanted digits
    BigInteger truncated = value / remove_scale;
    
    return BigDecimal(truncated, places);
  }

  string format(int places = -1) const {
    if(places < 0) {
      // Use current decimal places, but round/truncate based on normalization
      places = decimal_places;
    }
    
    // If places specified, round first
    BigDecimal to_format = *this;
    if(places >= 0 && places < decimal_places) {
      to_format = round(places);
    } else if(places > decimal_places) {
      to_format.setDecimalPlaces(places);
    }
    
    BigInteger value = static_cast<const BigInteger&>(to_format);
    bool negative = value.isNegative();
    if(negative) value = value.abs();
    
    string int_str = value.format();
    int actual_places = to_format.decimal_places;
    
    if(actual_places == 0) {
      return negative ? "-" + int_str : int_str;
    }
    
    // Pad with zeros if needed to have enough digits for decimal places
    while((int)int_str.length() < actual_places) {
      int_str = "0" + int_str;
    }
    
    if((int)int_str.length() == actual_places) {
      // All digits are fractional
      string result = "0." + int_str;
      // Remove trailing zeros
      while(result.length() > 1 && result.back() == '0') {
        result.pop_back();
      }
      if(result.back() == '.') result.pop_back();
      return negative ? "-" + result : result;
    }
    
    size_t dot_pos = int_str.length() - actual_places;
    if(dot_pos >= int_str.length()) {
      // Safety: shouldn't happen, but handle it
      return negative ? "-" + int_str : int_str;
    }
    
    string int_part = int_str.substr(0, dot_pos);
    string frac_part = int_str.substr(dot_pos);
    
    if(int_part.empty()) int_part = "0";
    
    string result = int_part + "." + frac_part;
    
    // Remove trailing zeros
    while(result.length() > 1 && result.back() == '0' && result[result.length()-2] != '.') {
      result.pop_back();
    }
    if(result.back() == '.') result.pop_back();
    
    return negative ? "-" + result : result;
  }

  int getDecimalPlaces() const { return decimal_places; }
  void setDecimalPlaces(int places) { 
    if(places < 0) return; // Invalid
    if(places < decimal_places) {
      // Truncate
      BigInteger scale(1);
      for(int i = 0; i < (decimal_places - places); i++) {
        scale = scale * BigInteger(10);
      }
      static_cast<BigInteger&>(*this) = static_cast<BigInteger&>(*this) / scale;
    } else if(places > decimal_places) {
      // Pad with zeros
      BigInteger scale(1);
      for(int i = 0; i < (places - decimal_places); i++) {
        scale = scale * BigInteger(10);
      }
      static_cast<BigInteger&>(*this) = static_cast<BigInteger&>(*this) * scale;
    }
    decimal_places = places;
    // Don't normalize when explicitly setting decimal places - user wants this precision
  }
  
  // Comparison operators (inherit from BigInteger, but need to align decimals)
  bool operator == (const BigDecimal& n) const {
    BigDecimal a = *this;
    BigDecimal b = n;
    alignDecimalPlaces(a, b);
    return static_cast<const BigInteger&>(a) == static_cast<const BigInteger&>(b);
  }
  
  bool operator < (const BigDecimal& n) const {
    BigDecimal a = *this;
    BigDecimal b = n;
    alignDecimalPlaces(a, b);
    return static_cast<const BigInteger&>(a) < static_cast<const BigInteger&>(b);
  }
  
  bool operator > (const BigDecimal& n) const { return n < *this; }
  bool operator <= (const BigDecimal& n) const { return !(n < *this); }
  bool operator >= (const BigDecimal& n) const { return !(*this < n); }
  bool operator != (const BigDecimal& n) const { return !(*this == n); }
};

ostream& operator<<(ostream& os, const BigDecimal& n) { return os << n.format(); }

} // namespace bign
