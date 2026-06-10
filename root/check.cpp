#include <G4Types.hh>
#include <TError.h>
#include <TInterpreter.h>
#include <TROOT.h>
#include <type_traits>

void check() {  // NOLINT
  // c++ < 20, ROOT conflict
  // spdlog::info("check Double_t double {}", std::is_same_v<double, Double_t>);
  // spdlog::info("check G4double double {}", std::is_same_v<double, G4double>);

  Warning("check Double_t double ", "%d", std::is_same_v<double, Double_t>);
  Warning("check G4double double ", "%d", std::is_same_v<double, G4double>);
}
