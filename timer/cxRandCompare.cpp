#include <CLHEP/Random/DualRand.h>
#include <CLHEP/Random/JamesRandom.h>
#include <CLHEP/Random/RandomEngine.h>
#include <CLHEP/Random/RanluxppEngine.h>
#include <TRandom3.h>
#include <TRandom.h>
#include <Randomize.hh>
#include "benchmark.hh"
#include <memory>
#include <spdlog/spdlog.h>
using std::make_unique;

using namespace std;
using namespace cxfunc::timer;

namespace {
auto cyc(int n) -> int {
  int a = 1;
  for (int i = 0; i < n; i++) {
    a = 1;
  }
  return a;
}

auto cycGeant42(int n) -> int {
  int a = 1;
  for (int i = 0; i < n; i++) {
    a = 1;
    static CLHEP::HepRandomEngine* ae = CLHEP::HepRandom::getTheEngine();
    ae->flat();
  }
  return a;
}
auto cycGeant4(int n) -> int {
  int a = 1;
  for (int i = 0; i < n; i++) {
    a = 1;
    G4UniformRand();
  }
  return a;
}

auto cycROOT(int n) -> int {
  int a = 1;
  for (int i = 0; i < n; i++) {
    a = 1;
    gRandom->Uniform();
  }
  return a;
}

void cyc1(int n) {
  for (int i = 0; i < n; i++) {
    gRandom->Uniform();
  }
}
}  // namespace

void cxRandCompare(int n = 300000000) {  // NOLINT
  gErrorIgnoreLevel = kPrint;
  gBenchmark = new TBenchmark;
  spdlog::set_level(spdlog::level::debug);
  benchmarkDecorator(cyc, "cyc")(n);
  benchmarkDecorator(cycGeant4, "Geant4 Default")(n);
  benchmarkDecorator(cycGeant42, "Geant4 Default flat")(n);
  benchmarkDecorator(cycROOT, "ROOT TRandom3")(n);

  std::unique_ptr<CLHEP::HepRandomEngine> engine;

  engine = make_unique<CLHEP::RanshiEngine>();
  G4Random::setTheEngine(engine.release());
  benchmarkDecorator(cycGeant4, "DualRand")(n);

  engine = std::make_unique<CLHEP::HepJamesRandom>();
  G4Random::setTheEngine(engine.release());
  benchmarkDecorator(cycGeant4, "James")(n);

  engine = std::make_unique<CLHEP::RanecuEngine>();
  G4Random::setTheEngine(engine.release());;
  benchmarkDecorator(cycGeant4, "Ranecu")(n);

  // G4Random::setTheEngine(new CLHEP::Ranlux64Engine);
  // benchmarkDecorator(cycGeant4, "Ranlux64")(n);
  //
  engine = make_unique<CLHEP::MixMaxRng>();
  G4Random::setTheEngine(engine.release());;
  benchmarkDecorator(cycGeant4, "MixMax")(n);

  engine = make_unique<CLHEP::MTwistEngine>();
  G4Random::setTheEngine(engine.release());;
  benchmarkDecorator(cycGeant4, "MTwistEngine")(n);

  engine = make_unique<CLHEP::RanluxEngine>();
  G4Random::setTheEngine(engine.release());;
  benchmarkDecorator(cycGeant4, "RanluxEngine")(n);

  engine = make_unique<CLHEP::RanluxppEngine>();
  G4Random::setTheEngine(engine.release());;
  benchmarkDecorator(cycGeant4, "RanluxppEngine")(n);

  engine = make_unique<CLHEP::RanshiEngine>();
  G4Random::setTheEngine(engine.release());;
  benchmarkDecorator(cycGeant4, "RanshiEngine")(n);

  // std::cout << benchmarkDecorator(cyc1, "")(n) << std::endl;
  /*benchmarkDecorator(cyc1, "")(n);*/
  benchmarkDecorator(cyc1, "")(n);
}

int main(int argc, const char* argv[]) {
  if (argc > 1) {
    char* endptr = nullptr;
    cxRandCompare(strtol(argv[1], &endptr, 10));
  } else {
    cxRandCompare();
  }
}
