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

// although it is good habit, you don't have to type 'std::' before many objects by including this line
using namespace std;

// TD: Case logic append / remove
// TD: Logging
// TD: Logger write -> Case distinctions
// TD: Parallelism

class Buffer {
public:
  Buffer(bool bounded, int bound = 0)
    : bounded(bounded), bound_limit(bound) {}

  void append(int i) {
    b.push_back(i);
  }
  int remove() {
    int r = b.front();
    b.erase(b.begin());
    return r;
  }
  void bound(int b) {
    bounded = true;
    bound_limit = b;
  }
  void unbound() {
    bounded = false;
    bound_limit = 0;
  }

private:
  vector<int> b;
  bool bounded;
  int bound_limit;
};

class Logger {
public:
  void write(string op, bool did_work) {}

  string read(int idx) {
    return l.at(idx);
  }
private:
  vector<string> l;
};


int main(int argc, char* argv[]) {
	return 0;
}

