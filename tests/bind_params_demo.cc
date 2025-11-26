// Demonstration: How parameters are passed to bound functions
// This shows that bind_lazy() captures arguments at bind time
// compile using ./build bind_params_demo
#include "uniq.h"
using namespace uniq;

int main() {
  out("=== Demonstrating Parameter Passing in bind_lazy() ===\n\n");
  
  // Example 1: Single parameter
  out("Example 1: Single parameter\n");
  int value = 0;
  auto handler1 = [&](int n) {
    value = n;
    out("Handler received: ", n, "\n");
  };
  
  // Bind with argument - argument is CAPTURED here
  voidfunction bound1 = bind_lazy(handler1, 42);
  out("Created voidfunction with 42 captured\n");
  
  // Execute later - no args needed! 42 was captured
  bound1();
  CHECK(value == 42);
  out("✓ Handler executed with captured value\n\n");
  
  // Example 2: Multiple parameters
  out("Example 2: Multiple parameters\n");
  string result;
  auto handler2 = [&](int a, string b, double c) {
    result = sstr(a, " ", b, " ", c);
    out("Handler received: ", a, ", ", b, ", ", c, "\n");
  };
  
  voidfunction bound2 = bind_lazy(handler2, 10, string("hello"), 3.14);
  out("Created voidfunction with (10, 'hello', 3.14) captured\n");
  
  bound2();
  CHECK(result == "10 hello 3.14");
  out("✓ Handler executed with all captured values\n\n");
  
  // Example 3: Event-like pattern
  out("Example 3: Event-like pattern\n");
  vector<u64> received;
  auto event_handler = [&](u64 n) {
    received.push_back(n);
    out("Event handler received: ", n, "\n");
  };
  
  // Simulate emit("event", value)
  for (u64 i = 1; i <= 5; i++) {
    voidfunction task = bind_lazy(event_handler, i);
    // In real event system, this would go to queue
    task();  // Execute immediately for demo
  }
  
  CHECK(received.size() == 5);
  CHECK(received[0] == 1);
  CHECK(received[4] == 5);
  out("✓ All events processed with correct values\n\n");
  
  // Example 4: Reference parameters
  out("Example 4: Reference parameters\n");
  int x = 0;
  auto ref_handler = [&](int& r) {
    r = 100;
    out("Reference handler modified value to: ", r, "\n");
  };
  
  voidfunction bound4 = bind_lazy(ref_handler, ref(x));
  bound4();
  CHECK(x == 100);
  out("✓ Reference parameter worked\n\n");
  
  out("=== All examples passed! ===\n");
  out("Conclusion: Parameters ARE passed - they're captured at bind time!\n");
  
  return 0;
}

