// #pragma once
#include "uniq.h"
#include "Any.h"
// namespace json {

typedef map<string, any> Json;

//   typedef map<const char*, >> CastTable;
//   const CastTable caster = {
//   const Json caster = {
//     {typeid(const char*).name(), [](any v){ return string(any_cast<const
//     char*>(v)); }}, {typeid(int).name(),         [](any v){ return
//     to_string(any_cast<int>(v)); }}, {typeid(bool).name(),        [](any v){
//     return to_string(any_cast<bool>(v)); }}, {typeid(string).name(), [](any
//     v){ return string(any_cast<string>(v)); }}
//   };

// class Json : public map<string, any>{ };

/*   json   is {object} or [array]
//   object is list of pairs between curly braces { "a":b, c:"d" }
//   list   is sequence of (pairs or values) separated by coma [],b,{},d,null,,a:"b",
//   pair   is key : value
//   key    is string // or tag
//   value  is string, number, true, false or null
//   string is anything between double quotes ".*"
//   array  is list of values [between,brackets,{},[],0]
//   number is ?- +digit ?frac ?exp
*/

// Parser =================================================================
class Parser {
  string source;
  int i = 0, line = 0, row = 0;
  char c;
  any current;
  string lastToken = "";
  string lastSpace = "";

  bool next(char stop = 0) {
    do {
      c = source[i++];
      if (!c || c == stop)
        return false;
      source.erase(0, 1);  // std::deque is faster
      lastToken += c;
      cout << c;
    } while (spaces(lastSpace));
    return true;
  }

  bool token(const string& t, bool req = false) {
    // assert(token("]",1));

    // if next token = t consome o t e retorna true
    cout << t;
  }

 protected:                   // productions
  bool spaces(string& res) {  // " ,\t,\r,\n"
    bool r = false;
    const string blanks = " \t\r\n";

    while (blanks.find(c)) {
      if (c == '\n')
        line++;
      next();
      r = true;
    }
    return r;
  }

  bool signal(int& res) {  // ?('+'|'-')
    if (c == '+')
      res = 1;
    else if (c == '-')
      res = -1;
    else
      return false;
    next();
    return true;
  }

  bool exp(double& res) {  // (e|E) ?('+'|'-') +digit
    int s = 1, d;
    if (c == 'e' || c == 'E')
      signal(s);
    digits(d, 1);
    res = pow(10, d * s);  // retornar a potencia correta.
    return true;
  }
  
  bool frac(double& d) {  // . +digit
    int i;
    if (c != '.')
      return false;
    digits(i, 1);
    d = i / 10;
    return true;
  }
  
  bool digits(int& res, bool req = 0) {  // (0|1|2|3|4|5|6|7|8|9)
    bool r = false;
    const string digits = "0123456789";
    while (digits.find(c) && next()) {
      r = true;
      res += c;
    }
    // stoi(
    assert(!req && r && "digit expected");
    return r;
  }
  
  bool number(double& res) {  // ?- +digit ?frac ?exp
    int m, s = 1;
    double f = 0, e = 0;

    signal(s);
    digits(m, 1);
    if (frac(f)) {
    };
    if (exp(e)) {
    };
  }
  
  bool str(string& res, bool req = false) {  // ".*"
    if (c != '"')
      return false;
    while (next('"')) {
      if (c == '\\' && source[1] == '"')
        next();
    }
    token("\"", 1);
    next();
  }
  
  bool boolean(string& res) {  // true|false
    return token("true") || token("false");
  }
  
  bool null(string& res) {  // null
    return (token("null"));
  }
  
  bool value(any& res) {  // (string|number|object|array|boolean|null)
    string s;
    return str(s);  // || number() || object() || array() || boolean() || null();
    // assert(false);
  }

  bool values(Json& res) {  // *[ value ?[,value] ] }
    any v;
    while (next(',') && value(v))
      ;
    return true;
  }

  bool array(Json& res) {  // '[' ?list ']'
    if (c != '[')
      return false;
    assert(values(res));
    token("]", 1);
    next();
    return true;
  }

  bool pair(Json& res) {  // string : value
    string key;
    str(key, 1);
    token(":", 1);

    // any v;
    // assert(value(&v));
    // res.insert ( pair<string,any>(key,100) );
    // res.push_back(p)
    return true;
  }

  bool object(Json& res) {  // { ?members }
    if (c != '{')
      return false;

    Json p = {};
    while (next('}') && pair(p)) {
      // add pair to collection
    }

    assert(c == '}');
    next();
    return true;
  }

  Json json(const string res) {  // (object|array) eof
    // auto x = pair<string, any>("Hello", "world");
    // return Json(x);
    return Json({{"Hello", "world"}});
    // Json data;
    // // while (next()) {
    // //   assert(object(data) || array(data) && "Invalid json");
    // // }
    // return data;
  }

