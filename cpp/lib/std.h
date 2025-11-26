// All C++ Standard Library headers - https://en.cppreference.com/w/cpp/header
#pragma once

#if __cplusplus >= 199711L // ============================================ C++98
// Utilities
// #include <bitset> // (~1ms) bitset class template
// #include <csetjmp> // (~0.1ms) Macro (and function) that saves (and jumps) to an execution context
// #include <csignal> // (~0.1ms) Functions and macro constants for signal management
#include <cstdarg> // (~0.1ms) Handling of variable length argument lists
#include <cstddef> // (~0.1ms) Standard macros and typedefs
#include <cstdlib> // (~0.5ms) General purpose utilities: program control, dynamic memory allocation, random numbers, sort and search
#include <ctime>   // (~0.2ms) C-style time/date utilites
#include <functional> // (~15ms) Function objects, Function invocations, Bind operations and Reference wrappers
#include <typeinfo>   // (~1ms) Runtime type information utilities
#include <utility>    // (~1ms) Various utility components */

// Dynamic memory management
#include <memory> // (~12ms) High-level memory management utilities
#include <new>    // (~0.5ms) Low-level memory management utilities */

// Numeric limits
// #include <cfloat>  // (~0.1ms) Limits of floating-point types
#include <climits> // (~0.1ms) Limits of integral types
#include <limits>  // (~1ms) Uniform way to query properties of arithmetic types */

// Error handling
#include <cassert>   // (~0.1ms) Conditionally compiled macro that compares its argument to zero
// #include <cerrno>    // (~0.1ms) Macro containing the last error number
#include <exception> // (~1ms) Exception handling utilities
#include <stdexcept> // (~1ms) Standard exception objects */

// Strings
#include <cctype>  // (~0.1ms) Functions to determine the category of narrow characters
#include <cstring> // (~0.2ms) Various narrow character string handling functions
// #include <cwchar>  // (~0.2ms) Various wide and multibyte string handling functions
// #include <cwctype> // (~0.1ms) Functions to determine the catagory of wide characters
#include <string>  // (~10ms) basic_string class template */

// Containers
// #include <deque>  // (~5ms) deque container
// #include <list>   // (~5ms) list container
#include <map>    // (~8ms) map and multimap associative containers
#include <queue>  // (~3ms) queue and priority_queue container adaptors
#include <set>    // (~8ms) set and multiset associative containers
// #include <stack>  // (~2ms) stack container adaptor
#include <vector> // (~5ms) vector container */

// Iterators
#include <iterator> // (~2ms) Range iterators */

// Algorithms
#include <algorithm> // (~20ms) Algorithms that operate on ranges */

// Numerics
#include <cmath>    // (~3ms) Common mathematics functions
// #include <complex>  // (~5ms) Complex number type
#include <numeric>  // (~5ms) Numeric operations on values in ranges
// #include <valarray> // (~8ms) Class for representing and manipulating arrays of values */

// Localization
// #include <clocale> // (~0.2ms) C localization utilities
// #include <locale>  // (~10ms) Localization utilities */

// Input/output
#include <cstdio>    // (~0.5ms) C-style input-output functions */
#include <fstream>   // (~25ms) basic_fstream, basic_ifstream, basic_ofstream class templates and several typedefs
// #include <iomanip>   // (~5ms) Helper functions to control the format of input and output
#include <ios>       // (~15ms) ios_base class, basic_ios class template and several typedefs
// #include <iosfwd>    // (~0.1ms) Forward declarations of all classes in the input/output
#include <iostream>  // (~30ms) Several standard stream objects
// #include <istream>   // (~15ms) basic_istream class template and several typedefs
#include <ostream>   // (~25ms) basic_ostream, basic_iostream class templates and several typedefs
#include <sstream>   // (~20ms) basic_stringstream, basic_istringstream, basic_ostringstream class templates and several typedefs
// #include <streambuf> // (~10ms) basic_streambuf class template
// #include <strstream> // (deprecated in C++98) strstream, istrstream, ostrstream */
#endif

