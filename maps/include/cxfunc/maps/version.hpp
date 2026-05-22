#ifndef CXFUNC_MAPS_VERSION_HPP
#define CXFUNC_MAPS_VERSION_HPP

#include <string>
#include <cmath>

namespace cxfunc {
namespace maps {

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
#ifndef M_PI
constexpr double M_PI = 3.14159265358979323846;
#endif

constexpr double RAD_TO_DEG = 180.0 / M_PI;    ///< Radians to degrees conversion
constexpr double DEG_TO_RAD = M_PI / 180.0;    ///< Degrees to radians conversion
constexpr double ARCSEC_TO_DEG = 1.0 / 3600.0; ///< Arcseconds to degrees
constexpr double ARCSEC_TO_RAD = M_PI / (180.0 * 3600.0); ///< Arcseconds to radians

} // namespace maps
} // namespace cxfunc

#endif // CXFUNC_MAPS_VERSION_HPP