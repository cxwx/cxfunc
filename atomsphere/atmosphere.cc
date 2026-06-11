// cspell:disable
#include <Math/IFunction.h>
#include <cmath>
#include "atmosphere.hh"
namespace cxfunc::atmosphere {
double cal(double h = 4300) {
  // https://en.wikipedia.org/wiki/Density_of_air
  const double p0 = 101.325;   // [kPa]
  const double T0 = 288.15;    // [K]
  const double g = 9.80665;    // [m/s2]
  const double L = 0.0065;     // [K/m]
  const double R = 8.31447;    // [J/(mol*K)]
  const double M = 0.0289644;  // [kg/mol]

  double T = T0 - (L * h);
  double p = p0 * pow(1 - (L * h / T0), g * M / R / L);
  double rho = p * M / R / T;
  return rho;
}
double Function::DoEval(double x) const {
  return cal(x);
}
}  // namespace cxfunc::atmosphere
