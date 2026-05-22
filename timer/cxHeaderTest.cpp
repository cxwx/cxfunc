// TODO(CX): rootforum:t/it-there-any-way-for-lazyloading-or-progressing-for-root-prompt-now/62588
#include "benchmark.hh"
#include <TApplication.h>
#include <TInterpreter.h>
#include <spdlog/spdlog.h>
#include <string>
using namespace std;

namespace {

void f1clear() {
  if (gInterpreter) {
    gInterpreter->ResetAll();
  }
}

void f1(const string & str) {
  if (gInterpreter) {
    gInterpreter->ProcessLine(str.c_str());
  }
}

}  // namespace

void cxHeaderTest() {  // NOLINT
  spdlog::set_level(spdlog::level::debug);

  // 初始化 ROOT 环境
  if (!gInterpreter) {
    int argc = 0;
    char** argv = nullptr;
    TApplication* app = new TApplication("cxHeaderTest", &argc, argv);
  }

  // 初始化 benchmark
  gBenchmark = new TBenchmark;
  gErrorIgnoreLevel = kInfo;

  using namespace cxfunc::timer;

  benchmarkDecorator(f1, "CoordLib")("gSystem->Load(\"libcxthetaphi\")");
  benchmarkDecorator(f1, "root base")("#include <ROOT/RDataFrame.hxx>");
  benchmarkDecorator(f1, "spdlog")("#include <spdlog/spdlog.h>");
  benchmarkDecorator(f1, "yaml-cpp")("#include <yaml-cpp/yaml.h>");
  // benchmarkDecorator(f1, "sol2")("#include <sol/sol.hpp>");
  benchmarkDecorator(f1, "boost ini")("#include <boost/property_tree/ini_parser.hpp>");
  f1clear();  // 并没用
  benchmarkDecorator(f1, "boost xml")("#include <boost/property_tree/xml_parser.hpp>");
  benchmarkDecorator(f1, "wcs")("#include <wcslib/wcs.h>");
  benchmarkDecorator(f1, "cx Corsika IO")("#include <CorsikaIO/inc_all.hxx>");
  benchmarkDecorator(f1, "cx Coord")("#include <Coord_cx/incall.hh>");
}

int main() {
  cxHeaderTest();
}
