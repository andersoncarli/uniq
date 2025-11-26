# How Parameters Are Passed to Bound Functions

## The Key Insight

**`bind_lazy(function, args...)` captures the arguments at bind time!**

When you create a `voidfunction` using `bind_lazy()`, the arguments are "baked in" to the function object. When you call the `voidfunction` later, it executes with those captured arguments.

## How It Works

### Step-by-Step Flow

```cpp
// 1. Handler expects a parameter
auto handler = [](u64 n) { 
  cout << "Got: " << n << endl; 
};

// 2. Bind handler with argument - arguments are CAPTURED here
voidfunction bound = bind_lazy(handler, 42);
// Now 'bound' is a voidfunction that has 42 already captured

// 3. Later, execute the voidfunction (no args needed!)
bound();  // This calls handler(42) - the 42 was captured at bind time
```

## Event System Example

```cpp
// Registration: Handler expects u64 parameter
events.on("candidate", [](u64 n) {
  cout << "Testing: " << n << endl;
});

// Emission: Pass the value
events.emit("candidate", 42);  // Pass 42 to handler

// Inside emit():
// 1. Get handler from storage
// 2. bind_lazy(handler, 42) → creates voidfunction with 42 captured
// 3. Push voidfunction to queue
// 4. Worker executes voidfunction() → handler(42) runs!
```

## Visual Flow

```
emit("event", 42)
    ↓
bind_lazy(handler, 42)  ← Arguments captured HERE
    ↓
voidfunction (has 42 baked in)
    ↓
Queue.push(voidfunction)
    ↓
Worker.pop(voidfunction)
    ↓
voidfunction()  ← No args needed! 42 already captured
    ↓
handler(42) executes
```

## Why This Works

`std::bind()` captures arguments by value (or reference if you use `ref()`). The resulting `voidfunction` stores:
- The function pointer
- All the arguments (captured at bind time)

When you call the `voidfunction`, it reconstructs the original call with the captured arguments.

## Multiple Parameters

```cpp
// Handler with multiple parameters
auto handler = [](int a, string b, double c) {
  cout << a << " " << b << " " << c << endl;
};

// Bind with all arguments
voidfunction bound = bind_lazy(handler, 10, "hello", 3.14);
// All three arguments are captured

// Execute later
bound();  // Calls handler(10, "hello", 3.14)
```

## Event System Implementation

In `Event::emit()`:

```cpp
template<typename... Args>
void emit(const string& name, Args&&... args) {
  HandlerType h = any_cast<HandlerType>(handler);
  
  // Bind handler with emit arguments - they're captured here!
  voidfunction bound = bind_lazy(h, forward<Args>(args)...);
  // Now 'bound' has all the args captured
  
  run(bound);  // Push to queue
  // When worker executes bound(), handler runs with captured args
}
```

## Conclusion

**Yes, parameters ARE passed to listeners!** They're just captured at bind time (when `emit()` is called), not at execution time (when worker runs).

This is exactly how `ThreadPool::run()` works:
```cpp
run(function, arg1, arg2);  // Args captured at bind time
// Later, worker executes voidfunction() → function(arg1, arg2) runs
```

The event system uses the same pattern!

