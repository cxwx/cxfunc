#ifndef BENCHMARK_HH__
#define BENCHMARK_HH__

#include <TBenchmark.h>
#include <TError.h>
#include <string>
#include <utility>

namespace cxfunc::timer {

template <typename Func>
auto benchmarkDecorator(Func func, const std::string& fname, TBenchmark* ag = gBenchmark) {
  return [func, fname, ag](auto&&... args) {
    if (gErrorIgnoreLevel <= kInfo) {
      ag->Start(fname.c_str());
      if constexpr (std::is_void_v<std::invoke_result_t<Func, decltype(args)...>>) {
        func(std::forward<decltype(args)>(args)...);
        ag->Stop(fname.c_str());
        ag->Show(fname.c_str());
        ag->Reset();
      } else {
        auto result = func(std::forward<decltype(args)>(args)...);
        ag->Stop(fname.c_str());
        ag->Show(fname.c_str());
        ag->Reset();
        return result;
      }
    } else {
      return func(std::forward<decltype(args)>(args)...);
    }
  };
}

}  // namespace cxfunc::timer

#endif
