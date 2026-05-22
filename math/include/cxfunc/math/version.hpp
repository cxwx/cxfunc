#ifndef CXFUNC_MATH_VERSION_HPP
#define CXFUNC_MATH_VERSION_HPP

#include <string>

namespace cxfunc {
namespace math {

/// Version information
constexpr int MAJOR_VERSION = 1;
constexpr int MINOR_VERSION = 0;
constexpr int PATCH_VERSION = 0;

/// Version as string
inline std::string version() {
    return std::to_string(MAJOR_VERSION) + "." +
           std::to_string(MINOR_VERSION) + "." +
           std::to_string(PATCH_VERSION);
}

/// Configuration constants
constexpr double TRUNCATION_VALUE = 1e-25;  // For log/truncation safety

} // namespace math
} // namespace cxfunc

#endif // CXFUNC_MATH_VERSION_HPP