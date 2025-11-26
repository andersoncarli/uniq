# Event System Signature Matching: Best Solutions

## The Core Challenge

**Problem**: How to match signatures between:
- `emit("event", arg1, arg2, ...)` 
- `on("event", [](Type1, Type2, ...) { ... })`

## Current Best Solutions

### 1. **Type Erasure with std::any** (Current Approach)
```cpp
// Store handlers as any
map<string, vector<any>> handlers;

// At emit time, try to cast
HandlerType h = any_cast<HandlerType>(handler);
```

**Pros**:
- ✅ Flexible: Can store any callable type
- ✅ Runtime registration
- ✅ Simple storage

**Cons**:
- ❌ Runtime type checking (exceptions)
- ❌ No compile-time safety
- ❌ Can't extract lambda types from `any`

**Status**: What we're currently using, but has limitations.

---

### 2. **Type Erasure with std::function** (Better)
```cpp
// Store handlers as function<void(Args...)>
map<string, vector<function<void(Args...)>>> handlers;

// At registration, convert handler to function
template<typename Func>
void on(const string& name, Func&& f) {
  using HandlerType = function<void(Args...)>;
  handlers[name].push_back(HandlerType(forward<Func>(f)));
}
```

**Pros**:
- ✅ Type-safe storage
- ✅ Can convert lambdas to `function<void(Args...)>`
- ✅ No runtime type checking needed

**Cons**:
- ❌ Need to know Args at registration time
- ❌ Can't support multiple signatures for same event

**Status**: Better, but requires knowing Args upfront.

---

### 3. **Template-Based Type-Safe Events** (Best for Compile-Time)
```cpp
template<typename... Args>
class TypedEvent {
  vector<function<void(Args...)>> handlers;
public:
  void on(function<void(Args...)> handler) {
    handlers.push_back(handler);
  }
  void emit(Args... args) {
    for (auto& h : handlers) h(args...);
  }
};

// Usage
TypedEvent<u64> candidateEvent;
candidateEvent.on([](u64 n) { ... });
candidateEvent.emit(42);  // Compile-time checked!
```

**Pros**:
- ✅ Full compile-time type safety
- ✅ No runtime overhead
- ✅ Clear API

**Cons**:
- ❌ Can't use string names (need typed events)
- ❌ Less flexible for dynamic systems
- ❌ Each event type needs separate instance

**Status**: Best for type safety, but loses string-based flexibility.

---

### 4. **Variant-Based System** (Flexible)
```cpp
using EventData = variant<u64, string, pair<u64, u64>>;

class EventEmitter {
  map<string, vector<function<void(EventData)>>> handlers;
  
  template<typename T>
  void emit(const string& name, T&& data) {
    EventData v = forward<T>(data);
    for (auto& h : handlers[name]) h(v);
  }
};

// Handlers use std::visit
events.on("candidate", [](EventData data) {
  u64 n = get<u64>(data);
  // ...
});
```

**Pros**:
- ✅ Type-safe payload
- ✅ Can support multiple types per event
- ✅ Compile-time checked variant access

**Cons**:
- ❌ Limited to predefined types
- ❌ Handlers must handle variant
- ❌ More complex API

**Status**: Good for systems with known data types.

---

### 5. **Type-Erased Callable Wrapper** (Hybrid)
```cpp
struct CallableWrapper {
  any handler;
  
  template<typename... Args>
  void invoke(Args&&... args) {
    // Try to convert handler to function<void(Args...)>
    function<void(decay_t<Args>...)> f = 
      any_cast<function<void(decay_t<Args>...)>>(handler);
    f(forward<Args>(args)...);
  }
};
```

**Pros**:
- ✅ Flexible storage
- ✅ Type-safe invocation
- ✅ Can wrap any callable

**Cons**:
- ❌ Still needs runtime type checking
- ❌ Can't extract lambda types

**Status**: Similar to current approach, but cleaner.

---

