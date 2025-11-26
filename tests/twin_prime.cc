// Twin Prime Sieve - Event-driven with Single Queue Architecture
// Demonstrates: Actors respond to events, all through pool().queue
// compile using ./build twin_prime
#include "uniq.h"
using namespace uniq;

// Prime checking (from prime.cc)
u64 spiral(u64 n, u64 min, u64 max) {
  for (u64 i = min; i <= max; i += 30) {
    u64 j = i;
    if (!(n % j)) return j;
    if (!(n % (j += 4))) return j;
    if (!(n % (j += 2))) return j;
    if (!(n % (j += 4))) return j;
    if (!(n % (j += 2))) return j;
    if (!(n % (j += 4))) return j;
    if (!(n % (j += 6))) return j;
    if (!(n % (j += 2))) return j;
  }
  return n;
}

u64 spiralDivisor(u64 n, u64 limit = 0) {
  if (!(n % 2)) return 2;
  if (!(n % 3)) return 3;
  if (!(n % 5)) return 5;
  if (limit == 0) limit = n;
  limit = std::min(limit, (u64)sqrt(n));
  return spiral(n, 7, limit);
}

bool isPrime(u64 n) {
  return spiralDivisor(n) == n;
}

// Global event emitter - all events flow through single queue
EventEmitter events;

// Shared state
atomic<int> twinCount(0);
vector<pair<u64, u64>> twins;
mutex twinsMutex;
atomic<u64> numbersTested(0);
atomic<u64> primesFound(0);

int main(int argc, char* argv[]) {
  pool().showstats = true;
  pool().start();
  
  u64 start = 3;
  u64 end = argc > 1 ? stoull(argv[1]) : 100000;
  
  out("Twin Prime Sieve: [", start, ", ", end, "]\n");
  out("Architecture: Single Queue (pool().queue)\n");
  out("Actors respond to events: 'candidate', 'prime', 'twin'\n");
  out("All events flow through ONE queue\n\n");
  
  Time t;
  
  // PrimeChecker Actor: listens to 'candidate' events
  // When prime found, emits 'prime' event
  events.on("candidate", [](u64 n) {
    numbersTested++;
    if (isPrime(n)) {
      primesFound++;
      events.emit("prime", n);  // Goes to single queue
    }
  });
  
  // TwinChecker Actor: listens to 'prime' events
  // Checks if p+2 or p-2 is also prime, emits 'twin' if found
  events.on("prime", [end, start](u64 p) {
    // Check p+2
    if (p + 2 <= end) {
      run([p, end] {
        if (isPrime(p + 2)) {
          lock_guard<mutex> lock(twinsMutex);
          twins.push_back({p, p + 2});
          twinCount++;
        }
      });
    }
    
    // Check p-2
    if (p - 2 >= start) {
      run([p, start] {
        if (isPrime(p - 2)) {
          lock_guard<mutex> lock(twinsMutex);
          // Avoid duplicates
          bool exists = false;
          for (auto& twin : twins) {
            if (twin.first == p - 2 && twin.second == p) {
              exists = true;
              break;
            }
          }
          if (!exists) {
            twins.push_back({p - 2, p});
            twinCount++;
          }
        }
      });
    }
  });
  
  // Main loop: emit 'candidate' events
  // No waits - if queue full, emit() preempts naturally
  for (u64 i = start; i <= end; i += 2) {  // Only odd numbers
    events.emit("candidate", i);  // All go to single queue
  }
  
  // Wait for single queue to be empty (all events processed)
  while (pool().size() > 0) {
    sleep(10);
  }
  
  pool().stop();
  pool().join();
  
  Time elapsed = t();
  
  out("\nResults:\n");
  out("  Numbers tested: ", numbersTested, "\n");
  out("  Primes found: ", primesFound, "\n");
  out("  Twin pairs: ", twinCount, "\n\n");
  
  if (twinCount > 0 && twinCount <= 20) {
    out("Twin primes:\n");
    sort(twins.begin(), twins.end());
    for (auto& twin : twins) {
      out("  (", twin.first, ", ", twin.second, ")\n");
    }
  }
  
  log("\nTime: ", elapsed);
  log("All events processed through single queue (pool().queue)");
  
  quick_exit(0);
}
