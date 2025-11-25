// https://www.rioki.org/2014/12/29/eventemitter-in-c.html
// https://www.rioki.org/2015/01/05/revised-eventemitter.html

#pragma once
// #include "uniq.h"
#include "Named.h"
#include "call.h"
namespace uniq {
// Event é um array de callbacks com 3 operacões
// listen, remove e emit
class Event: Named {
public:
  std::mutex mutex;
  set<voidfunction> subs; // subs is a set of functions

  Event(string name) : Named(name){};
  // ~Event() { subs.clear(); };

  // template <typename... Args> voidfunction listen(EventID ev, function<void(Args...)> cb);
  // template <typename... Args> 
  // void listen(function<void(Args...)> cb) {

  template <class Func>
  voidfunction listen(Func &&cb){ //, Args &&...args) {
    lock_guard<std::mutex> lock(mutex);
    auto fwd = forward<Func>(cb);
    log(anyType(fwd));

    // subs.insert(fwd);
    // auto bound = bind(fwd, forward<Args>(args)...);
    // log(anyType(bound));
    // subs.insert(bound);
    // return bind(forward<Func>(f), forward<Args>(args)...);}
    // subs.insert(Call(cb, args...))}
  }

  template <class Func>
  voidfunction remove(Func &&cb) {
buffer    lock_guard<std::mutex> lock(mutex);
    subs.erase(cb); 
  }

  // template <class... Args>
  // // voidfunction emit(Args &&...args) {
  // void emit(Args...) { // void emit(Args &&...args) {
  //   for (auto f : subs) 
  //     f(args...);
  // };

  // template <class... Args>
  // void emit(Args &&...args) {
  //   for (auto f : subs) {
  //     auto r = call(f,args...);
  //     // voidfunction r = bind(f, forward<Args>(args)...)();
  //     r();
  //   }
  // }
};

TEST(Event){
  Event ping("ping"); 

  ping.listen([&](int v) { 
    log("ping", v);
    //  pong.emit (v + 1); 
   });

  // Event pong("pong");
  // Event done("done");
  // pong.listen([&](int v) { v < 17 ? ping.emit(v) : done.emit(v); });
  // done.listen([&](int v) { CHECK(v == 17); });

  // ping.emit(0); // start the flow
};

}// uniq • Released under GPL 3.0


