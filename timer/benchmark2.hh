#ifndef BENCHMARK2_HH__
#define BENCHMARK2_HH__

#include <chrono>
#include <iostream>
#include <string>
#include <utility>
#include <iomanip>

namespace cxfunc::timer {

template <typename Func> auto benchmark2(Func func, const std::string &fname) {
  return [func, fname](auto &&...args) {
    using Clock = std::chrono::high_resolution_clock;
    auto start = Clock::now();

    if constexpr (std::is_void_v<std::invoke_result_t<Func, decltype(args)...>>) {
      func(std::forward<decltype(args)>(args)...);
      auto end = Clock::now();
      std::chrono::duration<double> elapsed = end - start;

      std::cout << std::fixed << std::setprecision(6);
      std::cout << "[Benchmark] " << fname << " : " << elapsed.count() << " seconds" << '\n';
    } else {
      auto result = func(std::forward<decltype(args)>(args)...);
      auto end = Clock::now();
      std::chrono::duration<double> elapsed = end - start;

      std::cout << std::fixed << std::setprecision(6);
      std::cout << "[Benchmark] " << fname << " : " << elapsed.count() << " seconds" << '\n';
      return result;
    }
  };
}

} // namespace cxfunc::timer

#endif
