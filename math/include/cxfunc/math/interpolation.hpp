#ifndef CXFUNC_MATH_INTERPOLATION_HPP
#define CXFUNC_MATH_INTERPOLATION_HPP

#include <Eigen/Dense>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <cmath>

#include "cxfunc/math/version.hpp"

namespace cxfunc {
namespace math {
namespace interpolation {

/**
 * @brief Interpolation scale types
 */
enum class Scale {
    Linear,  ///< Linear scale: f(x) = x
    Log,     ///< Logarithmic scale: f(x) = log(x)
    Sqrt     ///< Square root scale: f(x) = sqrt(x)
};

/**
 * @brief Apply scale transformation to value
 * @param value Input value
 * @param scale Scale type
 * @return Transformed value
 */
inline double apply_scale(double value, Scale scale) {
    switch (scale) {
        case Scale::Linear:
            return value;
        case Scale::Log:
            if (value <= 0) {
                throw std::invalid_argument(
                    "Log scale requires positive values");
            }
            return std::log(value);
        case Scale::Sqrt:
            if (value < 0) {
                throw std::invalid_argument(
                    "Sqrt scale requires non-negative values");
            }
            return std::sqrt(value);
        default:
            throw std::invalid_argument("Unknown scale type");
    }
}

/**
 * @brief Apply inverse scale transformation
 * @param value Scaled value
 * @param scale Scale type
 * @return Unscaled (original scale) value
 */
inline double apply_inverse_scale(double value, Scale scale) {
    switch (scale) {
        case Scale::Linear:
            return value;
        case Scale::Log:
            return std::exp(value);
        case Scale::Sqrt:
            if (value < 0) {
                throw std::invalid_argument(
                    "Cannot compute square of negative sqrt-scaled value");
            }
            return value * value;
        default:
            throw std::invalid_argument("Unknown scale type");
    }
}

/**
 * @brief Apply scale transformation to array
 * @param values Input array
 * @param scale Scale type
 * @return Scaled array
 */
inline Eigen::ArrayXd apply_scale(const Eigen::ArrayXd& values, Scale scale) {
    Eigen::ArrayXd result(values.size());
    for (int i = 0; i < values.size(); ++i) {
        result[i] = apply_scale(values[i], scale);
    }
    return result;
}

/**
 * @brief Apply inverse scale transformation to array
 * @param values Scaled array
 * @param scale Scale type
 * @return Unscaled array
 */
inline Eigen::ArrayXd apply_inverse_scale(const Eigen::ArrayXd& values, Scale scale) {
    Eigen::ArrayXd result(values.size());
    for (int i = 0; i < values.size(); ++i) {
        result[i] = apply_inverse_scale(values[i], scale);
    }
    return result;
}

/**
 * @brief 1D linear interpolation
 * @param x_points X coordinates (must be monotonically increasing)
 * @param y_points Y values at x_points
 * @param x X coordinate to interpolate at
 * @return Interpolated Y value
 * @throws std::invalid_argument if x_points is not monotonically increasing
 *
 * Performs linear interpolation: y = y₀ + (y₁ - y₀) * (x - x₀) / (x₁ - x₀)
 */
inline double interpolate_1d(const Eigen::ArrayXd& x_points,
                             const Eigen::ArrayXd& y_points,
                             double x) {
    if (x_points.size() != y_points.size()) {
        throw std::invalid_argument(
            "interpolate_1d: x_points and y_points must have same size");
    }

    if (x_points.size() < 2) {
        throw std::invalid_argument(
            "interpolate_1d: need at least 2 points for interpolation");
    }

    // Check for monotonic increasing
    for (int i = 1; i < x_points.size(); ++i) {
        if (x_points[i] <= x_points[i - 1]) {
            throw std::invalid_argument(
                "interpolate_1d: x_points must be monotonically increasing");
        }
    }

    // Handle extrapolation (use edge values)
    if (x <= x_points[0]) {
        return y_points[0];
    }
    if (x >= x_points[x_points.size() - 1]) {
        return y_points[y_points.size() - 1];
    }

    // Find interpolation interval
    int idx = 0;
    for (int i = 0; i < x_points.size() - 1; ++i) {
        if (x >= x_points[i] && x < x_points[i + 1]) {
            idx = i;
            break;
        }
    }

    // Linear interpolation
    double x0 = x_points[idx];
    double x1 = x_points[idx + 1];
    double y0 = y_points[idx];
    double y1 = y_points[idx + 1];

    double t = (x - x0) / (x1 - x0);
    return y0 + t * (y1 - y0);
}

/**
 * @brief Scaled regular grid interpolator
 *
 * Multi-dimensional interpolator on a regular grid with support for
 * value scaling (linear, log, sqrt) to handle data spanning many
 * orders of magnitude.
 *
 * This is designed for interpolating model spectra, flux points,
 * and other regularly spaced astronomical data.
 *
 * Example:
 * @code
 * // Create energy axis (log-spaced)
 * Eigen::ArrayXd energy(10);
 * energy << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0;  // TeV
 *
 * // Create flux values (power law)
 * Eigen::ArrayXd flux(10);
 * flux = 1.0e-12 / energy.pow(2.0);  // E^-2 spectrum
 *
 * // Create interpolator with log scaling
 * std::vector<Eigen::ArrayXd> points = {energy};
 * interpolation::ScaledRegularGridInterpolator interpolator(
 *     points, flux, interpolation::Scale::Log, interpolation::Scale::Log);
 *
 * // Interpolate at 3.16 TeV
 * double result = interpolator.interpolate(3.16);
 * std::cout << "Flux at 3.16 TeV: " << result << std::endl;
 * @endcode
 */
class ScaledRegularGridInterpolator {
public:
    /**
     * @brief Construct scaled interpolator
     * @param points Grid points for each dimension
     * @param values Function values at grid points
     * @param points_scale Scale for coordinate axes
     * @param values_scale Scale for function values
     *
     * The grid points must be regularly spaced (monotonically increasing).
     * Dimensions with size 1 are ignored (constant along that dimension).
     */
    ScaledRegularGridInterpolator(
        const std::vector<Eigen::ArrayXd>& points,
        const Eigen::ArrayXd& values,
        Scale points_scale = Scale::Linear,
        Scale values_scale = Scale::Linear)
        : points_(points),
          values_(values),
          points_scale_(points_scale),
          values_scale_(values_scale) {

        if (points.empty()) {
            throw std::invalid_argument(
                "ScaledRegularGridInterpolator: points vector cannot be empty");
        }

        // Validate dimensions
        size_t total_size = 1;
        for (const auto& point : points) {
            if (point.size() < 1) {
                throw std::invalid_argument(
                    "ScaledRegularGridInterpolator: each dimension must have at least 1 point");
            }

            // Check for monotonic increasing
            for (int i = 1; i < point.size(); ++i) {
                if (point[i] <= point[i - 1]) {
                    throw std::invalid_argument(
                        "ScaledRegularGridInterpolator: points must be monotonically increasing");
                }
            }

            total_size *= point.size();
        }

        if (static_cast<size_t>(values.size()) != total_size) {
            throw std::invalid_argument(
                "ScaledRegularGridInterpolator: values size doesn't match grid dimensions");
        }

        // Scale the points and values
        scaled_points_.reserve(points.size());
        for (const auto& point : points) {
            scaled_points_.push_back(apply_scale(point, points_scale_));
        }

        scaled_values_ = apply_scale(values, values_scale_);

        // Track which dimensions have size > 1
        for (const auto& point : points) {
            include_dimensions_.push_back(point.size() > 1);
        }

        // Check if all dimensions are scalar
        all_scalar_ = std::none_of(include_dimensions_.begin(),
                                   include_dimensions_.end(),
                                   [](bool v) { return v; });
    }

