#include "BigPrime.h"
#include "Time.h"
#include <vector>
#include <iomanip>
using namespace std;
namespace uniq {

void runDemo() {
  initPrimality(200);
  
  cout << "\n" << string(80, '=') << "\n";
  cout << "Primality Test - MR vs QR Comparison for Large BigNumbers\n";
  cout << string(80, '=') << "\n\n";
  
  // Large primes
  vector<pair<BigNumber, string>> primes = {
    {BigNumber("2147483647"), "M31 (2^31-1)"},
    {BigNumber("37975227936943673922808872755445627854565536638199"), "RSA-100 Factor 1"},
    {BigNumber("40094690950920881030683735292761468389214899724061"), "RSA-100 Factor 2"},
  };
  
  // Large composites  
  vector<pair<BigNumber, string>> composites = {
    {BigNumber("172947529"), "Carmichael"},
    {BigNumber("1522605027922533360535618378132637429718068114961380688657908494580122963258952897654000350692006139"), "RSA-100"},
  };
  
  cout << left << setw(30) << "BigNumber" << setw(6) << "QR" << setw(6) << "MR" 
       << setw(15) << "QR (ms)" << setw(15) << "MR (ms)" << setw(15) << "Speedup" << "\n";
  cout << string(80, '-') << "\n";
  
  int total = 0, passed = 0;
  
  // Test primes
  for(const auto& [n, name] : primes) {
    total++;
    bool qr_result = false, mr_result = false;
    double tqr_ms = 0, tmr_ms = 0;
    
    try {
      auto t0 = CpuTime();
      qr_result = isPrimeQR(n, 10);
      auto tqr = CpuTime() - t0;
      tqr_ms = tqr(MILI);
    } catch(const exception& e) {
      cout << left << setw(30) << name << "QR ERROR: " << e.what() << "\n";
      continue;
    }
    
    try {
      auto t1 = CpuTime();
      mr_result = isPrimeMR(n, 10);
      auto tmr = CpuTime() - t1;
      tmr_ms = tmr(MILI);
    } catch(const exception& e) {
      cout << left << setw(30) << name << "MR ERROR: " << e.what() << "\n";
      continue;
    }
    
    bool agree = (qr_result == mr_result) && qr_result;
    if(agree) passed++;
    
    string qr_str = qr_result ? "P" : "C";
    string mr_str = mr_result ? "P" : "C";
    
    double speedup = 0;
    string speedup_str = "-";
    if(tqr_ms > 0.001 && tmr_ms > 0.001) {
      speedup = tqr_ms / tmr_ms;
      if(speedup >= 1) {
        speedup_str = to_string((int)(speedup * 10) / 10.0) + "x (QR slower)";
      } else {
        speedup = 1.0 / speedup;
        speedup_str = to_string((int)(speedup * 10) / 10.0) + "x (MR slower)";
      }
    } else if(tqr_ms > 0.001 || tmr_ms > 0.001) {
      speedup_str = "N/A";
    }
    
    cout << left << setw(30) << name << setw(6) << qr_str << setw(6) << mr_str
         << setw(15) << fixed << setprecision(3) << tqr_ms
         << setw(15) << fixed << setprecision(3) << tmr_ms
         << setw(15) << speedup_str;
    if(!agree) cout << " ✘";
    cout << "\n";
  }
  
  // Test composites
  for(const auto& [n, name] : composites) {
    total++;
    bool qr_result = false, mr_result = false;
    double tqr_ms = 0, tmr_ms = 0;
    
    try {
      auto t0 = CpuTime();
      qr_result = isPrimeQR(n, 10);
      auto tqr = CpuTime() - t0;
      tqr_ms = tqr(MILI);
    } catch(const exception& e) {
      cout << left << setw(30) << name << "QR ERROR: " << e.what() << "\n";
      continue;
    }
    
    try {
      auto t1 = CpuTime();
      mr_result = isPrimeMR(n, 10);
      auto tmr = CpuTime() - t1;
      tmr_ms = tmr(MILI);
    } catch(const exception& e) {
      cout << left << setw(30) << name << "MR ERROR: " << e.what() << "\n";
      continue;
    }
    
    bool agree = (qr_result == mr_result) && !qr_result;
    if(agree) passed++;
    
    string qr_str = qr_result ? "P" : "C";
    string mr_str = mr_result ? "P" : "C";
    
    double speedup = 0;
    string speedup_str = "-";
    if(tqr_ms > 0.001 && tmr_ms > 0.001) {
      speedup = tqr_ms / tmr_ms;
      if(speedup >= 1) {
        speedup_str = to_string((int)(speedup * 10) / 10.0) + "x (QR slower)";
      } else {
        speedup = 1.0 / speedup;
        speedup_str = to_string((int)(speedup * 10) / 10.0) + "x (MR slower)";
      }
    } else if(tqr_ms > 0.001 || tmr_ms > 0.001) {
      speedup_str = "N/A";
    }
    
    cout << left << setw(30) << name << setw(6) << qr_str << setw(6) << mr_str
         << setw(15) << fixed << setprecision(3) << tqr_ms
         << setw(15) << fixed << setprecision(3) << tmr_ms
         << setw(15) << speedup_str;
    if(!agree) cout << " ✘";
    cout << "\n";
  }
  
  cout << string(80, '-') << "\n";
  cout << "Result: " << passed << "/" << total << " tests passed";
  if(passed == total) cout << " ✓";
  cout << "\n";
}

} // namespace uniq

int main() {
  uniq::runDemo();
  return 0;
}
