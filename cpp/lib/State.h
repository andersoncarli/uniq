// One state is defined as the initial state. When a machine starts to execute, it automatically enters this state.
// Each state can define actions that occur when a machine enters or exits that state. Actions will typically have side effects.
// Each state can define events that trigger a transition.
// A transition defines how a machine would react to the event, by exiting one state and entering another state.
// A transition can define actions that occur when the transition happens. Actions will typically have side effects.

//==============================================================================
// State is a 
// StateMachine or a collection of states
//==============================================================================
#pragma once
#include "uniq.h"
namespace uniq {

struct StateRecord{ string name; voidfunction onenter; voidfunction onexit; };

class State {
private:
  int current = -1;
  vector<StateRecord> states;
public:
  State(){};

  int id(const string state, bool check_=0){ // the numeric id of the state
    auto it = find_if(states.begin(), states.end(), [&](StateRecord &s)->bool { 
      // log(s.name, state);
      return s.name==state; 
    });

    int r = (it != states.end()) ? states.end()-it-1 : -1;
    log("state:", state," res:",r, " end-it:", states.end()-it, " it-begin:",it-states.begin());
    check(r>=0||!check_, "State[\"", state, "\"] not found");
    return r;
  }

  // transition 
  void on(const string name, voidfunction onenter, voidfunction onexit=nullptr){
    int i = id(name);
    if(i<0) states.push_back({name, onenter, onexit});
    else {
      states[i].onenter = onenter;
      states[i].onexit = onexit;
    }
    if(current<0) enter(name);
  }

  void remove(const string name){ 
    int i = id(name,1);
    states.erase(states.begin()+i);
  }

  // int operator[](const string name) const {
  //     int i = id(name, 1); 
  //     return i;
  // }

  string operator[](int id) const {
    check( id>=0 && id < states.size(), "State::id(", id, ") not found");
    return states[id].name;
  }

  State& enter(const string name){
    if(current>=0 && states[current].onexit) 
      states[current].onexit();

    int i = id(name, 1);
    states[i].onenter();
    current = i;
    return *this;
  }

  State& operator()(string s){ return enter(s);  }
  // State& operator =(string s){ return enter(s); }

  inline operator string() const { assert(current); return states[current].name; }
  inline operator int() const { assert(current); return current; }
  
  inline bool is(string s) { 
    return current && states[current].name == s; 
  }
  inline bool is(int i) { return current && current == i; }
};

ostream& operator<<(ostream& os, State& t) { 
  return os << "[" << string(t) << ":" << int(t) << "]"; 
}

//================================================================= TEST(State)
// TODO: fix this test
// TEST(State){
//   State S;

//   int ON=0, OFF=0;
//   S.on("off", [&]{ 
//     OFF++; log(GRN,"off"); }, [&]{ 
//       log(RED,"off"); });

//   S.on("on", [&]{ 
//     ON++; log(YEL,"on"); }, [&]{ 
//       log(MAG,"on"); });

//   // CHECK(S["off"] < S["on"]);
//   // CHECK_EXCEPTION(S["opz"]); // non existent state raises an exception

//   CHECK(OFF==1); 
//   CHECK(S.is("off")); 
//   CHECK(S.is(0)); 

//   // functor call changes the state
//   S("on"); 
//   CHECK(ON==1); 
//   CHECK(S.is("on")); 
//   CHECK(S.is(1)); 
  
//   S("off"); 
//   CHECK(OFF==2); 
//   CHECK(S.is("off")); 
// }//*/
// // https://kentcdodds.com/blog/implementing-a-simple-state-machine-library-in-javascript?ck_subscriber_id=739354581

}// uniq • Released under GPL 3.0