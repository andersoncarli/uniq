# Using run() Pattern for Events

## Key Insight

Looking at `ThreadPool::run()`:

```cpp
template <typename Func, typename... Args>
inline int run(Func &&f, Args &&...args) {
  voidfunction vf = bind(forward<Func>(f), forward<Args>(args)...);
  return push(vf);
}
```

**This accepts ANY function with ANY args and uses `bind()` to create `voidfunction`!**

## Why This Works

1. **Template parameters**: `Func` and `Args...` are known at compile time
2. **bind() works**: `bind(function, args...)` creates `voidfunction`
3. **Type erasure**: `voidfunction` can store the bound callable

## Applying to Events

### Current Challenge

For events, we have:
- **Registration**: `on("event", handler)` - handler type known
- **Emission**: `emit("event", args...)` - args known, but handler is in `any`

### Solution: Same Pattern as run()

**At Registration**:
```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Store handler - same as run() accepts any function
  handlers[name].push_back(any(forward<Func>(f)));
}
```

**At Emission**:
```cpp
template<typename... Args>
void emit(const string& name, Args&&... args) {
  for (auto& handler : handlers[name]) {
    // Use bind() like run() does!
    // But we need to extract handler from any first
    voidfunction bound = bind(handler, forward<Args>(args)...);
    run(bound);
  }
}
```

### The Problem

`bind()` needs the handler type, but it's stored in `any`. We can't do:
```cpp
bind(handler, args...)  // handler is any, bind() doesn't know the type
```

### Solution: Extract Handler Type

We need to extract the handler from `any` and then use `bind()`. Options:

1. **Store as function<void(Args...)>** (but we don't know Args at registration)
2. **Use type-erased wrapper** that can be bound
3. **Convert at emit time** using `std::function` constructor

### Best Approach: Convert at Emit Time

```cpp
template<typename... Args>
void emit(const string& name, Args&&... args) {
  using HandlerType = function<void(decay_t<Args>...)>;
  
  for (auto& handler : handlers[name]) {
    // Try to extract handler
    try {
      HandlerType h = any_cast<HandlerType>(handler);
      // Now use bind() like run() does!
      voidfunction bound = bind(h, forward<Args>(args)...);
      run(bound);
    } catch (const bad_any_cast&) {
      // Handler doesn't match - skip
    }
  }
}
```

But this still requires handlers to be stored as `function<void(Args...)>`.

### Alternative: Store Handler Wrapper

Store handlers in a wrapper that can be bound:

```cpp
struct BindableHandler {
  any handler;
  
  template<typename... Args>
  voidfunction bind(Args&&... args) {
    // Try to extract and bind
    using HandlerType = function<void(decay_t<Args>...)>;
    HandlerType h = any_cast<HandlerType>(handler);
    return std::bind(h, forward<Args>(args)...);
  }
};
```

But we still need to know the handler type...

## Conclusion

The `run()` pattern is perfect, but the challenge is:
- `run()` has `Func` and `Args` at the same time (template parameters)
- Events have `Func` at registration and `Args` at emission (separate times)

**Solution**: Store handlers as `function<void(Args...)>` when we know Args, OR use a type-erased wrapper that can be converted at emit time.

The current approach (try to extract and bind) is reasonable given the constraints!