    /**
     * @brief Interpolate at given coordinates
     * @param coords Coordinates for interpolation (one per dimension)
     * @return Interpolated value
     *
     * For 1D interpolation, pass a single coordinate:
     * @code
     * double result = interpolator.interpolate(3.16);
     * @endcode
     *
     * For multi-dimensional interpolation, pass multiple coordinates:
     * @code
     * double result = interpolator.interpolate(3.16, 0.5, 1.2);
     * @endcode
     */
    double interpolate(double coord) const {
        if (points_.size() != 1) {
            throw std::invalid_argument(
                "ScaledRegularGridInterpolator: number of coordinates doesn't match dimensions");
        }

        if (all_scalar_) {
            return apply_inverse_scale(scaled_values_[0], values_scale_);
        }

        std::vector<double> coords = {coord};
        return interpolate_nd(coords);
    }

    template<typename... Coords>
    double interpolate(Coords... coords) const {
        static_assert(sizeof...(coords) > 1,
                     "ScaledRegularGridInterpolator: use single argument for 1D interpolation");

        if (sizeof...(coords) != points_.size()) {
            throw std::invalid_argument(
                "ScaledRegularGridInterpolator: number of coordinates doesn't match dimensions");
        }

        if (all_scalar_) {
            return apply_inverse_scale(scaled_values_[0], values_scale_);
        }

        // Pack coordinates into vector
        std::vector<double> coord_array = {coords...};
        return interpolate_nd(coord_array);
    }