 public:
  Parser(const Json grammar) {}
  ~Parser() {}
  // Json parse(string source, Json op = {{}}, Json ast = {{}}) {
  Json parse(string source, const Json op, Json& ast) { return json(source); };
};  // Parser */

Json json(const string source, const Json op, const Json& ast) {
  // pair<string, string> p({"dialect", "json"});
  // ast.insert(pair<string,int>("a",100));
  // ast.insert({"type", "object"});
  // Json res = Parser(jsonGrammar).parse(source, op, ast);

  // auto x = pair<string, string>("Hello", "world");
  // Json res = Json(x);
  Json res = {{"Hello", "world"}};
  // map<string, string> op = { dialect : "json" };
  // Json res = Parser(grammar).parse(source, op, ast);
  return res;
};

const string jsonSample = R"({
  "number": 123, "float": 1.234, "exp": -0.56e10, "zero": 0,
  "negexp": -1e-1,  "null": null, "true": true, "false": false,
  "str": "string", "escaped": "\"escaped\"",
  "deep": [[[[[[[[[[[[[[[[[[[["deep"]]]]]]]]]]]]]]]]]]],1]",
  "extra": [["a","b"],{},[{}, 1],[{}, true, {}, false, null, -1]]
})";

const Json jsonGrammar{
    {"json", "(object | array) eof"},
    {"object", "{ *[ pair ?[,pair] ] }"},
    {"pair", "string : value"},
    {"value", "string|number|object|array|boolean|null"},
    {"boolean", "true|false"},
    {"array", "'[' ?values ']'"},
    {"values", "value *[,value]"},
    {"number", "?- digits ?frac ?exp"},
    {"frac", ". digits"},
    {"exp", "e|E ?('+'|'-') digits"},
    {"digits", "+(0|1|2|3|4|5|6|7|8|9)"},
    {"string", ".*?"},
    {"space", " ,\t,\r,\n"}};

// }; // namespace json
//*///

typedef map<string, string> Grammar;

string print(const Grammar g, const Json op) {  //} = {{}}) {
  for (auto p : g)
    cout << p.first << ": " << p.second << endl;
  return "";
}

string print(const Json j, const Json op) {  // op = {{}}) {
  // bool dmg = anyto<bool>(op["demangle"]);
  // string s = anyToString(op["url"]);
  // cout << s;
  // cout << endl;
  for (auto o : op) {
    string n = o.second.type().name();
    cout << "\"" << o.first << "\": " << anyString(o.second) << "\t\t("
         << anyType(o.second) << ")" << endl;
  }
  return "";
}

void session(string s = "") { cout << "\n= " << s << " =============================================================>\n"; };

TEST(Json) {  // ========================================================= //

  const Json op = {{"url", "localhost"}, {"port", 80}, {"public", false}, {"dialect", "json"}, 
       {"tabsize", 2}, {"demangle", true}, {"zero", 0}, {"backtime", "-5.39e−44"}, {"e^iπ", -1}, {"grammar", jsonGrammar}};

  // auto v = anyto<int>(op["tabsize"]);
  // auto v2 = anyto<int>(op["tabsize"]);
  // auto v3 = op["tabsize"];
  // auto g = anyto<Json>(op["grammar"]);

  session("jsonSample");
  cout << jsonSample;

  session("jsonGrammar");
  print(jsonGrammar, op);

  session("op");
  print(op, op);

  // Json j = json(jsonSample, op, ast);
  // print(j, op);

  // session("ast");
  // Json ast; // = {{"a", "b"}};
  // print(ast, op);
  // cout << endl << endl;
}

/*
string AST = R"(
  {
    type: 'Object',
    children: <Property>[],
    loc?: Object
  },
  {
    type: 'Property',
    key: <Identifier>,
    value: Object | Array | <Literal>,
    loc?: Object
  },
  {
    type: 'Identifier',
    value: string,
    raw: string,
    loc?: Object
  },
  {
    type: 'Array',
    children: (Object | Array | <Literal>)[],
    loc?: Object
  },
  {
    type: 'Literal',
    value: string | number | boolean | null,
    raw: string,
    loc?: {start: 1, end: 7}
  })"
*/
// https://awesomeopensource.com/project/vtrushin/json-to-ast

// struct vec2d : std::array<float, 2>
// { }; // simplified
// std::array<float, 2> x = {1, 2};
// vec2d z = {1, 2};

// https://tools.ietf.org/html/rfc4627
// https://stackoverflow.com/questions/2583472/regex-to-validate-json
// https://github.com/dropbox/json11 204 +790 + 244
// https://github.com/nlohmann/json/blob/develop/single_include/nlohmann/json.hpp
// 25.855 https://github.com/amir-s/jute 53 332 22
// https://github.com/zserge/jsmn/blob/master/jsmn.h 440
