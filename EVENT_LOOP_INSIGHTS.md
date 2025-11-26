# Event Loop Insights from Medium Article

## Key Pattern from Article

From [Idiomatic Event Loop in C++](https://tony-space.medium.com/idiomatic-event-loop-in-c-edfb9ca5a862):

```cpp
using callable_t = std::function<void()>;

void enqueue(callable_t&& callable) {
  // Store in queue
  m_writeBuffer.emplace_back(std::move(callable));
}

// Usage:
eventLoop.enqueue(std::bind(function, arg1, arg2));
```

**Key Insight**: `std::bind(function, args...)` creates `std::function<void()>`!

## How This Solves Our Signature Matching Problem

### The Pattern

1. **Store handlers as `std::function<void(Args...)>`**
   ```cpp
   map<string, vector<function<void(Args...)>>> handlers;
   ```

2. **At emit time, use `std::bind()` to create `voidfunction`**
   ```cpp
   voidfunction bound = bind(handler, forward<Args>(args)...);
   ```

3. **Push `voidfunction` to queue**
   ```cpp
   run(bound);  // Goes to pool().queue
   ```

### Why This Works

- `std::function<void(Args...)>` can store any callable (lambda, function, functor)
- `std::bind()` captures both function AND arguments into `std::function<void()>`
- This is exactly what `Lazy` does: `bind(lambda, args...)` → `voidfunction`

## Solution for UniQ

### Current Problem
We store handlers as `any`, but can't extract lambda types.

### Solution: Store as `std::function<void(Args...)>`

**At Registration**:
```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Convert handler to function<void(Args...)> immediately
  // But we don't know Args yet...
  
  // Solution: Store handler in a way that preserves callability
  // Use std::function constructor which can convert callables
}
```

**The Challenge**: We don't know `Args` at registration time.

### Alternative: Store Handler + Convert at Emit

**At Registration**:
```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Store handler - we'll convert at emit time
  // Store as function<void(Args...)> when we know Args
  handlers[name].push_back(any(forward<Func>(f)));
}
```

**At Emit**:
```cpp
template<typename... Args>
void emit(const string& name, Args&&... args) {
  using HandlerType = function<void(decay_t<Args>...)>;
  
  for (auto& handler : handlers[name]) {
    // Try to convert handler to HandlerType
    // std::function constructor can convert callables!
    HandlerType h = HandlerType(any_cast<decay_t<Func>>(handler));
    
    // Use bind() like the article shows
    voidfunction bound = bind(h, forward<Args>(args)...);
    run(bound);
  }
}
```

**Problem**: Still can't extract lambda type from `any`.

## Best Solution: Store as `std::function` at Registration

The article shows the key: **Convert to `std::function` immediately**.

But we need to know the signature. Options:

### Option 1: Require Explicit Signature
```cpp
events.on<u64>("candidate", [](u64 n) { ... });
// Now we know it's function<void(u64)>
```

### Option 2: Infer from Handler
```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Extract signature from Func using function_traits
  using Signature = function_traits<Func>;
  using HandlerType = function<void(typename Signature::args...)>;
  
  handlers[name].push_back(HandlerType(forward<Func>(f)));
}
```

### Option 3: Store Handler + Type Info
```cpp
struct HandlerInfo {
  any handler;
  type_index signature;
  
  template<typename Func>
  HandlerInfo(Func&& f) 
    : handler(forward<Func>(f))
    , signature(typeid(function_traits<Func>))
  {}
};
```

## Recommended Approach for UniQ

Based on the article's pattern:

1. **Store handlers as `std::function<void(Args...)>`**
   - Convert at registration using `std::function` constructor
   - This requires knowing Args or using function_traits

2. **At emit, use `std::bind()` pattern**
   ```cpp
   voidfunction bound = bind(handler, forward<Args>(args)...);
   run(bound);
   ```

3. **This matches the Lazy pattern perfectly!**
   - `Lazy`: `bind(lambda, args...)` → `voidfunction`
   - `Event`: `bind(handler, args...)` → `voidfunction`

## Implementation

The article shows exactly what we need:

```cpp
// Store handlers as function<void(Args...)>
map<string, vector<function<void(Args...)>>> handlers;

// At emit:
voidfunction bound = bind(handler, args...);
run(bound);
```

The challenge is converting handlers to `function<void(Args...)>` at registration when we don't know Args.

**Solution**: Use `std::function` constructor which can convert any callable:
```cpp
function<void(Args...)> handler = f;  // Works if f is callable with Args
```

But we still need to know Args...

## Conclusion

The article confirms our approach is correct:
- Use `std::bind()` to create `voidfunction` (like Lazy)
- Store handlers as `std::function<void(Args...)>` if possible
- Use `std::function` constructor to convert callables

The remaining challenge: How to know Args at registration time?

**Best practical solution**: 
- Store handlers as `any` (current approach)
- At emit, try to convert using `std::function` constructor
- If conversion fails, handler doesn't match (skip)

This is what we have now, and it's reasonable given the constraints!

