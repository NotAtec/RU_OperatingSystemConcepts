/**
  * Assignment: synchronization
  * Operating Systems

  * Student names:
  - Jeroen Brinkhorst s1101799
  - Jeske Groenheiden s1093553
*/

/**
  Hint: F2 (or Control-klik) on a functionname to jump to the definition
  Hint: Ctrl-space to auto complete a functionname/variable.
  */

// function/class definitions you are going to use
#include <algorithm>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// although it is good habit, you don't have to type 'std::' before many objects
// by including this line
using namespace std;

// TD: Logging
// TD: Parallelism
enum Operation { Append, Remove, Bound, Unbound };

class Buffer {
public:
  Buffer(bool bounded, int bound = 0) : bounded(bounded), bound_limit(bound) {}

  void append(int i) {
    if (bounded && b.size() >= bound_limit) {
      // Log error
    } else {
      b.push_back(i);
      // Log Success
    }
  }
  optional<int> remove() {
    if (b.empty()) {
      // Log Error
      return {};
    } else {
      int r = b.front();
      b.erase(b.begin());
      // Log Success
      return r;
    }
  }
  void bound(int b) {
    bounded = true;
    bound_limit = b;
    // Log Success
  }
  void unbound() {
    bounded = false;
    bound_limit = 0;
    // Log Success
  }

private:
  vector<int> b;
  bool bounded;
  int bound_limit;
};

class Logger {
public:
  void write(Operation op, bool did_work) {
    switch (op) {
    case Append:
      if (did_work) {
        l.push_back("Operation: 'Append' succeeded");
      } else {
        l.push_back("Operation: 'Append' failed");
      }
      break;
    case Remove:
      if (did_work) {
        l.push_back("Operation: 'Remove' succeeded");
      } else {
        l.push_back("Operation: 'Remove' failed");
      }
      break;
    case Bound:
      if (did_work) {
        l.push_back("Operation: 'Bound' succeeded");
      } else {
        l.push_back("Operation: 'Bound' failed");
      }
      break;
    case Unbound:
      if (did_work) {
        l.push_back("Operation: 'Unbound' succeeded");
      } else {
        l.push_back("Operation: 'Unbound' failed");
      }
      break;
    };
  }

  string read(int idx) { return l.at(idx); }

private:
  vector<string> l;
};

int main(int argc, char *argv[]) { return 0; }