#if __cplusplus >= 201103L // ============================================ C++11
#include <chrono>           // (~12ms) time utilites
// #include <initializer_list> // (~0.5ms) initializer_list class template
// #include <tuple>            // (~8ms) tuple class template
#include <type_traits>      // (~10ms) Compile-time type information
// #include <typeindex>        // (~2ms) type_index */
// Dynamic memory management
// #include <scoped_allocator> // (~5ms) Nested allocator class */
// Numeric limits
// #include <cinttypes> // (~0.2ms) Formatting macros, intmax_t and uintmax_t math and conversions
#include <cstdint> // (~0.2ms) Fixed-width integer types and limits of other types */
// Error handling
// #include <system_error> // (~5ms) Defines error_code, a platform-dependent error code */
// Strings
// #include <cuchar> // (~0.2ms) C-style Unicode character conversion functions */
// Containers
#include <array>        // (~2ms) array container
// #include <forward_list> // (~3ms) forward_list container
// #include <unordered_map> // (~10ms) unordered_map and unordered_multimap unordered associative containers
// #include <unordered_set> // (~10ms) unordered_set and unordered_multiset unordered associative containers */
// Numerics
// #include <cfenv>  // (~0.5ms) Floating-point environment access functions
#include <random> // (~15ms) Random number generators and distributions
// #include <ratio>  // (~2ms) Compile-time rational arithmetic */
// Localization
// #include <codecvt> // (~8ms) Unicode conversion facilities (deprecated in C++17) */
// Regular Expressions
#include <regex> // (~25ms) Classes, algorithms and iterators to support regular expression processing */
// Atomic Operations
#include <atomic> // (~8ms) Atomic operations */
// Thread support
#include <condition_variable> // (~12ms) Thread waiting conditions
// #include <future>             // (~15ms) Primitives for asynchronous computations
#include <mutex>              // (~10ms) Mutual exclusion primitives
#include <thread>             // (~20ms) thread class and supporting functions */
#endif

#if __cplusplus >= 201402L // ============================================ C++14
// Thread support
// #include <shared_mutex> // (~12ms) Shared mutual exclusion primitives */
#endif

#if __cplusplus >= 201703L // ============================================ C++17
#include <any>      // (~15ms) any class
// #include <optional> // (~5ms) optional class template
#include <variant>  // (~18ms) variant class template */
// Dynamic memory management
// #include <memory_resource> // (~8ms) Polymorphic allocators and memory resources */
// Strings
// #include <charconv>    // (~5ms) to_chars and from_chars
// #include <string_view> // (~3ms) basic_string_view class template */
// Filesystem
// #include <filesystem> // (~15ms) path class and supporting functions */
/*/ Algorithms
// #include <execution> // (~10ms) Predefined execution policies for parallel versions of the algorithms */
#endif

#if __cplusplus >= 202002L // ============================================ C++20
// #include <compare>         // (~3ms) Three-way comparison operator support
// #include <concepts>        // (~8ms) Fundamental library concepts
// #include <coroutine>       // (~15ms) Coroutines library
// #include <source_location> // (~2ms) Supplies means to obtain source code location
#include <version> // (~0.1ms) Supplies implementation-dependent library information */
// Strings
// #include <format> // (~20ms) Formatting library including format */
// Containers
// #include <span> // (~2ms) span view */
// Ranges
// #include <ranges> // (~25ms) Range access, primitives, requirements, utilities and adaptors */
// Numerics
// #include <bit>     // (~3ms) Bit manipulation functions
// #include <numbers> // (~2ms) Math constants */
// Input/output
// #include <syncstream> // (~8ms) basic_osyncstream, basic_syncbuf, and typedefs */
// Thread support
// #include <barrier>    // (~10ms) Barriers */
// #include <latch>      // (~5ms) Latches
// #include <semaphore>  // (~8ms) Semaphores
// #include <stop_token> // (~5ms) Stop tokens for jthread
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