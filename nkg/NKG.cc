#include "NKG.hh"
#include <TMath.h>
using namespace TMath;

namespace cxfunc::NKG {

auto Function::DoEvalPar(double x, const double *p) const -> double {
  double ne = p[0];
  double s = p[1];
  return func(ne, s, x, 130);
}
auto func(double ne, double age, double r, double rm) -> double {
  auto a = age - 2;
  auto b = age - 4.5;
  auto c = Gamma(-b) / (2 * Pi() * Gamma(a + 2) * Gamma(-b - a - 2));
  auto rho = c * Power(r / rm, a) * Power((r / rm) + 1., b) / (rm * rm);
  return ne * rho;
}

auto func2(double ne, double r, double s, double rm, double (*as)(double s), double (*bs)(double s)) -> double {
  auto a = as(s);
  auto b = bs(s);
  auto c = Gamma(-b) / (2 * Pi() * Gamma(a + 2) * Gamma(-b - a - 2));
  auto rho = c * Power(r / rm, a) * Power((r / rm) + 1., b) / (rm * rm);
  return ne * rho;
}
}
