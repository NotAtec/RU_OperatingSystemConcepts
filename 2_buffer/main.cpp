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

// TD: Parallelism
enum Operation { Append, Remove, Bound, Unbound };

mutex log_mutex;

class Logger {
public:
  void write(Operation op, bool did_work) {
    switch (op) {
    case Append:
      log_mutex.lock();
      if (did_work) {
        l.push_back("Operation: 'Append' succeeded");
      } else {
        l.push_back("Operation: 'Append' failed");
      }
      log_mutex.unlock();
      break;
    case Remove:
      log_mutex.lock();
      if (did_work) {
        l.push_back("Operation: 'Remove' succeeded");
      } else {
        l.push_back("Operation: 'Remove' failed");
      }
      log_mutex.unlock();
      break;
    case Bound:
      log_mutex.lock();
      if (did_work) {
        l.push_back("Operation: 'Bound' succeeded");
      } else {
        l.push_back("Operation: 'Bound' failed");
      }
      log_mutex.unlock();
      break;
    case Unbound:
      log_mutex.lock();
      if (did_work) {
        l.push_back("Operation: 'Unbound' succeeded");
      } else {
        l.push_back("Operation: 'Unbound' failed");
      }
      log_mutex.unlock();
      break;
    };
  }

  string read(int idx) {
    if (l.empty() || l.size() <= idx) {
      l.push_back("Logger: Operation 'Read' failed");
      return "";
    } else {
      return l.at(idx);
    }
  }

private:
  vector<string> l;
};

class Buffer {
public:
  Buffer(bool bounded, int bound = 0) : bounded(bounded), bound_limit(bound) {}

  Logger log;

  void append(int i) {
    if (bounded && b.size() >= bound_limit) {
      log.write(Append, false);
    } else {
      b.push_back(i);
      log.write(Append, true);
    }
  }

  int remove() {
    if (b.empty()) {
      log.write(Remove, false);
      return -1;
    } else {
      int r = b.front();
      b.erase(b.begin());
      log.write(Remove, true);
      return r;
    }
  }

  void bound(int b) {
    bounded = true;
    bound_limit = b;
    log.write(Bound, true);
  }

  void unbound() {
    bounded = false;
    bound_limit = 0;
    log.write(Unbound, true);
  }

private:
  vector<int> b;
  bool bounded;
  int bound_limit;
};

// Our implementation prevents -1 from being used as a buffer state, since -1
// indicates a failed read due to empty buffer.

int main(int argc, char *argv[]) { return 0; }
