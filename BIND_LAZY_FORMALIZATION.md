# Formalizing bind_lazy() Pattern

## Core Pattern

**Foundation**: `bind(function, args...)` → `voidfunction`

This is the fundamental abstraction used throughout UniQ:
- **Lazy**: Stores `voidfunction` for later execution
- **ThreadPool**: Pushes `voidfunction` to queue, workers execute
- **Events**: Creates `voidfunction` from handlers + args, pushes to queue

## Implementation

### Core Function (in `call.h`)

```cpp
template <typename Func, typename... Args>
inline voidfunction bind_lazy(Func&& f, Args&&... args) {
  ASSERT_INVOCABLE(Func, Args);
  return bind(forward<Func>(f), forward<Args>(args)...);
}
```

### Usage Pattern

```cpp
// 1. Create voidfunction from function + args
voidfunction task = bind_lazy(function, arg1, arg2);

// 2. Execute later
task();

// 3. Or push to queue
queue.push(task);
```

## Architecture

```
bind_lazy() [Foundation]
    ↓
    ├─→ Lazy: Stores voidfunction, executes on demand
    ├─→ ThreadPool::run(): Creates voidfunction, pushes to queue
    └─→ Event::emit(): Creates voidfunction from handler+args, pushes to queue
```

## Benefits

1. **Consistency**: Same pattern everywhere
2. **Type Safety**: `ASSERT_INVOCABLE` ensures function can be called with args
3. **Testability**: Core pattern is tested independently
4. **Clarity**: Clear abstraction - "bind function with args to create voidfunction"

## Tests

### TEST(Bind)
- Lambda with no args
- Lambda with args
- Function with args
- Member function
- Multiple args
- String args

### TEST(BindEdges)
- Empty function
- References
- Const values
- Move semantics
- Chaining

## Integration

### Lazy
```cpp
Lazy(Func&& lambda, Args&&... args) {
  beat = bind_lazy(forward<Func>(lambda), forward<Args>(args)...);
}
```

### ThreadPool::run()
```cpp
template <typename Func, typename... Args>
inline int run(Func &&f, Args &&...args) {
  voidfunction vf = bind_lazy(forward<Func>(f), forward<Args>(args)...);
  return push(vf);
}
```

### Event::emit()
```cpp
template<typename... Args>
void emit(const string& name, Args&&... args) {
  HandlerType h = any_cast<HandlerType>(handler);
  voidfunction bound = bind_lazy(forward<HandlerType>(h), forward<Args>(args)...);
  run(bound);
}
```

## Worker Pattern

```cpp
void worker(int id) {
  voidfunction f;
  while (this->running() && pop(f)) {
    f();  // Execute voidfunction from queue
  }
}
```

**Flow**:
1. `run()` or `emit()` creates `voidfunction` using `bind_lazy()`
2. Pushes to queue
3. Worker pops and executes

## Next Steps

1. ✅ Formalize `bind_lazy()` in `call.h`
2. ✅ Add comprehensive tests
3. ✅ Update Lazy to use `bind_lazy()`
4. ✅ Update ThreadPool to use `bind_lazy()`
5. ✅ Update Events to use `bind_lazy()`
6. ✅ Verify all tests pass

## Result

**Single, tested, consistent pattern** used throughout UniQ:
- Foundation: `bind_lazy()`
- Built on: Lazy, ThreadPool, Events
- All use the same tested abstraction