### 6. **Registration-Time Conversion** (Recommended)
```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Convert handler to a type-erased callable that stores the original
  // Use a wrapper that can be invoked with any Args
  auto wrapper = [f = forward<Func>(f)](auto&&... args) mutable {
    return invoke(f, forward<decltype(args)>(args)...);
  };
  handlers[name].push_back(any(wrapper));
}
```

**Pros**:
- ✅ Stores original handler
- ✅ Can invoke with any Args
- ✅ Flexible

**Cons**:
- ❌ Generic lambdas can't be stored in `any`
- ❌ Still needs runtime checking

**Status**: Theoretical, but generic lambdas can't be stored.

---

## Recommended Solution for UniQ

### **Hybrid Approach: Type-Safe Storage with Runtime Validation**

```cpp
class EventEmitter {
  // Store handlers as function<void(Args...)> per event signature
  // Use a type map to track signatures
  map<string, any> eventSignatures;  // event -> signature type info
  map<string, vector<any>> handlers;
  
  template<typename Func>
  void on(const string& name, Func&& f) {
    // Extract signature from Func
    using Signature = function_traits<Func>;
    
    // Store signature info
    eventSignatures[name] = typeid(Signature);
    
    // Convert handler to function<void(Args...)>
    using HandlerType = function<void(typename Signature::args...)>;
    handlers[name].push_back(HandlerType(forward<Func>(f)));
  }
  
  template<typename... Args>
  void emit(const string& name, Args&&... args) {
    // Check signature matches
    using ExpectedSignature = function<void(decay_t<Args>...)>;
    
    // Get handlers and invoke
    using HandlerType = function<void(decay_t<Args>...)>;
    for (auto& handler : handlers[name]) {
      HandlerType h = any_cast<HandlerType>(handler);
      voidfunction bound = bind(h, forward<Args>(args)...);
      run(bound);
    }
  }
};
```

**Key Insight**: Convert handlers to `function<void(Args...)>` at registration time, but we need to know Args...

---

## Best Practical Solution

### **Store Handlers with Signature Validation**

1. **At Registration**: Convert handler to `function<void(Args...)>` if possible
   - Problem: We don't know Args yet

2. **At Emit Time**: Try to convert stored handler to `function<void(Args...)>`
   - Use `std::function` constructor which can convert callables
   - If conversion fails, handler doesn't match (skip)

3. **Type Safety**: Use compile-time checks where possible
   - Template constraints
   - SFINAE to check callability

### **Implementation Strategy**

```cpp
template<typename Func>
void on(const string& name, Func&& f) {
  // Store handler - we'll validate at emit time
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
      voidfunction bound = bind(h, forward<Args>(args)...);
      run(bound);
    } catch (const bad_any_cast&) {
      // Handler doesn't match - skip
      // Could log warning in debug mode
    }
  }
}
```

**This is what we have now!** The challenge is that lambdas stored in `any` can't be extracted.

---

## Ultimate Solution: Type Registry

Store handlers with their type information:

```cpp
struct HandlerInfo {
  any handler;
  type_index handlerType;
  
  template<typename Func>
  HandlerInfo(Func&& f) 
    : handler(forward<Func>(f))
    , handlerType(typeid(function_traits<Func>))
  {}
  
  template<typename... Args>
  bool canInvoke() const {
    return handlerType == typeid(function<void(Args...)>);
  }
};
```

But this still requires knowing the handler type at registration...

---

## Conclusion

**Current Best Practice**:
1. Store handlers as `any` (flexible)
2. At emit time, try to convert to `function<void(Args...)>`
3. Use `std::function` constructor which can convert callables
4. Skip handlers that don't match (with optional logging)

**Future Improvement**:
- Convert handlers to `function<void(Args...)>` at registration
- But this requires knowing Args, which we don't have
- **Solution**: Use a type registry or require explicit signature declaration

**For UniQ**: Current approach is reasonable. The key is ensuring handlers match emit signatures through:
- Documentation
- Testing
- Optional runtime validation/warnings

