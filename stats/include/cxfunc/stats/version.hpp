#ifndef CXFUNC_STATS_VERSION_HPP
#define CXFUNC_STATS_VERSION_HPP

#include <string>

namespace cxfunc {
namespace stats {

// Version information
inline constexpr int VERSION_MAJOR = 1;
inline constexpr int VERSION_MINOR = 0;
inline constexpr int VERSION_PATCH = 0;

inline std::string version_string() {
    return std::to_string(VERSION_MAJOR) + "." +
           std::to_string(VERSION_MINOR) + "." +
           std::to_string(VERSION_PATCH);
}

// Compile-time configuration
inline constexpr double TRUNCATION_VALUE = 1e-25;
inline constexpr int MAX_ITERATIONS = 200;
inline constexpr double CONVERGENCE_TOLERANCE = 1e-10;

// Mathematical constants
inline constexpr double PI = 3.14159265358979323846;
inline constexpr double EULER_MASCHERONI = 0.57721566490153286060;

} // namespace stats
} // namespace cxfunc

#endif // CXFUNC_STATS_VERSION_HPP