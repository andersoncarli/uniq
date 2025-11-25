
#pragma once
#include "call.h"

volatile u64 CURRENT_PROCESSOR_FREQ = 0;// 

namespace uniq {

// Timer ======================================================================
// É um Actor cujo beat se reapete ao longo do tempo, equanto este estiver ativo.
class Timer : Actor {
  Ativado a cada 1ms,
  registra estatisticas de 
  ciclos de clock
  tempo transcorrido
  
public:
  Timer(int interval): interval(interval){ }

  Timer(int interval_, void (*callback_)(void)) {
    interval = interval_;
    callback = callback_;
  }

  // Timer(int interval_, Actor other) {
  //   interval = interval_;
  //   callback = [&](){ other.beat();};
  // }

  // virtual void start(){ running = true; beat(); } 
  // virtual void stop(){ running = false; beat(); }

  virtual bool condition() { return 0; }

  int interval; // beat interval in microseconds : asap. -1: never
  void (*callback)(void);

  virtual void beat() override { if(callback) callback(); }; 
};

// Tests =======================================================================
void call(void (*callback)(int)){
  callback(1); // calls lambda which takes an int and returns void
}

TEST(Timer) {
  // call( [](int i){ log("lambda ", i); });

  Timer(1, [](){ 
    log("lambda"); 
  });

}

}// uniq • Released under GPL 3.0
