#ifndef ERROR_EXIT_HH__
#define ERROR_EXIT_HH__

#include <exception>
#include <iostream>
#include <string>
#include <utility>

namespace cxfunc::error {

template <typename Func>
auto errorExitDecorator(Func func, const std::string& fname, bool show_traceback = false) {
  return [func, fname, show_traceback](auto&&... args) -> auto {
    try {
      std::cout << "[ENTER] " << fname << '\n';

      if constexpr (std::is_void_v<std::invoke_result_t<Func, decltype(args)...>>) {
        func(std::forward<decltype(args)>(args)...);
        std::cout << "[SUCCESS] " << fname << " completed successfully" << '\n';
      } else {
        auto result = func(std::forward<decltype(args)>(args)...);
        std::cout << "[SUCCESS] " << fname << " completed successfully" << '\n';
        return result;
      }
    } catch (const std::exception& e) {
      std::cerr << "[ERROR] " << fname << " failed!" << '\n';
      std::cerr << "  Error Type: " << typeid(e).name() << '\n';
      std::cerr << "  Error Message: " << e.what() << '\n';

      if (show_traceback) {
        std::cerr << "  Function: " << fname << '\n';
        std::cerr << "  Exception caught in errorExitDecorator" << '\n';
      }
      throw;
    } catch (...) {
      std::cerr << "[ERROR] " << fname << " failed with unknown exception!\n";
      std::cerr << "  Function: " << fname << '\n';
      throw;
    }
  };
}

template <typename Func>
auto simpleErrorExit(Func func, const std::string& fname = "unknown") {
  return errorExitDecorator(func, fname, false);
}

}  // namespace cxfunc::error

#endif