    /**
     * @brief Interpolate at vector of coordinates
     * @param coords Coordinates for each dimension
     * @return Interpolated value
     */
    double interpolate(const std::vector<double>& coords) const {
        if (coords.size() != points_.size()) {
            throw std::invalid_argument(
                "ScaledRegularGridInterpolator: number of coordinates doesn't match dimensions");
        }

        if (all_scalar_) {
            return apply_inverse_scale(scaled_values_[0], values_scale_);
        }

        return interpolate_nd(coords);
    }

    /**
     * @brief Get number of dimensions
     * @return Number of dimensions in the grid
     */
    size_t n_dim() const { return points_.size(); }

    /**
     * @brief Get grid points for a dimension
     * @param dim Dimension index
     * @return Grid points for the requested dimension
     */
    const Eigen::ArrayXd& points(size_t dim) const {
        return points_.at(dim);
    }

    /**
     * @brief Get values
     * @return Array of all grid values
     */
    const Eigen::ArrayXd& values() const { return values_; }

private:
    std::vector<Eigen::ArrayXd> points_;        ///< Original grid points
    Eigen::ArrayXd values_;                     ///< Original values
    std::vector<Eigen::ArrayXd> scaled_points_; ///< Scaled grid points
    Eigen::ArrayXd scaled_values_;              ///< Scaled values
    Scale points_scale_;                        ///< Scale for coordinates
    Scale values_scale_;                        ///< Scale for values
    std::vector<bool> include_dimensions_;      ///< Which dims have size > 1
    bool all_scalar_;                           ///< Whether all dims are size 1

