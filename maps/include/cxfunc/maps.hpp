#ifndef CXFUNC_MAPS_HPP
#define CXFUNC_MAPS_HPP

/**
 * @file cxfunc/maps.hpp
 * @brief Main header file for cxfunc maps library
 *
 * This library provides map and coordinate system tools for gamma-ray astronomy,
 * including coordinate axes, geometric transformations, and map operations.
 *
 * Key features:
 * - Map axes: Regular and irregular coordinate axes with interpolation
 * - Coordinate systems: Pixel ↔ World coordinate conversion
 * - Map geometries: WCS and HEALPix support
 * - Map operations: Cutting, slicing, and projection
 *
 * Dependencies:
 * - Eigen3 (required): Linear algebra and vector operations
 * - cxfunc_math (optional): Mathematical functions and interpolation
 *
 * Usage example:
 * @code
 * #include <cxfunc/maps.hpp>
 *
 * using namespace cxfunc::maps;
 *
 * // Create energy axis
 * Eigen::ArrayXd edges(6);
 * edges << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0;  // TeV
 *
 * MapAxis energy_axis(edges, "Energy", "TeV");
 * std::cout << "Number of bins: " << energy_axis.nbin() << std::endl;
 *
 * // Convert pixel to world coordinates
 * double pix = 5.5;
 * double world = energy_axis.pix_to_coord(pix);
 * std::cout << "Pixel " << pix << " → " << world << " TeV" << std::endl;
 * @endcode
 */

// Version information and configuration
#include "cxfunc/maps/version.hpp"

// Core map components
#include "cxfunc/maps/axes.hpp"

// Coordinate systems (to be implemented)
// #include "cxfunc/maps/coord.hpp"
// #include "cxfunc/maps/geom.hpp"

/**
 * @brief Main namespace for cxfunc maps library
 */
namespace cxfunc {
/**
 * @brief Maps namespace containing all map and coordinate functions
 */
namespace maps {
// All functions and classes are available in this namespace
}

} // namespace cxfunc

#endif // CXFUNC_MAPS_HPP