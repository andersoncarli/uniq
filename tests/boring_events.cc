// Event-driven ping-pong using the event system
// Demonstrates: Actors respond to events, all through single queue
// compile using ./build boring_events
#include "uniq.h"
using namespace uniq;

// Global event emitter - all events flow through single queue
EventEmitter events;

// Shared state
atomic<int> pingCount(0);
int target = 0;

int main(int argc, char* argv[]) {
  pool().showstats = true;
  pool().start();
  
  target = argc > 1 ? stoi(argv[1]) : 1000000;
  
  out("Event-driven ping-pong: ", target, " iterations\n");
  out("Architecture: Single Queue (pool().queue)\n");
  out("Events: 'ping' and 'pong' flow through ONE queue\n\n");
  
  Time t;
  
  // Ping Actor: listens to 'pong' events
  // If value > 0, emits 'ping' with value
  // If value <= 0, stops the pool
  events.on("pong", [](int v) {
    if (v > 0) {
      events.emit("ping", v);  // Goes to single queue
    } else {
      pool().stop();
    }
  });
  
  // Pong Actor: listens to 'ping' events
  // Decrements value and emits 'pong' with v-1
  events.on("ping", [](int v) {
    pingCount++;
    if (v % 100000 == 0) {
      out("togo: ", v, "\n");
    }
    events.emit("pong", v - 1);  // Goes to single queue
  });
  
  out("Registered handlers for 'ping' and 'pong'\n");
  out("Queue size before emit: ", pool().size(), "\n");
  
  // Start the flow: emit initial 'ping' event
  events.emit("ping", target);  // Goes to single queue
  
  out("Emitted initial 'ping' event with value: ", target, "\n");
  out("Queue size after emit: ", pool().size(), "\n");
  
  // Wait for all events to be processed
  pool().join();
  
  Time elapsed = t();
  
  out("\nTotal events processed: ", pingCount, "\n");
  out("Time: ", elapsed, "\n");
  out("All events flowed through single queue (pool().queue)\n");
  
  quick_exit(0);
}

