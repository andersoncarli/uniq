# Parameter Passing in Event System

## The Question

**Q**: If `bind_lazy()` creates a `voidfunction` (no parameters), how can we pass event values to listeners?

**A**: **Parameters ARE passed!** They're captured at **bind time**, not execution time.

## How It Works

### The Magic of std::bind()

`std::bind(function, args...)` creates a callable object that:
1. Stores the function pointer
2. **Stores all the arguments** (captured at bind time)
3. When called, reconstructs the original function call

### Example

```cpp
// Handler expects a parameter
auto handler = [](u64 n) {
  cout << "Received: " << n << endl;
};

// Bind with argument - 42 is CAPTURED here
voidfunction bound = bind_lazy(handler, 42);
// 'bound' now contains: handler + 42

// Execute later - no args needed!
bound();  // This internally calls handler(42)
// Output: "Received: 42"
```

## Event System Flow

```cpp
// 1. Register handler (expects u64 parameter)
events.on("candidate", [](u64 n) {
  cout << "Testing: " << n << endl;
});

// 2. Emit event with value
events.emit("candidate", 42);

// Inside emit():
//   - Get handler from storage
//   - bind_lazy(handler, 42) → creates voidfunction with 42 captured
//   - Push voidfunction to queue
//   - Worker executes voidfunction() → handler(42) runs!
```

## Visual Timeline

```
Time T1: emit("event", 42) called
    ↓
    bind_lazy(handler, 42)  ← Arguments CAPTURED here
    ↓
    voidfunction created (has 42 stored inside)
    ↓
    Queue.push(voidfunction)
    
Time T2: Worker pops voidfunction from queue
    ↓
    voidfunction() called (no args!)
    ↓
    Internally: handler(42) executed ← 42 was captured at T1
```

## Why This Works

`std::bind()` uses **closure semantics**:
- Arguments are captured by value (or reference with `ref()`)
- The bound object stores both function and arguments
- When called, it reconstructs the original call

## Multiple Parameters

```cpp
events.on("data", [](int a, string b, double c) {
  // Process a, b, c
});

events.emit("data", 10, "hello", 3.14);
// bind_lazy() captures all three: (10, "hello", 3.14)
// When executed: handler(10, "hello", 3.14) runs
```

## Comparison with ThreadPool::run()

This is exactly how `run()` works:

```cpp
run(function, arg1, arg2);
// 1. bind_lazy(function, arg1, arg2) → voidfunction
// 2. Push to queue
// 3. Worker executes → function(arg1, arg2) runs
```

Events use the **same pattern**!

## Conclusion

**Yes, parameters ARE passed to event listeners!**

- Parameters are captured when `emit()` is called
- They're stored inside the `voidfunction`
- When worker executes the `voidfunction`, handler runs with captured values

This is the power of `std::bind()` - it creates closures that capture both function and arguments!

