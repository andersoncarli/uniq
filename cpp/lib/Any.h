//==============================================================================
// Any simpler std:any
//==============================================================================
#include "uniq.h"

namespace uniq {

typedef function<string(const any &v)> anyfunction;

typedef map<const string, const anyfunction> castTable;

const castTable table = {
  {typeid(const char*).name(), [](any v){ return string(any_cast<const char*>(v)); }}, 
  {typeid(int).name(),         [](any v){ return to_string(any_cast<int>(v)); }},   
  {typeid(bool).name(),        [](any v){ return to_string(any_cast<bool>(v)); }},      
  {typeid(string).name(),      [](any v){ return string(any_cast<string>(v)); }} 
};

class Any : public any {
public:
  Any(){};
  Any(const any& v) : any(v){};
  Any operator=(const any& v) { return any::operator=(v); }

  // const type_info& type() const noexcept
  string typeText() {
    string t = type().name();
    t = demangle(t.c_str());
    t = replace(t,"std::","");
    t = replace(t,"__cxx11::","");
    t = replace(t,"basic_string<char, char_traits<char>, allocator<char> >","string");
    t = replace(t,"char const .*? ","char");
    // t = replace(t,"\\w+",  ORA+"$0"+GRY); t = GRY+t;
    return t;
  }

  template <typename T>
  void is(T) { cout << sizeof(T); }
};

template <class T> inline T anyto(const any& v) {
  // if (v.type() == typeid(T))
  return any_cast<T>(v);
}

string anyString(const any v) {
  string r = "unknown", t = v.type().name();
  if      (t == typeid(const char*).name()) r = string(anyto<const char*>(v));  // "PKc"
  else if (t == typeid(int).name()) r = to_string(int(anyto<int>(v)));
  else if (t == typeid(bool).name()) r = to_string(anyto<bool>(v));
  else if (t == typeid(string).name()) r = anyto<string>(v);
  return r;
}

TEST(Any) {
  any a=1;
  CHECK(any_cast<int>(a) == 1);
  // log(anyString(a));

  a = string("abc");
  CHECK(any_cast<string>(a) == "abc");
  // log(anyString(a));
}

}// uniq • Released under GPL 3.0