// del C method
// update to c++ std
//
#include "read.hh"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

void cxfunc::read::errorexit(int count, ...) {
  va_list args = nullptr;
  va_start(args, count);
  while ((count--) != 0) { cerr << args << " "; }
  va_end(args);
  cerr << '\n';
  std::terminate();
}

auto cxfunc::read::striget(const string &aString, int n) -> int {
  string tmp = cxfunc::read::strsget(aString, n);
  return (atoi(tmp.c_str()));
}

auto cxfunc::read::strfget(const string &aString, int n) -> double {
  string tmp = cxfunc::read::strsget(aString, n);
  return (atof(tmp.c_str()));
}

auto cxfunc::read::strsget(const string &aString, int n) -> string {
  string tmp;
  stringstream ss(aString.c_str());
  for (int i = 0; i < n; i++) { ss >> tmp; }
  if (tmp.size() == 0) {
    cerr << "err@strsget"
         << "cannot read string: \nssa]:" << aString.c_str() << ":[aas\n"
         << "num: " << n << " " << '\n';
    exit(1);
  }
  return tmp;
}

auto cxfunc::read::getline(ifstream &fin, string &s) -> bool {
  // return false when fin eof
lab_getline_cx_1:
  while (fin.peek() != EOF) {
    getline(fin, s);
    uint64_t n = s.size();
    if (s.size() == 0) { goto lab_getline_cx_1; }
    for (int i = 0; i < n; i++) {
      switch (s[i]) {
      case '\t':
      case ' ':
        break;
      case '#':
      case '!':
      case '\n':
        goto lab_getline_cx_1;
      default:
        return true;
      }
    }
  }
  return false;
}
