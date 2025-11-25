//==============================================================================
// Hello • A starting point for C++ classes
//==============================================================================
#pragma once
#include "uniq.h"
namespace uniq {

// This represents overload set
template<class F1, class F2>
struct Joint : public F1, public F2 {
    using F1::operator(); 
    using F2::operator(); 
}; 

template<class F1, class F2>
Joint(F1, F2) -> Joint<F1, F2>; 

template<class F1, class F2>
auto join(F1&& f1, F2&& f2) {
    return Joint { std::forward<F1>(f1), std::forward<F2>(f2) }; 
}


template<class F, class F2, class... Fs>
auto join(F&& f, F2&& f2, Fs&&... fs) {
    return Joint{
        std::forward<F>(f),
        join(std::forward<F2>(f2), std::forward<Fs>(fs)...)
    };
}

//======================================================================== Hello
struct Hello: public Named {

};

//================================================================= TEST(Hello)
TEST(Join){

  // This code magically works
  auto var = std::make_pair(10, 20);

  auto result = std::visit(Joint{
      [](std::string const& s) { return "it's a string"; },
      [](std::pair<int, int> const& p) { return "it's a pair"; }
  }, var);

  // auto result = std::visit(custom::join(
  //     [](std::string const& s) { return "it's a string"; },
  //     [](std::pair<int, int> const& p) { return "it's a pair"; }
  // ), var);

    assert(result == "it's a string");

    auto lvalue_lambda = [](std::string const& s) { return "it's a string"; };
    result = std::visit(join(
        lvalue_lambda,
        [](std::pair<int, int> const& p) { return "it's a pair"; }
    ), var);

    assert(result == "it's a pair");
}//*/
}// uniq • Released under GPL 3.0