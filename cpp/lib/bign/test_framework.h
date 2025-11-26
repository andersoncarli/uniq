#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <cstring>

namespace bign_test {

// Simple test framework - no external dependencies
class TestFramework {
private:
    static int passed;
    static int failed;
    static int exceptions;
    
    struct TestCase {
        std::string name;
        std::function<void()> func;
        std::string file;
        int line;
        
        TestCase(const std::string& n, std::function<void()> f, const std::string& file, int line)
            : name(n), func(f), file(file), line(line) {}
    };
    
    static std::vector<TestCase> tests;
    
public:
    static void registerTest(const std::string& name, std::function<void()> func, 
                            const std::string& file, int line) {
        tests.emplace_back(name, func, file, line);
    }
    
    static void runTests() {
        std::cout << "Running tests...\n";
        std::cout << std::string(80, '=') << "\n";
        
        for (const auto& test : tests) {
            std::cout << test.name << " ";
            try {
                test.func();
                std::cout << "\033[1;32m✓\033[0m";
                passed++;
            } catch (const std::exception& e) {
                std::cout << "\033[1;31m✘\033[0m";
                std::cout << " \033[90m(" << test.file << ":" << test.line << ")\033[0m\n";
                std::cout << "  Error: " << e.what() << "\n";
                failed++;
            } catch (...) {
                std::cout << "\033[1;31m💥\033[0m";
                std::cout << " \033[90m(" << test.file << ":" << test.line << ")\033[0m\n";
                exceptions++;
            }
            std::cout << "\n";
        }
        
        std::cout << std::string(80, '=') << "\n";
        std::cout << "\033[1;32m✓\033[0m " << passed;
        if (failed > 0) std::cout << "  \033[1;31m✘\033[0m " << failed;
        if (exceptions > 0) std::cout << "  \033[1;31m💥\033[0m " << exceptions;
        std::cout << "\n\n";
        
        tests.clear();
    }
    
    static int getExitCode() {
        return (failed + exceptions > 0) ? 1 : 0;
    }
};

int TestFramework::passed = 0;
int TestFramework::failed = 0;
int TestFramework::exceptions = 0;
std::vector<TestFramework::TestCase> TestFramework::tests;

// Test assertion class
class TestAssertion {
private:
    bool result;
    std::string expr;
    std::string file;
    int line;
    
public:
    TestAssertion(bool r, const std::string& e, const std::string& f, int l)
        : result(r), expr(e), file(f), line(l) {
        if (!result) {
            std::cerr << "\033[1;31m✘\033[0m \033[90m(" << expr << ")\033[0m";
            throw std::runtime_error("Test assertion failed: " + expr);
        }
        std::cout << "\033[1;32m✓\033[0m";
    }
};

// Macros
#define TEST(name) \
    void test_##name(); \
    struct TestReg_##name { \
        TestReg_##name() { \
            bign_test::TestFramework::registerTest(#name, test_##name, __FILE__, __LINE__); \
        } \
    }; \
    static TestReg_##name test_reg_##name; \
    void test_##name()

#define CHECK(expr) \
    do { \
        if (!(expr)) { \
            throw std::runtime_error("CHECK failed: " #expr); \
        } \
    } while(0)

#define CHECK_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            throw std::runtime_error("CHECK_EQ failed: " #a " != " #b); \
        } \
    } while(0)

#define CHECK_NE(a, b) \
    do { \
        if ((a) == (b)) { \
            throw std::runtime_error("CHECK_NE failed: " #a " == " #b); \
        } \
    } while(0)

} // namespace bign_test

