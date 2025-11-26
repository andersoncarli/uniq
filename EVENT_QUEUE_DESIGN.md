# Event-Driven System with Queue - Design Proposal

## Architecture: Multithreaded Event Loop

### Core Concept
Instead of calling event handlers synchronously, push them to a Queue and let Workers process them.

### New EventEmitter Design

```cpp
class EventEmitter {
  map<string, Event> events;  // Event name -> Event
  Queue<voidfunction> eventQueue;  // Queue for distributing events
  
public:
  void listen(string name, voidfunction f);
  void emit(string name);  // Push handlers to queue instead of calling directly
  void start(int numWorkers = 0);  // Start worker threads
  void stop();
};
```

### Event Flow

1. **emit("test_number", n)** called
2. Event handlers for "test_number" are pushed to `eventQueue`
3. Workers pop from queue and execute handlers
4. Handlers can emit new events (which go back to queue)
5. Creates a multithreaded event loop!

### Twin Prime Sieve Flow

```
1. Emit "test_number" events for range [start, end]
2. PrimeChecker Workers listen to "test_number"
3. When prime found, emit "prime_found" event
4. TwinChecker Workers listen to "prime_found"
5. Check if p+2 or p-2 is also prime
6. If twin found, emit "twin_prime_found"
7. Collector listens to "twin_prime_found" and records results
```

### Event Types

- **"test_number"**: `{n}` - number to test for primality
- **"prime_found"**: `{p}` - prime number found
- **"twin_prime_found"**: `{p, p+2}` - twin prime pair found
- **"done"**: all numbers tested

### Actor Types

1. **NumberEmitter**: Emits "test_number" events
2. **PrimeChecker** (multiple): Listen to "test_number", test primality
3. **TwinChecker** (multiple): Listen to "prime_found", check twins
4. **Collector**: Listens to "twin_prime_found", collects results

## Implementation Questions

1. **Event Data Passing**:
   - How to pass data with events?
   - Option A: Use closures in voidfunction (capture n, p, etc.)
   - Option B: Create EventData struct
   - Option C: Use Queue<EventData> instead of Queue<voidfunction>

2. **Event Queue vs Handler Queue**:
   - Queue<voidfunction> - queue the handlers themselves
   - Queue<EventData> - queue event data, handlers process it
   - Which is better?

3. **Worker Management**:
   - Should EventEmitter have its own ThreadPool?
   - Or use global pool()?
   - Or each Event has its own Queue?

## Recommendation

**Use closures with Queue<voidfunction>**:
- Simple and flexible
- Event handlers capture data via closures
- `emit("test_number")` pushes `[&]{ checkPrime(n); }` to queue
- Workers execute these closures

This creates a true multithreaded event loop!




