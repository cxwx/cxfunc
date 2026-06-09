#ifndef CXFUNC_MATH_HPP
#define CXFUNC_MATH_HPP

/**
 * harper:ignore
 * This library provides mathematical tools for gamma-ray astronomy,
 * including Gaussian distributions, interpolation, and root finding.
 *
 * Key features:
 * - Gaussian PDFs: Gauss2DPDF, MultiGauss2D for PSF modeling
 * - Interpolation: ScaledRegularGridInterpolator with various scales
 * - Root finding: Robust algorithms for equation solving
 *
 * Dependencies:
 * - Eigen3 (required): Linear algebra and vector operations
 * - GSL (optional): Advanced root finding algorithms
 * - Boost.Math (optional): Special mathematical functions
 *
 * Usage example:
 * @code
 * #include <cxfunc/math.hpp>
 *
 * using namespace cxfunc::math;
 *
 * // Create a 2D Gaussian PDF
 * gauss::Gauss2DPDF gauss(0.5);  // sigma = 0.5 deg
 *
 * // Calculate containment radius
 * double radius = gauss.containment_radius(0.68);  // 68% containment
 *
 * // Interpolation
 * std::vector<Eigen::ArrayXd> points = {energy_axis};
 * Eigen::ArrayXd values = flux_values;
 * interpolation::ScaledRegularGridInterpolator interpolator(
 *     points, values, interpolation::Scale::Log);
 *
 * double result = interpolator(1.0);  // Interpolate at 1.0 TeV
 * @endcode
 */

#include "cxfunc/math/version.hpp"

// Core mathematical functions
#include "cxfunc/math/gauss.hpp"
#include "cxfunc/math/interpolation.hpp"
#include "cxfunc/math/roots.hpp"

#include <numbers>
#include <numeric>
#include <vector>

/**
 * @brief Main namespace for cxfunc math library
 */
namespace cxfunc {
/**
 * @brief Math namespace containing all mathematical functions
 */
namespace math {

template <typename T> auto sq(const T &x) -> T { return x * x; }
template <typename... Args> auto cxSq(const Args &...args) { return (sq(args) + ...); }
template <typename T> auto sq(const std::vector<T> &x) -> T {
  return std::accumulate(x.begin(), x.end(), 0, [](T acc, T x) -> auto { return acc + sq(x); });
}

namespace asgamma {
constexpr auto bestAngleVSsigma() -> double { return 1.5852010652445132; }
constexpr auto GaussSmoothFactor() -> double { return 1.25643120862617; }
}
namespace physics {
//== from gamera======
constexpr auto TeV_to_erg() -> double { return 1.602; }
constexpr auto pc_to_cm() -> double { return 3.0857e18; }
constexpr auto kpc_to_cm() -> double { return 3.0857e21; }
/* proton mass in g */
constexpr auto m_p_g() -> double { return 1.6726e-24; }
/* year in seconds */
constexpr auto yr_to_sec() -> double { return 3.15576e7; }
/* solar mass */
constexpr auto mSol() -> double { return 1.9891e33; }
/* Thomson cross section */
constexpr auto sigma_T() -> double { return 6.6524e-25; }
/* Electron mass in erg */
constexpr auto m_e() -> double { return 8.187e-7; }
/* Boltzmann constant (erg/K) */
constexpr auto kb() -> double { return 1.380658e-16; }
/* proton mass in erg */
constexpr auto m_p() -> double { return 1.50310854e-3; }
/* pi0 mass in erg */
constexpr auto m_pi() -> double { return 2.1622194e-4; }
/* parsec to cm */
constexpr auto c_speed() -> double { return 29979245800.; }
/* elementary charge */
constexpr auto el_charge() -> double { return 4.80320427e-10; }
/* classical electron radius (cm) */
constexpr auto eRadius() -> double { return 2.8179e-13; }
/* Planck's constant */
constexpr auto hp() -> double { return 6.62606896e-27; }
/* fine structure constant */
constexpr auto fineStructConst() -> double { return 7.2974e-3; }
/* hour in seconds */
constexpr auto h_to_sec() -> double { return 3.6e3; }
/* pc to lyr */
constexpr auto pc_to_lyr() -> double { return 3.26156; }
/* GeV->erg */
constexpr auto GeV_to_erg() -> double { return 1.602e-3; }
/* eV->erg */
constexpr auto eV_to_erg() -> double { return 1.602e-12; }
/* classical electron radius (cm) */
constexpr auto e_radius() -> double { return 2.8179e-13; }
/* AU to cm*/
constexpr auto AU_to_cm() -> double { return 1.496e13; }
/* Electron mass in g */
constexpr auto m_e_g() -> double { return 9.1093837015e-28; }
/* natural logarithm of 10 */
constexpr auto ln10() -> double { return 2.302585092994046; }
}

}

}

#endif // CXFUNC_MATH_HPP
