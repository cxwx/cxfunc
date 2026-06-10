#ifndef NKGFUNC_CX___
#define NKGFUNC_CX___

#include <Math/IParamFunction.h>

namespace cxfunc::NKG {

class Function : public ROOT::Math::IParametricFunctionOneDim {
 private:
  const double *pars{};

 public:
  auto DoEvalPar(double x, const double *p) const -> double override;
  [[nodiscard]] auto Clone() const -> ROOT::Math::IParametricFunctionOneDim * override { return new Function(); }
  [[nodiscard]] auto Parameters() const -> const double * override { return pars; }
  void SetParameters(const double *p) override { pars = p; }
  [[nodiscard]] auto NPar() const -> unsigned int override { return 2; }  // Ne, s,
};

auto func(double ne, double age, double r = 50, double rm = 130) -> double;
auto func2(double ne, double r, double s, double rm, double (*as)(double s), double (*bs)(double s)) -> double;
}

#endif
