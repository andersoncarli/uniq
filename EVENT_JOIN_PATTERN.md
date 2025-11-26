# Event System Using Join Pattern

## Key Insight from join.h

```cpp
template<class F1, class F2>
struct Joint : public F1, public F2 {
    using F1::operator(); 
    using F2::operator(); 
};

auto join(F1&& f1, F2&& f2) {
    return Joint { std::forward<F1>(f1), std::forward<F2>(f2) }; 
}
```

**Pattern**: Combine multiple callables into a single callable object using inheritance.

## Potential Application to Event System

### Current Approach
```cpp
// Store handlers in vector
vector<any> handlers;
for (auto& handler : handlers) {
  voidfunction bound = bind(handler, args...);
  run(bound);
}
```

### Using Join Pattern
```cpp
// Combine all handlers into one callable
auto combined = join(handler1, handler2, handler3, ...);
voidfunction bound = bind(combined, args...);
run(bound);
```

## Benefits

1. **Single bind() call**: Instead of binding each handler separately, bind once
2. **Simpler iteration**: One callable instead of vector iteration
3. **Type safety**: Handlers are combined at compile time

## Challenges

1. **Dynamic handlers**: `join()` works at compile time, but handlers are registered at runtime
2. **Type erasure**: We store handlers as `any`, but `join()` needs concrete types
3. **Signature matching**: All handlers must have compatible signatures

## Alternative: Runtime Join

We could create a runtime version that combines handlers:

```cpp
struct HandlerSet {
  vector<any> handlers;
  
  template<typename... Args>
  void operator()(Args&&... args) {
    for (auto& handler : handlers) {
      // Invoke each handler
    }
  }
};
```

But this is essentially what we're already doing!

## Conclusion

The `join()` pattern is elegant for compile-time combination of callables, but for runtime event handlers stored as `any`, our current approach (iterating and binding each) is more appropriate.

However, `join()` could be useful if we:
1. Store handlers with concrete types (not `any`)
2. Want to combine handlers at registration time
3. Need to support overloaded handlers (different signatures)

