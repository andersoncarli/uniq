# Event System Using Lazy Pattern

## Key Insight from Lazy.h

```cpp
Lazy(Func&& lambda, Args&&... args) {
  beat = bind(forward<Func>(lambda), forward<Args>(args)...);
}
```

**Pattern**: `bind(function, args...)` creates a `voidfunction` that can be called later.

## Event System Design

### Registration
```cpp
events.on("candidate", [](u64 n) { ... });
```
- Store handler as `any` (type-erased)
- Handler signature: `void(u64)`

### Emission
```cpp
events.emit("candidate", 42);
```
- Get handler from storage
- Use `bind(handler, args...)` to create `voidfunction` (like Lazy)
- Push `voidfunction` to `pool().queue`

## Challenge

**Problem**: Handler is stored as `any`, but `bind()` needs the exact type.

**Solution Options**:

1. **Convert at registration**: Convert handler to `function<void(Args...)>` when storing
   - ❌ We don't know Args at registration time

2. **Convert at emit time**: Extract handler from `any` and convert to `function<void(Args...)>`
   - ❌ Can't extract lambda type from `any` without knowing it

3. **Type-erased wrapper**: Store handler in a wrapper that can be invoked with any Args
   - ✅ Use `std::function` constructor which can convert callables
   - ✅ At emit time, wrap handler in `function<void(Args...)>` and bind

## Implementation

```cpp
// Store handler
template<typename Func>
void on(const string& name, Func&& f) {
  typedHandlers[name].push_back(any(forward<Func>(f)));
}

// Emit with args
template<typename... Args>
void emit(const string& name, Args&&... args) {
  for (auto& handler : handlers) {
    // Create voidfunction using bind() pattern (like Lazy)
    voidfunction bound = [handler, args...]() mutable {
      using HandlerType = function<void(decay_t<Args>...)>;
      
      // Try to extract as function<void(Args...)>
      try {
        HandlerType h = any_cast<HandlerType>(handler);
        h(forward<Args>(args)...);
      } catch (const bad_any_cast&) {
        // Handler is lambda - need to convert
        // std::function constructor can convert callables
        // But we need the handler type...
      }
    };
    
    run(bound);  // Push to single queue
  }
}
```

## Best Solution

**Store handlers wrapped in a type-erased callable**:
- When storing: Wrap handler in a way that preserves callability
- At emit: Use `bind()` pattern to create `voidfunction`

**Or simpler**: Require handlers to be registered as `function<void(Args...)>`:
- Convert at registration: `function<void(Args...)> wrapped(handler)`
- Store as `function<void(Args...)>`
- At emit: `bind(wrapped, args...)` → `voidfunction`

But we still don't know Args at registration...

## Current Approach

Store handlers as `any`, and at emit time:
1. Try to extract as `function<void(Args...)>`
2. If that fails, the handler type doesn't match (skip)

This works if handlers are registered with matching signatures.

