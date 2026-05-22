#ifndef CXFUNC_MATH_HPP
#define CXFUNC_MATH_HPP

/**
 * @file cxfunc/math.hpp
 * @brief Main header file for cxfunc math library
 *
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

// Version information and configuration
#include "cxfunc/math/version.hpp"

// Core mathematical functions
#include "cxfunc/math/gauss.hpp"
#include "cxfunc/math/interpolation.hpp"
#include "cxfunc/math/roots.hpp"

/**
 * @brief Main namespace for cxfunc math library
 */
namespace cxfunc {
/**
 * @brief Math namespace containing all mathematical functions
 */
namespace math {
// All functions and classes are available in this namespace
}
}

#endif // CXFUNC_MATH_HPP