    /**
     * @brief N-dimensional interpolation implementation
     * @param coords Coordinates for each dimension
     * @return Interpolated value
     */
    double interpolate_nd(const std::vector<double>& coords) const {
        // Scale coordinates
        std::vector<double> scaled_coords(coords.size());
        for (size_t i = 0; i < coords.size(); ++i) {
            if (include_dimensions_[i]) {
                scaled_coords[i] = apply_scale(coords[i], points_scale_);
            }
        }

        // Find indices and weights for interpolation
        std::vector<int> lower_indices(coords.size());
        std::vector<int> upper_indices(coords.size());
        std::vector<double> weights(coords.size());

        for (size_t dim = 0; dim < coords.size(); ++dim) {
            if (!include_dimensions_[dim]) {
                lower_indices[dim] = 0;
                upper_indices[dim] = 0;
                weights[dim] = 1.0;
                continue;
            }

            const auto& points = scaled_points_[dim];
            double x = scaled_coords[dim];

            // Find interval using binary search (O(log n) instead of O(n))
            int idx = 0;
            int left = 0;
            int right = points.size() - 1;

            // Handle extrapolation cases first
            if (x < points[0]) {
                lower_indices[dim] = 0;
                upper_indices[dim] = 0;
                weights[dim] = 0.0;
                continue;
            } else if (x >= points[right]) {
                lower_indices[dim] = right;
                upper_indices[dim] = right;
                weights[dim] = 1.0;
                continue;
            }

            // Binary search for the interval
            while (left < right) {
                int mid = left + (right - left) / 2;
                if (x < points[mid]) {
                    right = mid;
                } else if (x >= points[mid + 1]) {
                    left = mid + 1;
                } else {
                    // Found the interval [mid, mid+1]
                    idx = mid;
                    break;
                }
            }

            lower_indices[dim] = idx;
            upper_indices[dim] = idx + 1;

            // Linear interpolation weight
            double x0 = points[idx];
            double x1 = points[idx + 1];
            weights[dim] = (x - x0) / (x1 - x0);
        }

        // Compute multi-linear interpolation
        double result = 0.0;

        // For each corner of the hypercube
        int n_corners = 1 << coords.size();  // 2^n_dim
        for (int corner = 0; corner < n_corners; ++corner) {
            double weight = 1.0;
            int linear_index = 0;
            int stride = 1;

            for (size_t dim = 0; dim < coords.size(); ++dim) {
                if (!include_dimensions_[dim]) {
                    continue;
                }

                // Choose lower or upper index based on corner bit
                int idx = (corner & (1 << dim)) ? upper_indices[dim] : lower_indices[dim];

                // Accumulate weight
                double w = (corner & (1 << dim)) ? weights[dim] : (1.0 - weights[dim]);
                weight *= w;

                // Compute linear index
                linear_index += idx * stride;
                stride *= points_[dim].size();
            }

            result += weight * scaled_values_[linear_index];
        }

        // Inverse scale the result
        return apply_inverse_scale(result, values_scale_);
    }
};

/**
 * @brief Profile interpolation for flux points
 *
 * Specialized interpolator for flux points with asymmetric errors
 * and upper limits, commonly used in gamma-ray astronomy.
 *
 * @param energy_axis Energy values
 * @param dnde Differential flux values
 * @param dnde_errp Positive errors (upper)
 * @param dnde_errn Negative errors (lower)
 * @param energy Energy to interpolate at
 * @param scale Scale type (typically Log for flux)
 * @return Interpolated flux value
 */
inline double interpolate_profile(
    const Eigen::ArrayXd& energy_axis,
    const Eigen::ArrayXd& dnde,
    const Eigen::ArrayXd& dnde_errp,
    const Eigen::ArrayXd& dnde_errn,
    double energy,
    Scale scale = Scale::Log) {

    if (energy_axis.size() != dnde.size() ||
        energy_axis.size() != dnde_errp.size() ||
        energy_axis.size() != dnde_errn.size()) {
        throw std::invalid_argument(
            "interpolate_profile: all input arrays must have same size");
    }

    // Scale energy axis
    auto scaled_energy = apply_scale(energy_axis, scale);
    double scaled_target = apply_scale(energy, scale);

    // Find interpolation interval
    int idx = 0;
    for (int i = 0; i < scaled_energy.size() - 1; ++i) {
        if (scaled_target >= scaled_energy[i] && scaled_target < scaled_energy[i + 1]) {
            idx = i;
            break;
        }
    }

    // Interpolate with proper error propagation
    double x0 = scaled_energy[idx];
    double x1 = scaled_energy[idx + 1];
    double y0 = dnde[idx];
    double y1 = dnde[idx + 1];

    double errp0 = dnde_errp[idx];
    double errp1 = dnde_errp[idx + 1];
    double errn0 = dnde_errn[idx];
    double errn1 = dnde_errn[idx + 1];

    double t = (scaled_target - x0) / (x1 - x0);

    // Linear interpolation for value
    double value = y0 + t * (y1 - y0);

    // Interpolate errors (weighted average)
    double errp = errp0 + t * (errp1 - errp0);
    double errn = errn0 + t * (errn1 - errn0);

    // Return value (could return errors too if needed)
    return value;
}

} // namespace interpolation
} // namespace math
} // namespace cxfunc

#endif // CXFUNC_MATH_INTERPOLATION_HPP