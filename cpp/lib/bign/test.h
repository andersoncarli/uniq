#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <cassert>
#include <climits>
#include <algorithm>
#include <sstream>
#include <utility>
#include <cstdint>
#ifndef SIZE_MAX
#define SIZE_MAX ((size_t)-1)
#endif

// Simple test framework compatible with existing test files
namespace bign {
  extern int TEST_PASSED;
  extern int TEST_FAILED;
  extern int TEST_EXCEPTION;
  
  class Test {
  public:
    Test(bool passed, const std::string& expr, const std::string& /*func*/, 
         const std::string& file, int line) {
      if (passed) {
        TEST_PASSED++;
        std::cout << "\033[1;32m✓\033[0m";
      } else {
        TEST_FAILED++;
        std::cerr << "\033[1;31m✘\033[0m \033[90m(" << expr << ")\033[0m";
        std::cerr << " \033[90m(" << file << ":" << line << ")\033[0m\n";
        throw std::runtime_error("Test failed: " + expr);
      }
    }
    
    Test(const std::exception& ex, const std::string& /*func*/, 
         const std::string& file, int line) {
      if (std::string(ex.what()) == "bign::Fail") return;
      TEST_EXCEPTION++;
      std::cerr << "\033[1;31m💥\033[0m \033[90m(" << ex.what() << ")\033[0m";
      std::cerr << " \033[90m(" << file << ":" << line << ")\033[0m\n";
    }
  };
  
  struct TestCase {
    std::string name;
    std::function<void()> func;
    std::string file;
    int line;
    
    TestCase(const std::string& n, void (*f)(), const std::string& file, int line);
    
    void run() {
      std::cout << name << " ";
      try {
        func();
        std::cout << "\n";
      } catch (const std::exception& e) {
        Test(e, name, file, line);
        std::cout << "\n";
      } catch (...) {
        TEST_EXCEPTION++;
        std::cerr << "\033[1;31m💥\033[0m \033[90m(unknown exception)\033[0m\n";
        std::cout << "\n";
      }
    }
  };
  
  extern std::vector<TestCase*> TESTS;
  
  int runTests() {
    std::string line(80, '=');
    std::cout << "Running tests...\n" << line << "\n";
    
    for (auto* test : TESTS) {
      test->run();
    }
    
    std::cout << line << "\n";
    std::cout << "\033[1;32m✓\033[0m " << TEST_PASSED;
    if (TEST_FAILED > 0) std::cout << "  \033[1;31m✘\033[0m " << TEST_FAILED;
    if (TEST_EXCEPTION > 0) std::cout << "  \033[1;31m💥\033[0m " << TEST_EXCEPTION;
    std::cout << "\n\n";
    
    TESTS.clear();
    return TEST_FAILED + TEST_EXCEPTION;
  }
  
  // Compatibility aliases for existing tests
  using std::string;
  using std::vector;
  using std::pair;
  using std::make_pair;
  using std::min;
  using std::max;
  using std::move;
  using std::ostream;
  using std::ostringstream;
  
}

// Global using declarations for test files
using bign::string;
using bign::vector;
using bign::pair;
using bign::make_pair;
using bign::min;
using bign::max;
using bign::move;
using bign::ostream;
using bign::ostringstream;

// Macros compatible with existing tests
#define TEST(name) \
  void test_##name(); \
  static bign::TestCase test__##name(#name, &test_##name, __FILE__, __LINE__); \
  void test_##name()

#define CHECK(expr) \
  do { \
    try { \
      bign::Test((expr), #expr, __FUNCTION__, __FILE__, __LINE__); \
    } catch (...) { \
      throw; \
    } \
  } while(0)

#define CHECK_EXCEPTION(expr) \
  do { \
    try { \
      expr; \
      bign::Test(false, #expr " should throw", __FUNCTION__, __FILE__, __LINE__); \
    } catch (...) { \
      bign::Test(true, #expr " throws", __FUNCTION__, __FILE__, __LINE__); \
    } \
  } while(0)

