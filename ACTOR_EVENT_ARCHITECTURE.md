# Actor-Event Architecture: Single Queue System

## Core Vision

**UniQ = Single Queue** - All Actors interact through ONE event queue

## Architecture

```
Main Loop
  ↓
emit('candidate', n) → [Single Queue] → Workers/Actors
  ↓                                    ↓
emit('prime', p)      ← [Single Queue] ← Actors respond
  ↓                                    ↓
emit('twin', p1,p2)   ← [Single Queue] ← Actors respond
```

## Actor Lifecycle

- Actors are like game characters - cheap threads
- They receive "beats" from event queue
- They respond to events via `on('event', lambda)`
- They emit events via `emit('event', data)`
- All interaction through single queue - no direct push/pop

## Event System Design

### Functions:
- `on('event', lambda)` - Register handler (Actor listens)
- `emit('event', data...)` - Emit event with data (goes to queue)
- Handlers receive data as parameters

### Flow:
1. Main loop: `while(i<max) emit('candidate', i++);`
2. PrimeChecker Actor: `on('candidate', [](u64 n){ ... })`
3. When prime found: `emit('prime', p);`
4. TwinChecker Actor: `on('prime', [](u64 p){ ... })`
5. When twin found: `emit('twin', p1, p2);`
6. Output Actor: `on('twin', [](u64 p1, u64 p2){ ... })`

## Benefits

1. **Natural Preemption**: If queue full, emit() waits naturally
2. **No Direct Queue Access**: Actors never push/pop directly
3. **Event-Driven**: Everything is reactive
4. **Single Pipeline**: Easy to monitor, throttle, debug
5. **Actor Isolation**: Actors only know about events, not each other

## Implementation

EventEmitter needs:
- `template<typename... Args> void emit(string name, Args&&... args)`
- `template<typename Func> void on(string name, Func&& f)`
- Store typed handlers (not just voidfunction)
- When emit() called, create closure with data, push to pool().queue




