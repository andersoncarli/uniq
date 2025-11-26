#include "BigDecimal.h"
#include "test.h"
namespace uniq {

TEST(BigDecimal) {
  // Basic construction ======================================================
  BigDecimal d0;
  CHECK(d0 == BigDecimal(0));
  CHECK(d0.getDecimalPlaces() == 0);
  CHECK(d0.format() == "0");
  
  BigDecimal d1(123);
  CHECK(d1 == BigDecimal(123, 0));
  CHECK(d1.format() == "123");
  
  BigDecimal d2(BigInteger(456), 2);
  CHECK(d2.format() == "4.56");
  
  // String construction =====================================================
  BigDecimal s1("123.456");
  CHECK(s1.format() == "123.456");
  CHECK(s1.getDecimalPlaces() == 3);
  
  BigDecimal s2("0.5");
  CHECK(s2.format() == "0.5");
  CHECK(s2.getDecimalPlaces() == 1);
  
  BigDecimal s3("100");
  CHECK(s3.format() == "100");
  CHECK(s3.getDecimalPlaces() == 0);
  
  BigDecimal s4("0.001");
  CHECK(s4.format() == "0.001");
  CHECK(s4.getDecimalPlaces() == 3);
  
  BigDecimal s5(".5");
  CHECK(s5.format() == "0.5");
  
  BigDecimal s6("123.");
  CHECK(s6.format() == "123");
  
  BigDecimal s7("-123.456");
  CHECK(s7.format() == "-123.456");
  CHECK(s7.getDecimalPlaces() == 3);
  
  BigDecimal s8("-0.5");
  CHECK(s8.format() == "-0.5");
  
  // Addition ================================================================
  BigDecimal a1("10.5");
  BigDecimal a2("20.25");
  BigDecimal a3 = a1 + a2;
  CHECK(a3.format() == "30.75");
  
  BigDecimal a4("1.1");
  BigDecimal a5("2.2");
  BigDecimal a6 = a4 + a5;
  CHECK(a6.format() == "3.3");
  
  BigDecimal a7("0.1");
  BigDecimal a8("0.2");
  BigDecimal a9 = a7 + a8;
  CHECK(a9.format() == "0.3");
  
  BigDecimal a10("100.5");
  BigDecimal a11("50.25");
  a10 += a11;
  CHECK(a10.format() == "150.75");
  
  // Addition with different decimal places ==================================
  BigDecimal a12("1.5");
  BigDecimal a13("2.25");
  BigDecimal a14 = a12 + a13;
  CHECK(a14.format() == "3.75");
  
  BigDecimal a15("1.1");
  BigDecimal a16("2.22");
  BigDecimal a17 = a15 + a16;
  CHECK(a17.format() == "3.32");
  
  // Subtraction ==============================================================
  BigDecimal sub1("10.5");
  BigDecimal sub2("3.25");
  BigDecimal sub3 = sub1 - sub2;
  CHECK(sub3.format() == "7.25");
  
  BigDecimal sub4("1.0");
  BigDecimal sub5("0.1");
  BigDecimal sub6 = sub4 - sub5;
  CHECK(sub6.format() == "0.9");
  
  BigDecimal sub7("100.5");
  BigDecimal sub8("50.25");
  sub7 -= sub8;
  CHECK(sub7.format() == "50.25");
  
  BigDecimal sub8a("0.1");
  BigDecimal sub8b("0.2");
  BigDecimal sub8c = sub8a - sub8b;
  CHECK(sub8c.format() == "-0.1");
  
  // Multiplication ==========================================================
  BigDecimal m1("2.5");
  BigDecimal m2("4.0");
  BigDecimal m3 = m1 * m2;
  CHECK(m3.format() == "10");
  
  BigDecimal m4("1.5");
  BigDecimal m5("2.0");
  BigDecimal m6 = m4 * m5;
  CHECK(m6.format() == "3");
  
  BigDecimal m7("0.5");
  BigDecimal m8("0.5");
  BigDecimal m9 = m7 * m8;
  CHECK(m9.format() == "0.25");
  
  BigDecimal m10("10.5");
  BigDecimal m11("2.0");
  m10 *= m11;
  CHECK(m10.format() == "21");
  
  BigDecimal m12("0.1");
  BigDecimal m13("0.1");
  BigDecimal m14 = m12 * m13;
  CHECK(m14.format() == "0.01");
  
  // Division ================================================================
  BigDecimal div1("10.0");
  BigDecimal div2("2.0");
  BigDecimal div3 = div1 / div2;
  CHECK(div3.format() == "5");
  
  BigDecimal div4("1.0");
  BigDecimal div5("4.0");
  BigDecimal div6 = div4 / div5;
  CHECK(div6.format() == "0.25");
  
  BigDecimal div7("1.0");
  BigDecimal div8("3.0");
  BigDecimal div9 = div7 / div8;
  // Should have some decimal places
  CHECK(div9.getDecimalPlaces() > 0);
  
  BigDecimal div10("100.0");
  BigDecimal div11("25.0");
  div10 /= div11;
  CHECK(div10.format() == "4");
  
  // Round ===================================================================
  BigDecimal r1("123.456");
  BigDecimal r2 = r1.round(2);
  CHECK(r2.format() == "123.46");
  CHECK(r2.getDecimalPlaces() == 2);
  
  BigDecimal r3("123.454");
  BigDecimal r4 = r3.round(2);
  CHECK(r4.format() == "123.45");
  
  BigDecimal r5("123.455");
  BigDecimal r6 = r5.round(2);
  CHECK(r6.format() == "123.46");
  
  BigDecimal r7("0.999");
  BigDecimal r8 = r7.round(2);
  CHECK(r8.format() == "1");
  
  BigDecimal r9("1.5");
  BigDecimal r10 = r9.round(0);
  CHECK(r10.format() == "2");
  
  BigDecimal r11("1.4");
  BigDecimal r12 = r11.round(0);
  CHECK(r12.format() == "1");
  
  // Truncate ================================================================
  BigDecimal t1("123.456");
  BigDecimal t2 = t1.truncate(2);
  CHECK(t2.format() == "123.45");
  CHECK(t2.getDecimalPlaces() == 2);
  
  BigDecimal t3("123.999");
  BigDecimal t4 = t3.truncate(2);
  CHECK(t4.format() == "123.99");
  
  BigDecimal t5("1.5");
  BigDecimal t6 = t5.truncate(0);
  CHECK(t6.format() == "1");
  
  BigDecimal t7("0.123");
  BigDecimal t8 = t7.truncate(1);
  CHECK(t8.format() == "0.1");
  
  // Comparisons ============================================================
  CHECK(BigDecimal("1.5") == BigDecimal("1.5"));
  CHECK(BigDecimal("1.5") != BigDecimal("1.6"));
  CHECK(BigDecimal("1.5") < BigDecimal("2.0"));
  CHECK(BigDecimal("2.0") > BigDecimal("1.5"));
  CHECK(BigDecimal("1.5") <= BigDecimal("1.5"));
  CHECK(BigDecimal("1.5") >= BigDecimal("1.5"));
  
  CHECK(BigDecimal("1.5") < BigDecimal("1.6"));
  CHECK(BigDecimal("0.1") < BigDecimal("0.2"));
  CHECK(BigDecimal("10.5") > BigDecimal("10.4"));
  
  // Comparisons with different decimal places ==============================
  CHECK(BigDecimal("1.5") == BigDecimal("1.50"));
  CHECK(BigDecimal("1.0") == BigDecimal("1"));
  CHECK(BigDecimal("0.5") < BigDecimal("1"));
  
  // Negative numbers =======================================================
  BigDecimal n1("-123.456");
  CHECK(n1.format() == "-123.456");
  
  BigDecimal n2("-10.5");
  BigDecimal n3("-20.25");
  BigDecimal n4 = n2 + n3;
  CHECK(n4.format() == "-30.75");
  
  BigDecimal n5("-10.5");
  BigDecimal n6("20.25");
  BigDecimal n7 = n5 + n6;
  CHECK(n7.format() == "9.75");
  
  BigDecimal n8("-10.5");
  BigDecimal n9("20.25");
  BigDecimal n10 = n8 - n9;
  CHECK(n10.format() == "-30.75");
  
  BigDecimal n11("-2.5");
  BigDecimal n12("4.0");
  BigDecimal n13 = n11 * n12;
  CHECK(n13.format() == "-10");
  
  BigDecimal n14("-10.0");
  BigDecimal n15("2.0");
  BigDecimal n16 = n14 / n15;
  CHECK(n16.format() == "-5");
  
  // Edge cases ==============================================================
  CHECK(BigDecimal("0") == BigDecimal("0.0"));
  CHECK(BigDecimal("0.0") == BigDecimal("0"));
  CHECK(BigDecimal("0.00") == BigDecimal("0"));
  
  BigDecimal e1("0.1");
  BigDecimal e2("0.2");
  BigDecimal e3 = e1 + e2;
  CHECK(e3.format() == "0.3"); // Exact, no floating point errors
  
  BigDecimal e4("0.1");
  BigDecimal e5("0.1");
  BigDecimal e6 = e4 + e5;
  CHECK(e6.format() == "0.2");
  
  BigDecimal e7("1.0");
  BigDecimal e8("0.1");
  BigDecimal e9 = e7 - e8;
  CHECK(e9.format() == "0.9");
  
  // Large numbers ===========================================================
  BigDecimal l1("123456789.123456789");
  CHECK(l1.format() == "123456789.123456789");
  
  BigDecimal l2("999999999.999999999");
  BigDecimal l3("0.000000001");
  BigDecimal l4 = l2 + l3;
  CHECK(l4.format() == "1000000000");
  
  // setDecimalPlaces ========================================================
  BigDecimal sd1("123.456");
  sd1.setDecimalPlaces(2);
  CHECK(sd1.format() == "123.45");
  
  BigDecimal sd2("123.4");
  sd2.setDecimalPlaces(3);
  CHECK(sd2.format() == "123.4");
  
  BigDecimal sd3("123");
  sd3.setDecimalPlaces(2);
  CHECK(sd3.format() == "123");
  
  // Normalization (trailing zeros) =========================================
  BigDecimal norm1("123.4500");
  CHECK(norm1.format() == "123.45");
  CHECK(norm1.getDecimalPlaces() == 2);
  
  BigDecimal norm2("100.00");
  CHECK(norm2.format() == "100");
  
  BigDecimal norm3("0.100");
  CHECK(norm3.format() == "0.1");
  
  // Financial calculations ==================================================
  BigDecimal price("19.99");
  BigDecimal quantity("3");
  BigDecimal total = price * quantity;
  CHECK(total.format() == "59.97");
  
  BigDecimal tax_rate("0.08");
  BigDecimal subtotal("100.00");
  BigDecimal tax = subtotal * tax_rate;
  CHECK(tax.format() == "8");
  
  BigDecimal final_total = subtotal + tax;
  CHECK(final_total.format() == "108");
  
  // Precision preservation ==================================================
  BigDecimal p1("0.1");
  BigDecimal p2("0.2");
  BigDecimal p3 = p1 + p2;
  CHECK(p3.format() == "0.3"); // Exact, not 0.30000000000000004
  
  BigDecimal p4("1.0");
  BigDecimal p5("3.0");
  BigDecimal p6 = p4 / p5;
  // Should maintain precision
  CHECK(p6.getDecimalPlaces() > 0);
};

} // namespace uniq

