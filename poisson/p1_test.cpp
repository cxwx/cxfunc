#include <cmath>
#include <iostream>

#include "cxPoisson.hh"

using namespace std;

static void p1_test() {
  for (int i = 0; i < 1000; i++) {
    auto [up, down] = cxfunc::poisson::errorBar(i);
    cout << i << " " << up << " " << down << " " << (up - sqrt(i)) / up << " "
         << (down - sqrt(i)) / down << "\n";
  }
}

int main() {
  p1_test();
  return 0;
}
