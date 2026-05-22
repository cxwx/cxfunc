#ifndef CXFUNC_MODELING_HPP
#define CXFUNC_MODELING_HPP

/**
 * @file cxfunc/modeling.hpp
 * @brief Main header file for cxfunc modeling library
 *
 * This library provides spectral and spatial modeling tools for gamma-ray astronomy,
 * including common model types, parameter management, and model evaluation.
 *
 * Key features:
 * - Spectral models: PowerLaw, ExpCutoffPowerLaw, LogParabola, etc.
 * - Spatial models: PointSource, Gaussian, Disk, Template, etc.
 * - Model evaluation: Fast evaluation at given coordinates
 * - Parameter management: Model parameters with constraints and scaling
 *
 * Dependencies:
 * - Eigen3 (required): Linear algebra and vector operations
 * - cxfunc_math (optional): Mathematical functions and interpolation
 * - cxfunc_maps (optional): Map and coordinate system support
 *
 * Usage example:
 * @code
 * #include <cxfunc/modeling.hpp>
 *
 * using namespace cxfunc::modeling::spectral;
 *
 * // Create power law model
 * PowerLaw pl(1e-12, 2.0, 1.0);  // amplitude, index, reference energy
 *
 * // Evaluate at different energies
 * std::vector<double> energies = {0.1, 1.0, 10.0};  // TeV
 * for (auto energy : energies) {
 *     double flux = pl.evaluate(energy);
 *     std::cout << "E = " << energy << " TeV: flux = " << flux << std::endl;
 * }
 * @endcode
 */

// Version information and physical constants
#include "cxfunc/modeling/version.hpp"

// Core modeling components
#include "cxfunc/modeling/spectral.hpp"
#include "cxfunc/modeling/spatial.hpp"
#include "cxfunc/modeling/parameters.hpp"

/**
 * @brief Main namespace for cxfunc modeling library
 */
namespace cxfunc {
/**
 * @brief Modeling namespace containing all model functions
 */
namespace modeling {
// All functions and classes are available in this namespace
}

} // namespace cxfunc

#endif // CXFUNC_MODELING_HPP