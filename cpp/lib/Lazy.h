#include "uniq.h"
using namespace uniq;

namespace uniq {

// Lazy(say, "Hello", 123)()
//====================================================================== Lazy()
template<typename Func, typename... Args> 
struct Lazy {
  // voidfunction beat = []<typename... Args>(Args&&... args) { log(args...); };
  voidfunction beat = [&]{};

  Lazy(Func&& lambda, Args&&... args){
   	ASSERT_INVOCABLE(Func, Args);
    beat = bind(forward<Func>(lambda), forward<Args>(args)...);
  }

  // functor
  inline void operator()(){ beat(); }
};
};

namespace unimock {

// say("Hello", 123)
//======================================================================== say()
template <typename... Args> string say(Args&&... args) {
    ostringstream ss;
    (ss << ... << args);
    cout << ss.str() << flush;
    return ss.str();
  }

// template <typename Func, typename... Args>
// using VariadicFunc = Func (*)(Args...);

// run(say, "Hello", 123)
//======================================================================== run()
template <typename Func, typename... Args> auto run(Func&& lambda, Args&&... args) {
  return lambda(args...); // this works
  // log(YEL, a...);
  // // save and apply
  // auto tuple = forward_as_tuple(a...); // or tie(a...);
  // log(anyType(tuple));
  // return apply(lambda, tuple);
  }

template<class T>
T echo(T arg) {return arg;}

string test_echo(string s) { 
  out("echo: ", s);
  return s; }

TEST(Lazy){
  int X = 0;
  Lazy L([&]{ X++; }); 
  L(); CHECK(X==1);

  auto L2 = Lazy(test_echo, string("Lazy")); 
  L2();
  // cout << lambda(" test_echo\n");



  // auto s = say("say ", "hello\n"); // cout << s;
  // cout <<  run(test_echo, "run");



  // cout <<  run(sstr, "run", 123, "test_echo\n");

  // cout <<  run(log, "run log\n");

  // All lines bellow dont compile with error of 
  // no instance of ... matches the argument list 

  // Lazy(test_echo)

  // // BANG! run(function with variadic params)
  // cout << run(say, "run", "say\n"); 

  // // The dream: Lazy call of a variadic tempĺate
  // auto hello = Lazy(say, "Lazy "); 
  // hello("hello: ", 123, "\n"); // should printout Lazy hello: 123
}

}// unimock • Released under GPL 3.0

// https://stackoverflow.com/questions/687490/how-do-i-expand-a-tuple-into-variadic-template-functions-arguments
// https://stackoverflow.com/questions/19620818/stdtuple-as-class-member
// https://en.cppreference.com/w/cpp/experimental/apply
// https://www.fluentcpp.com/2020/10/16/tie-make_tuple-forward_as_tuple-how-to-build-a-tuple-in-cpp/




// template <typename Func, typename... Args>
// class Lazy {
//   Func&& func;

//   Lazy(Func&& lambda, Args&&... a) {
//    	ASSERT_INVOCABLE(Func, Args);
//     func = bind(forward<Func>(lambda), forward<Args>(args)...);
//     func();

//     // auto tup = forward_as_tuple(a...); // or tie(a...);
//     // log(anyType(tup));
//     // args = tup;
//     // func = lambda;
//   }

//   tuple<Args&&...> args;
  
//   auto operator()(Args&&... moreargs) {
//     // return func(args..., moreargs...); 
//     return apply(func, args, moreargs...); 
//     // return apply([&](Args&&... a, Args&&... b) { func(a..., b...); }, args, moreargs...);
//   }
// };

// template <typename... Args>
// using Event = std::function<void(Args...)>;

