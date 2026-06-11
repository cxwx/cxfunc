#include "pause.hh"
#include <iostream>

using namespace std;

namespace cxfunc::debug {
void pause(char key) {
  char ch = ' ';
  while (cin.get(ch)) {
    cout << "Print enter to continue." << endl;
    if (ch == key) { return; }
  }
}
}  // namespace cxfunc::debug
