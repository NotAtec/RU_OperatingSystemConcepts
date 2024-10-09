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
      log_mutex.lock();
      l.push_back("Logger: Operation 'Read' failed");
      log_mutex.unlock();
      return "";
    } else {
      log_mutex.lock();
      log_mutex.unlock();
      return l.at(idx);
    }
  }

private:
  vector<string> l;
  mutex log_mutex;
};

class Buffer {
public:
  Buffer(bool bounded, int bound = 0) : bounded(bounded), bound_limit(bound) {}

  Logger log;

  void append(int i) {
    bound_t.lock();
    lim_t.lock();
    lim_t.unlock();
    bound_t.unlock();

    m_readers.lock();
    if (readers == 0) {
      bound_r.lock();
      lim_r.lock();
    }
    readers += 1;
    m_readers.unlock();

    if (bounded && b.size() >= bound_limit) {
      unlock_bound();
      log.write(Append, false);
    } else {
      unlock_bound();
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
    bound_t.lock();
    bound_r.lock();
    bounded = true;
    bound_r.unlock();
    bound_t.unlock();

    lim_t.lock();
    lim_r.lock();
    bound_limit = b;
    lim_r.unlock();
    lim_t.unlock();

    log.write(Bound, true);
  }

  void unbound() {
    bound_t.lock();
    bound_r.lock();
    bounded = false;
    bound_r.unlock();
    bound_t.unlock();

    log.write(Unbound, true);
  }

private:
  vector<int> b;

  int r_buf = 0;
  mutex m_r_buf;
  mutex buf_t;
  mutex buf_r;

  bool bounded;
  int bound_limit;

  int readers = 0;
  mutex m_readers;
  mutex bound_t;
  mutex bound_r;
  mutex lim_t;
  mutex lim_r;

  void unlock_bound() {
    m_readers.lock();
    readers -= 1;
    if (readers == 0) {
      bound_r.unlock();
      lim_r.unlock();
    }
    m_readers.unlock();
  }
};

// Our implementation should not have -1 being used as a value in the buffer,
// since -1 indicates a failed read due to empty buffer.

int main(int argc, char *argv[]) { return 0; }
