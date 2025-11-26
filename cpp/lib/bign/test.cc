#include "test.h"
#include <vector>

namespace bign {
  int TEST_PASSED = 0;
  int TEST_FAILED = 0;
  int TEST_EXCEPTION = 0;
  std::vector<TestCase*> TESTS;
  
  // Register test case
  TestCase::TestCase(const std::string& n, void (*f)(), const std::string& file, int line)
      : name(n), func(f), file(file), line(line) {
    TESTS.push_back(this);
  }
}

