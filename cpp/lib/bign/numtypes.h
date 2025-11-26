#pragma once
#include <climits>

typedef unsigned char       u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef __uint128_t       u128;

typedef char         i8;
typedef short       i16;
typedef int         i32;
typedef long long   i64;
typedef __int128_t i128;

// platform dependent integer types
#if __WORDSIZE == 64
  typedef i64 integer;
  typedef u64 uinteger;
  typedef i128 ioverflow;
  typedef u128 uoverflow;
#else // __WORDSIZE == 32
  typedef i32 integer;
  typedef u32 uinteger;
  typedef i64 ioverflow;
  typedef u64 uoverflow;
#endif

#define BITS(n) sizeof(n)*CHAR_BIT

