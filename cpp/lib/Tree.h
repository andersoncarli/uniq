//==============================================================================
// Tree name/value in a parent/child
//==============================================================================
#include "uniq.h"
namespace uniq {

typedef function<void(const Tree&)> callback;

struct Tree {
  any value;
  string name;
  Tree* parent;
  vector<const Tree&> children;

  Tree(any v, Tree* parent = nullptr)
      : value(v), parent(parent){};
      
  Tree(any v, string name, Tree* parent = nullptr)
      : value(v), name(name), parent(parent){};

  int level() {
    int l = 0;
    Tree* n = this;
    while (n->parent) {
      n = n->parent;
      l++;
    }
    return l;
  }

  vector<string> path() {
    vector<string> v;
    Tree* n = this;
    while (n->parent) {
      v.push_back(n->name);
    }
    reverse(v.begin(), v.end());
    return v;
  }

  string path(const string separator) {
    vector<string> v = path();
    string s = join(v, separator);
    return s;
  }

  bool isLeaf() { return children.empty(); }
  bool isLast() { return !parent || (!parent->children.empty() && (&parent->children.back() == this)); }

  // void run(const callback cb) { cb(*this); }

  // Tree& last() { return &children.back(); }
  // Tree& first() { return &children[0]; }

  vector<string> keys() {
    vector<string> r;
    for (auto& n : children)
      r.push_back(n.name);
    return r;
  }

  Tree* get(const string name) {
    for (auto& n : children)
      if (n.name == name)
        return &n;
    return nullptr;
  }

  Tree& push(any val, string name) {
    Tree r = Tree(val, name, this);
    children.push_back(r);
    return r;
  }

  Tree& set(string name, any value) {
    Tree* r = get(name);
    if (r) {
      r->name = name;
      r->value = value;
    } else
      push(value, name);
    return *r;
  }
};

TEST(Tree) { // ============================================= test_Tree
  Tree a(1, "a"), b(2, "b", &a), c = b.push(nullptr, "c"), d();  //, e(3, "e", &d);


  assert(!a.parent);     // && !c.value)// && c.isLast)
  assert(a.name == "a");  // && !c.value)// && c.isLast)
  // assert(a.value==1);// && !c.value)// && c.isLast)

  // assert(d.name=="");// && !d.value && !d.parent)
  // d.parent = c; // assert(d.isLast)
  // assert(d.path() == "a.b.c..");
  // assert(e.path() == "a.b.c..e");
}
}  // namespace uniq


//     , tree = new Tree().load(model) //
//   // console.log(tree.toString())

//   let flat = tree.flat()
//   let res = flat.map(node => node.name).join(' ')
//   return res
// }, 'one.two.three one.two.four one.five one.six seven.eight.nine seven.ten')


// typedef map<string, any> mapTree;

  // const mapTree model = 
  // {"one", 
  //    {"two", { 
  //       {"three", 3}, 
  //       {"four", 4} }, 
  //    {"five", 5}, 
  //    {"six", 6 }, 
  //    {"seven", 
  //       {"eight", 
  //       { "nine", 9 }, 
  //       { "ten", 10} } };

  // const mapTree json = {{"url", "localhost"}, {"port", 80}, {"public", false}, {"dialect", "json"}, 
  //      {"tabsize", 2}, {"demangle", true}, {"zero", 0}, {"backtime", "-5.39e−44"}, {"e^iπ", -1}, {"grammar", jsonGrammar}};
