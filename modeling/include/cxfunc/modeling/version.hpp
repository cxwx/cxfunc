#ifndef CXFUNC_MODELING_VERSION_HPP
#define CXFUNC_MODELING_VERSION_HPP

#include <string>
#include <cmath>

namespace cxfunc {
namespace modeling {

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

/// Physical constants
constexpr double ELECTRON_CHARGE = 1.602176634e-19;      ///< Elementary charge (C)
constexpr double SPEED_OF_LIGHT = 2.99792458e8;       ///< Speed of light (m/s)
constexpr double PLANCK_CONSTANT = 6.62607015e-34;    ///< Planck constant (J·s)
constexpr double BOLTZMANN_CONSTANT = 1.380649e-23;   ///< Boltzmann constant (J/K)

/// Astronomical constants
constexpr double PARSEC_TO_KM = 3.085677581e13;         ///< Parsec to km
constexpr double LIGHT_YEAR_TO_KM = 9.4607304725808e12; ///< Light year to km
constexpr double DEG_TO_RAD = M_PI / 180.0;             ///< Degrees to radians

} // namespace modeling
} // namespace cxfunc

#endif // CXFUNC_MODELING_VERSION_HPP