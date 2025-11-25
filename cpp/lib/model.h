//==============================================================================
// Mockup of core UniQ class model
//==============================================================================
#include "uniq.h"
namespace unimock {

typedef function<void()> voidfunction;

// #include "Actor.h"
//================================================================ Actor : Queue
template<typename T> struct Actor{
protected:
  bool _running = false;
public:
  voidfunction beat = [&]{ 
    uniq::log("Actor::beat()"); 
  };  

  template<typename F, typename... A> Actor(F&& f, A&&... args) {
   	ASSERT_INVOCABLE(F,A);
    beat = bind(forward<F>(f), forward<A>(args)...);
    start();
  }

  ~Actor(){ stop(); }

  virtual void start() { _running = true; }
  virtual void stop() { _running = false; }
  inline bool running() { return _running; }
  
  virtual int push(const T &item, bool wait=true) { return 0;  }
  virtual int pop(T &item, bool wait=true) { return 0; }

  virtual void onempty(){ }
  virtual void onfull(){ }
  // functor
  template<typename... Args> 
  // inline void operator()(Args&&... args){ bind(beat, forward<Args>(args)...)(); }
  inline void operator()(){ 
    this->beat(); } 
};

TEST(mockActor){
  int X = 0;
  auto L = [&]{ X++; };
  L(); CHECK(X==1);

  unimock::Actor<int>A([&]{ X++; }); CHECK(A.running());
  A(); CHECK(X==2);
  A.stop(); CHECK(!A.running());
};//*/

//======================================================================== Queue
template <typename T> struct Queue : public Actor<T>{
public:
  int size;
  vector<T> buffer;

  Queue(int size){ 
    buffer.resize(size); }

  template<typename F, typename... A> 
  Queue(int size, F&& f, A&&... args): Actor<T>(forward<F>(f), forward<A>(args)...) {
    buffer.resize(size);
   	// ASSERT_INVOCABLE(F,A);
    // Actor<T>::beat = bind(forward<F>(f), forward<A>(args)...);
    // this->beat();
    // Actor<T>(f, forward<A>(args)...);
    // Actor<T>(f, args...)
  };

  int push(const T &item, bool wait=true) override { 
    buffer.push_back(item); return 1; }

  int pop(T &item, bool wait=true) override { 
    auto r = !buffer.empty();
    if(r) item = buffer.back();
    return r; 
  }
};

TEST(mockQueue){
  int X = 0;
  Queue<int>Q(1, [&]{ X++; }); CHECK(Q.running());
  Q(); 
  CHECK(X==1); 

  CHECK(Q.push(1) > 0);
  int v;
  CHECK(Q.pop(v) && v==1);
};//*/

// #include "Worker.h"
//=============================================================== Worker : Actor 
class Worker : public Queue<voidfunction> {
 public:
  thread thrd;

  // template<typename F, typename... A> 
  // Queue(int size, F&& f, A&&... args): Actor<T>(forward<F>(f), forward<A>(args)...) {
 
  template<typename F, typename... A> 
  Worker(int queueSize, F&& f, A&&... args) : Queue<voidfunction>(queueSize, f, args...) {
   	// ASSERT_INVOCABLE(F,A);
    // beat = bind(forward<F>(f), forward<A>(args)...);
    // beat();
    thrd = thread(&Worker::loop, this); 
  };

  void join() { thrd.join(); }

  void loop() {
    voidfunction f;

    while (pop(f)) 
      f();

    // out("\n", colorcode(id), sstr("worker[", id, "]"));
  }

  template <typename F, typename... A>
  inline int run(F && f, A&&... args){
    return push(bind(forward<F>(f), forward<A>(args)...));
  }
};

TEST(mockWorker){
  int X = 0;
  auto L = [&]{ X++; };

  thread th( L );
  th.join(); CHECK(X==1);

  Worker W(1, [&]{ 
    X++; uniq::log("Worker::beat()"); } );

  CHECK(W.running());  
  W.join(); CHECK(X==2);

  // W();

};//*/

// #include "Helper.h"
/*/============================================================== Helper : Worker
class Helper : public Worker {
  integer id = Id("Helper");

};

TEST(mockHelper){
  Helper H; CHECK(H.running()); 
};//*/

// #include "WorkerPool.h"
/*/========================================================== WorkerPool : Worker
class WorkerPool : public Worker {
  
};

TEST(mockWorkerPool){
  WorkerPool WP; CHECK(WP.running()); 
};//*/

}// unimock

namespace uniq {

}// uniq • Released under GPL 3.0
