#pragma once
#include "uniq.h"
namespace uniq {

class WorkerPool;

struct Helper : public Worker {
  integer id = Id("Helper");
  WorkerPool& pool;
  Helper(WorkerPool &pool) : pool(pool) { 
    // thrd = thread(&Helper::beat, this);
  }
  void loop();
};

// ================================================================== WorkerPool
struct WorkerPool : public Worker {
  OpenQueue<Worker*> workers = OpenQueue<Worker*>(coreCount());

  WorkerPool(int queueSize = 1) : Worker(queueSize) { 
    workers.push(this); 
    // lambda = [](){ };
  };

  void onfull() override {
    Helper* h;
    if (workers.size() < 2) { //coreCount()
      h = new Helper(*this);
      workers.push(h);
      log("helper", h->id);
    }
  }

  bool showstats = false;
  // friend Helper;
};

// ============================================================== Helper::beat()
void Helper::loop() {
  voidfunction f;
  while(running){
    try {
      // while ((TaskID=pop(f,0)) || (TaskID=pool.pop(f,0))) 
      while (TaskID=pool.pop(f,0)) 
        f();
        stop();
      // todo: remover o helper da lista de helpers
      // pool.release()
    } catch (...) { handle_exception(); };
  };
  sleep(100);
  uniq::out(".");
  // if(showstats) out("\n", colorcode(id), sstr("worker[", id, "] handled ", done, " messages in ", total));
};

// ====================================================================== pool()
inline WorkerPool& workerPool() {
  static WorkerPool p;
  return p;
}

 // template <typename Func>
  // map<string, Func> funcs;
  // mutex mutex;

  // template <typename Func>
  // void register(const string name, Func &&f){
  //   lock_guard<mutex> lock(mutex);
  //   funcs.insert({name, f});
  // };

  // template <typename Func, typename... Args>
  // inline int run(const string name, Args &&...args) {
  //   Func f = funcs[name]; assert(f);
  //   return run(f, args...);
  // }

  // template <typename Func, typename... Args>
  // inline int atomic(Func &&f, Args &&...args) {
  //   voidfunction vf = bind(forward<Func>(f), forward<Args>(args)...);
  //   u64 address = *(long *)(char *)&vf; // https://stackoverflow.com/a/44236212/9464885
  //   // cout << address;
  //   address = rehash((u64)address);
  //   // cout << " " << address << "\n";
  //   return workers[address % size()]->push(vf);
  // }

// ============================================================ TEST(WorkerPool)
Atomic<int> test_rounds = 0;

void test_ping2(int N) { 
  if (N % 100'000 == 0){
    log("ping ", N);
    for (auto i = N-1; i >= N-100'000; i--)
    workerPool().run(test_ping2, i);
  }
  // run(test_ping, N-1);

  test_rounds++;
  if(N==0) workerPool().stop();
}

TEST(WorkerPool) {
  workerPool().run(test_ping, 1e6);
  
  workerPool().showstats = true;
  workerPool().join();
  
  CHECK(test_rounds==1e6);
}

}  // namespace uniq