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

enum Operation { Append, Remove, Bound, Unbound };

class Logger {
public:
  void write(Operation op, bool did_work) {
    lock_W();
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
    unlock_W();
  }

  string read(long unsigned int idx) {
    lock_R();
    if (l.empty() || l.size() <= idx) {
      unlock_R();
      lock_W();
      l.push_back("Logger: Operation 'Read' failed");
      unlock_W();
      return "";
    } else {
      string s = l.at(idx);
      unlock_R();
      return s;
    }
  }

  size_t size() {
    lock_R();
    size_t size = l.size();
    unlock_R();
    return size;
  }

private:
  vector<string> l;
  int readers = 0;
  mutex m_readers;
  mutex r;
  mutex t;

  void lock_W() {
    t.lock();
    r.lock();
  }

  void unlock_W() {
    r.unlock();
    t.unlock();
  }

  void lock_R() {
    t.lock();
    t.unlock();

    m_readers.lock();
    if (readers == 0) {
      r.lock();
    }
    readers += 1;
    m_readers.unlock();
  }

  void unlock_R() {
    m_readers.lock();
    readers -= 1;
    if (readers == 0) {
      r.unlock();
    }
    m_readers.unlock();
  }
};

class Buffer {
public:
  Buffer(bool bounded, long unsigned int bound = 0) : bounded(bounded), bound_limit(bound) {}

  Logger log;

  void append(int i) {
    lock_bound_R();
    if (bounded && b.size() >= bound_limit) {
      unlock_bound_R();
      log.write(Append, false);
    } else {
      unlock_bound_R();
      lock_buf_W();
      b.push_back(i);
      unlock_buf_W();
      log.write(Append, true);
    }
  }

  int remove() {
    lock_buf_R();
    if (b.empty()) {
      unlock_buf_R();
      log.write(Remove, false);
      return -1;
    } else {
      unlock_buf_R();
      lock_buf_W();
      int r = b.front();
      b.erase(b.begin());
      unlock_buf_W();
      log.write(Remove, true);
      return r;
    }
  }

  void bound(long unsigned int b) {
    bound_t.lock();
    bound_r.lock();
    lim_t.lock();
    lim_r.lock();

    bounded = true;
    bound_limit = b;

    lim_r.unlock();
    lim_t.unlock();
    bound_r.unlock();
    bound_t.unlock();

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
  long unsigned int bound_limit;

  int readers = 0;
  mutex m_readers;
  mutex bound_t;
  mutex bound_r;
  mutex lim_t;
  mutex lim_r;

  void lock_bound_R() {
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
  }

  void unlock_bound_R() {
    m_readers.lock();
    readers -= 1;
    if (readers == 0) {
      bound_r.unlock();
      lim_r.unlock();
    }
    m_readers.unlock();
  }

  void lock_buf_W() {
    buf_t.lock();
    buf_r.lock();
  }

  void unlock_buf_W() {
    buf_r.unlock();
    buf_t.unlock();
  }

  void lock_buf_R() {
    buf_t.lock();
    buf_t.unlock();

    m_r_buf.lock();
    if (r_buf == 0) {
      buf_r.lock();
    }
    r_buf += 1;
    m_r_buf.unlock();
  }

  void unlock_buf_R() {
    m_r_buf.lock();
    r_buf -= 1;
    if (r_buf == 0) {
      buf_r.unlock();
    }
    m_r_buf.unlock();
  }
};

// Our implementation should not have -1 being used as a value in the buffer,
// since -1 indicates a failed read due to empty buffer.

int main(int argc, char *argv[]) { return 0; }
