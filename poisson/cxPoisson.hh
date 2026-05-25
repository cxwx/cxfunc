#ifndef POISSON_HH_CX__
#define POISSON_HH_CX__

#include <cmath>
#include <tuple>

namespace cxfunc::poisson {
inline auto errorBar(double n) -> std::tuple<double, double> {
  double up = 0.5 + sqrt(n + 0.25);
  double down = -0.5 + sqrt(n + 0.25);
  return std::make_tuple(up, down);
}
}  // namespace cxfunc::poisson

#endif  // POISSON_HH_CX__
