// All C++ Standard Library headers - https://en.cppreference.com/w/cpp/header
#pragma once

#if __cplusplus >= 199711L // ============================================ C++98
// Utilities
// #include <bitset> // bitset class template
// #include <csetjmp> // Macro (and function) that saves (and jumps) to an execution context
// #include <csignal> // Functions and macro constants for signal management
#include <cstdarg> // Handling of variable length argument lists
#include <cstddef> // Standard macros and typedefs
#include <cstdlib> // General purpose utilities: program control, dynamic memory allocation, random numbers, sort and search
#include <ctime>   // C-style time/date utilites
#include <functional> // Function objects, Function invocations, Bind operations and Reference wrappers
#include <typeinfo>   // Runtime type information utilities
#include <utility>    // Various utility components */

// Dynamic memory management
#include <memory> // High-level memory management utilities
#include <new>    // Low-level memory management utilities */

// Numeric limits
// #include <cfloat>  // Limits of floating-point types
#include <climits> // Limits of integral types
#include <limits>  // Uniform way to query properties of arithmetic types */

// Error handling
#include <cassert>   // Conditionally compiled macro that compares its argument to zero
#include <exception> // Exception handling utilities
#include <stdexcept> // Standard exception objects */

// Strings
#include <cctype>  // Functions to determine the category of narrow characters
#include <cstring> // Various narrow character string handling functions
#include <string>  // basic_string class template */

// Containers
#include <map>    // map and multimap associative containers
#include <queue>  // queue and priority_queue container adaptors
#include <set>    // set and multiset associative containers
#include <vector> // vector container */

// Iterators
#include <iterator> // Range iterators */

// Algorithms
#include <algorithm> // Algorithms that operate on ranges */

// Numerics
#include <cmath>    // Common mathematics functions
#include <numeric>  // Numeric operations on values in ranges */

// Input/output
#include <cstdio>    // C-style input-output functions */
#include <fstream>   // basic_fstream, basic_ifstream, basic_ofstream class templates and several typedefs
#include <iomanip>   // Helper functions to control the format of input and output
#include <ios>       // ios_base class, basic_ios class template and several typedefs
#include <iostream>  // Several standard stream objects
#include <ostream>   // basic_ostream, basic_iostream class templates and several typedefs
#include <sstream>   // basic_stringstream, basic_istringstream, basic_ostringstream class templates and several typedefs
// #include <strstream> // (deprecated in C++98) strstream, istrstream, ostrstream */
#endif

#if __cplusplus >= 201103L // ============================================ C++11
#include <chrono>           // time utilites
#include <tuple>            // tuple class template
#include <type_traits>      // Compile-time type information
// Numeric limits
#include <cstdint> // Fixed-width integer types and limits of other types */
// Containers
#include <array>        // array container
// Numerics
#include <random> // Random number generators and distributions
// Regular Expressions
#include <regex> // Classes, algorithms and iterators to support regular expression processing */
// Atomic Operations
#include <atomic> // Atomic operations */
// Thread support
#include <condition_variable> // Thread waiting conditions
#include <mutex>              // Mutual exclusion primitives
#include <thread>             // thread class and supporting functions */
#endif

#if __cplusplus >= 201402L // ============================================ C++14
#endif

#if __cplusplus >= 201703L // ============================================ C++17
#include <any>      // any class
#include <variant>  // variant class template */
/*/ Algorithms
#include <execution> // Predefined execution policies for parallel versions of the algorithms */
#endif

#if __cplusplus >= 202002L // ============================================ C++20
#include <version> // Supplies implementation-dependent library information */
#define C17
#endif

/*/ ================================================================= deprecated
#include <assert.h>   // sames as <cassert>
#include <errno.h>    // sames as <cerrno>
#include <float.h>    // sames as <cfloat>
#include <limits.h>   // sames as <climits>
#include <ctype.h>    // sames as <cctype>  in global namespace
#include <fenv.h>     // sames as <cfenv>   in global namespace
#include <inttypes.h> // sames as <cinttypes> in global namespace
#include <locale.h>   // sames as <clocale> in global namespace
#include <math.h>     // sames as <cmath>   in global namespace
#include <setjmp.h>   // sames as <csetjmp> in global namespace
#include <signal.h>   // sames as <csignal> in global namespace
#include <stdarg.h>   // sames as <cstdarg> in global namespace
#include <stddef.h>   // sames as <cstddef> in global namespace
#include <stdint.h>   // sames as <cstdint> in global namespace
#include <stdio.h>    // sames as <cstdio>  in global namespace
#include <stdlib.h>   // sames as <cstdlib> in global namespace
#include <string.h>   // sames as <cstring> in global namespace
#include <time.h>     // sames as <ctime>   in global namespace
#include <uchar.h>    // sames as <cuchar>  in global namespace
#include <wchar.h>    // sames as <cwchar>  in global namespace
#include <wctype.h>   // sames as <cwctype> in global namespace */

using namespace std;