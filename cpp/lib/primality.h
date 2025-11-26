#pragma once
#include "Number.h"
#include <vector>
#include <random>
#include <cmath>
using namespace std;
namespace uniq {

vector<Number> sieve(int lim) {
  vector<bool> s(lim + 1, true);
  s[0] = s[1] = false;
  int sqrt_lim = (int)std::sqrt(lim);
  for(int i = 2; i <= sqrt_lim; i++) {
    if(s[i]) {
      for(int j = i * i; j <= lim; j += i) s[j] = false;
    }
  }
  vector<Number> primes;
  for(int i = 2; i <= lim; i++) {
    if(s[i]) primes.push_back(Number(i));
  }
  return primes;
}

int jacobiBig(const Number& a, const Number& n) {
  if(n <= 0 || (n % 2) == 0) return 0;
  Number aMod = ((a % n) + n) % n;
  Number nCopy = n;
  int r = 1;
  while(!aMod.isZero()) {
    while((aMod % 2) == 0) {
      aMod = aMod / 2;
      Number nMod8 = nCopy % 8;
      if(nMod8 == 3 || nMod8 == 5) r = -r;
    }
    Number temp = aMod;
    aMod = nCopy;
    nCopy = temp;
    if((aMod % 4) == 3 && (nCopy % 4) == 3) r = -r;
    aMod = aMod % nCopy;
  }
  return nCopy == 1 ? r : 0;
}

int leg2Big(const Number& n) {
  Number m8 = ((n % 8) + 8) % 8;
  if(m8 == 1 || m8 == 7) return 1;
  if(m8 == 3 || m8 == 5) return -1;
  return 0;
}

Number powMod(const Number& b, const Number& e, const Number& m) {
  if(m == 1) return 0;
  Number r(1);
  Number base = b % m;
  Number exp = e;
  while(!exp.isZero()) {
    if((exp % 2) == 1) r = (r * base) % m;
    exp = exp >> 1;
    base = (base * base) % m;
  }
  return r;
}

static vector<Number> primeBase;
static Number maxPrime;

void initPrimality(int sz = 200) {
  primeBase = sieve(sz * 10);
  if(primeBase.size() > sz) primeBase.resize(sz);
  if(!primeBase.empty()) maxPrime = primeBase.back();
}

bool isPrimeQR(const Number& n, int numTests = 10) {
  if(n < 2) return false;
  if(n == 2) return true;
  if((n % 2) == 0) return false;
  
  for(const auto& p : primeBase) {
    Number pSquared = p * p;
    if(pSquared > n) break;
    if((n % p) == 0) return false;
  }
  
  if(!maxPrime.isZero() && !maxPrime.isOne()) {
    Number mpSquared = maxPrime * maxPrime;
    if(n < mpSquared) return true;
  }
  
  Number nMinus1 = n - 1;
  if(nMinus1.isZero()) return false;
  Number nMinus1Over2 = nMinus1 / 2;
  
  int testsToRun = numTests < (int)primeBase.size() ? numTests : primeBase.size();
  for(int i = 0; i < testsToRun; i++) {
    Number p = primeBase[i];
    if(p == 2) {
      int pn = leg2Big(n);
      Number e = powMod(2, nMinus1Over2, n);
      int en = (e == nMinus1) ? -1 : ((e == 1) ? 1 : 0);
      if(en != pn) return false;
      continue;
    }
    Number pMinus1 = p - 1;
    Number pMinus1Over2 = pMinus1 / 2;
    Number pMinus1_val = p - 1;
    Number npRaw = powMod(n % p, pMinus1Over2, p);
    int npn = (npRaw == pMinus1_val) ? -1 : ((npRaw == 1) ? 1 : 0);
    int pn = jacobiBig(p, n);
    Number exponent = pMinus1Over2 * nMinus1Over2;
    int sign = ((exponent % 2) == 1) ? -1 : 1;
    int exp = sign * npn;
    if(pn != exp) return false;
    Number eu = powMod(p, nMinus1Over2, n);
    int eun = (eu == nMinus1) ? -1 : ((eu == 1) ? 1 : 0);
    if(eun != pn) return false;
  }
  
  vector<int> bases = {2, 3, 5, 7};
  for(int b : bases) {
    Number result = powMod(b, nMinus1, n);
    if(result != 1) return false;
  }
  return true;
}

bool isPrimeMR(const Number& n, int k = 10) {
  if(n < 2) return false;
  if(n == 2 || n == 3) return true;
  if((n % 2) == 0) return false;
  
  Number nMinus1 = n - 1;
  Number d = nMinus1;
  int r = 0;
  while((d % 2) == 0) {
    r++;
    d = d / 2;
  }
  
  // Use deterministic bases for large numbers
  vector<int> small_bases = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47};
  
  for(int i = 0; i < k; i++) {
    Number a(2);
    if(i < (int)small_bases.size()) {
      a = small_bases[i];
      if(a >= n) a = 2;
    } else {
      a = 2 + (i % 100);
      if(a >= n) a = 2;
    }
    
    Number x = powMod(a, d, n);
    if(x == 1 || x == nMinus1) continue;
    
    bool cont = false;
    for(int j = 0; j < r - 1; j++) {
      x = powMod(x, 2, n);
      if(x == nMinus1) { cont = true; break; }
    }
    if(!cont) return false;
  }
  return true;
}

Number firstFactor(const Number& n) {
  if(n < 2) return 0;
  if((n % 2) == 0) return 2;
  
  for(const auto& p : primeBase) {
    if((n % p) == 0) return p;
    Number pSquared = p * p;
    if(pSquared > n) break;
  }
  
  Number start = primeBase.empty() ? 3 : primeBase.back() + 2;
  if((start % 2) == 0) start = start + 1;
  
  for(Number i = start; i * i <= n; i = i + 2) {
    if((n % i) == 0) return i;
  }
  return n;
}

bool isPrimeTD(const Number& n) {
  if(n < 2) return false;
  if(n == 2) return true;
  if((n % 2) == 0) return false;
  return firstFactor(n) == n;
}

} // namespace uniq
