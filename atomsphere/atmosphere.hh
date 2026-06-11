#include <Math/IFunction.h>
namespace cxfunc::atmosphere {

class Function : public ROOT::Math::IBaseFunctionOneDim {
 public:
  [[nodiscard]] double DoEval(double x) const override;
  [[nodiscard]] ROOT::Math::IBaseFunctionOneDim* Clone() const override {
    return new Function();
  }
};
}  // namespace cxfunc::atmosphere

