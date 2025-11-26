#include "BigDecimal.h"
#include "test.h"
namespace bign {

TEST(BigDecimalConstruction) {
  // Default construction
  BigDecimal d0;
  CHECK(d0.getDecimalPlaces() == 0);
  CHECK(d0.isZero());
  
  // Integer construction
  BigDecimal d1(123);
  CHECK(d1.getDecimalPlaces() == 0);
  CHECK(d1.format() == "123");
  
  // String construction with decimal point
  BigDecimal d2("123.456");
  CHECK(d2.getDecimalPlaces() == 3);
  CHECK(d2.format() == "123.456");
  
  // Negative decimal
  BigDecimal d3("-123.456");
  CHECK(d3.getDecimalPlaces() == 3);
  CHECK(d3.format() == "-123.456");
  CHECK(d3.isNegative());
  
  // Zero with decimal places
  BigDecimal d4("0.000");
  CHECK(d4.isZero());
  CHECK(d4.format() == "0");
  
  // Integer string (no decimal point)
  BigDecimal d5("123");
  CHECK(d5.getDecimalPlaces() == 0);
  CHECK(d5.format() == "123");
  
  // Small decimal
  BigDecimal d6("0.1");
  CHECK(d6.getDecimalPlaces() == 1);
  CHECK(d6.format() == "0.1");
  
  // Large decimal
  BigDecimal d7("123456789.987654321");
  CHECK(d7.getDecimalPlaces() == 9);
  CHECK(d7.format() == "123456789.987654321");
}

TEST(BigDecimalArithmetic) {
  // Addition
  BigDecimal a("123.45");
  BigDecimal b("67.89");
  BigDecimal sum = a + b;
  CHECK(sum.format() == "191.34");
  
  // Subtraction
  BigDecimal diff = a - b;
  CHECK(diff.format() == "55.56");
  
  // Multiplication (123.45 * 67.89 = 8381.0205)
  BigDecimal prod = a * b;
  // Check that result has correct decimal places (2 + 2 = 4)
  CHECK(prod.getDecimalPlaces() == 4);
  // Check the numeric value - use comparison operator (handles alignment internally)
  BigDecimal expected("8381.0205");
  CHECK(prod == expected);
  
  // Division
  BigDecimal div = BigDecimal("100.0") / BigDecimal("4.0");
  CHECK(div.format() == "25");
  
  // Different decimal places
  BigDecimal x("10.5");
  BigDecimal y("2.25");
  BigDecimal z = x + y;
  CHECK(z.format() == "12.75");
  
  // Negative operations
  BigDecimal neg_a("-123.45");
  BigDecimal neg_b("-67.89");
  CHECK((neg_a + neg_b).format() == "-191.34");
  CHECK((neg_a - neg_b).format() == "-55.56");
}

TEST(BigDecimalComparison) {
  BigDecimal a("123.45");
  BigDecimal b("123.45");
  BigDecimal c("123.46");
  BigDecimal d("123.44");
  
  CHECK(a == b);
  CHECK(a < c);
  CHECK(a > d);
  CHECK(a <= b);
  CHECK(a >= b);
  CHECK(a != c);
  
  // Different decimal places
  BigDecimal e("123.450");
  CHECK(a == e); // Should normalize and compare equal
  
  // Negative comparisons
  BigDecimal neg("-123.45");
  CHECK(neg < a);
  CHECK(a > neg);
}

TEST(BigDecimalEdgeCases) {
  // Zero operations
  BigDecimal zero("0.0");
  BigDecimal one("1.0");
  
  CHECK(zero + one == one);
  CHECK(one - one == zero);
  CHECK(zero * one == zero);
  CHECK(zero / one == zero);
  
  // Very small decimals
  BigDecimal tiny("0.000000001");
  CHECK(tiny.getDecimalPlaces() == 9);
  CHECK(tiny.format() == "0.000000001");
  
  // Very large decimals
  BigDecimal huge("999999999999999999.999999999999999999");
  CHECK(huge.format() == "999999999999999999.999999999999999999");
  
  // Rounding
  BigDecimal r1("123.456");
  BigDecimal rounded = r1.round(2);
  CHECK(rounded.format() == "123.46");
  
  BigDecimal r2("123.454");
  BigDecimal rounded2 = r2.round(2);
  CHECK(rounded2.format() == "123.45");
  
  // Truncation
  BigDecimal t1("123.456");
  BigDecimal truncated = t1.truncate(2);
  CHECK(truncated.format() == "123.45");
  
  // Normalization (trailing zeros)
  BigDecimal n1("123.4500");
  CHECK(n1.format() == "123.45");
  
  BigDecimal n2("0.000");
  CHECK(n2.format() == "0");
}

TEST(BigDecimalFormat) {
  BigDecimal d1("123.456");
  CHECK(d1.format() == "123.456");
  CHECK(d1.format(2) == "123.46"); // Rounded
  
  BigDecimal d2("0.1");
  CHECK(d2.format() == "0.1");
  
  BigDecimal d3("0.0");
  CHECK(d3.format() == "0");
  
  BigDecimal d4("-123.456");
  CHECK(d4.format() == "-123.456");
  
  // Integer-like decimals
  BigDecimal d5("123.0");
  CHECK(d5.format() == "123");
}

TEST(BigDecimalPrecision) {
  BigDecimal d("123.456789");
  
  // Set more decimal places (should pad)
  d.setDecimalPlaces(10);
  CHECK(d.getDecimalPlaces() == 10);
  // Format normalizes trailing zeros, so check the value instead
  BigDecimal expected("123.4567890000");
  CHECK(d == expected);
  
  // Set fewer decimal places (should truncate)
  d.setDecimalPlaces(3);
  CHECK(d.getDecimalPlaces() == 3);
  CHECK(d.format() == "123.456");
}

TEST(BigDecimalInBigNumber) {
  // Test BigDecimal integration with BigNumber
  BigNumber n1("123.456");
  CHECK(n1.isDecimal());
  CHECK(n1.format() == "123.456");
  
  BigNumber n2("123");
  CHECK(n2.isCardinal());
  
  // Decimal + Cardinal should promote to Decimal
  BigNumber sum = n1 + n2;
  CHECK(sum.isDecimal());
  CHECK(sum.format() == "246.456");
  
  // Decimal + Integer should promote to Decimal
  BigNumber n3("-50");
  BigNumber sum2 = n1 + n3;
  CHECK(sum2.isDecimal());
  CHECK(sum2.format() == "73.456");
}

} // namespace bign
