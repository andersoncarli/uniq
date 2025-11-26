# Applying Event Loop Article to UniQ

## Key Pattern from Article

From [Idiomatic Event Loop in C++](https://tony-space.medium.com/idiomatic-event-loop-in-c-edfb9ca5a862):

```cpp
using callable_t = std::function<void()>;

void enqueue(callable_t&& callable) {
  m_writeBuffer.emplace_back(std::move(callable));
}

// Usage:
eventLoop.enqueue(std::bind(function, arg1, arg2));
```

**This is exactly what Lazy does!** And what we need for events.

## How Article Solves Signature Matching

The article shows:
1. **Store handlers as `std::function<void(Args...)>`**
2. **Use `std::bind(handler, args...)` to create `voidfunction`**
3. **Push `voidfunction` to queue**

This is the perfect pattern!

## Current UniQ Implementation

We're already using this pattern:
- Store handlers (as `any` for now)
- At emit, create `voidfunction` using bind pattern
- Push to `pool().queue`

## The Remaining Challenge

The article assumes you know the handler signature. For dynamic events with string names, we need:

1. **Option A**: Convert handlers to `std::function<void(Args...)>` at registration
   - Problem: We don't know Args yet

2. **Option B**: Store handlers as `any`, convert at emit time
   - Problem: Can't extract lambda types from `any`

3. **Option C**: Use `std::function` constructor at emit time
   - Works if handler is stored as callable that can be converted

## Best Solution Based on Article

The article shows handlers should be `std::function<void(Args...)>`. For dynamic events:

**Store handlers wrapped in a way that preserves callability:**

```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Convert to function<void(Args...)> when we know Args
  // For now, store as any
  handlers[name].push_back(any(forward<Func>(f)));
}

template<typename... Args>
void emit(const string& name, Args&&... args) {
  using HandlerType = function<void(decay_t<Args>...)>;
  
  for (auto& handler : handlers[name]) {
    // Try to convert handler to HandlerType
    // std::function constructor can convert callables
    try {
      HandlerType h = any_cast<HandlerType>(handler);
      // Use bind() pattern from article
      voidfunction bound = bind(h, forward<Args>(args)...);
      run(bound);
    } catch (const bad_any_cast&) {
      // Handler doesn't match - skip
    }
  }
}
```

## Key Insight from Article

The article confirms:
- ✅ `std::function` uses type erasure (can store any callable)
- ✅ `std::bind()` creates `voidfunction` from function + args
- ✅ This is the idiomatic C++ pattern

Our implementation follows this pattern! The only difference is we need to handle dynamic signatures.

## Recommendation

Keep current approach, but:
1. Document that handlers should match emit signatures
2. Add optional debug logging for mismatched handlers
3. Consider requiring explicit signature: `on<Args...>(name, handler)`

The article validates our approach - we're on the right track!

