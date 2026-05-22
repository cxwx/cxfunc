#ifndef CXFUNC_MAPS_AXES_HPP
#define CXFUNC_MAPS_AXES_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "cxfunc/maps/version.hpp"

namespace cxfunc {
namespace maps {

/**
 * @brief Axis types for different coordinate systems
 */
enum class AxisType {
    Energy,    ///< Energy axis (TeV, GeV, etc.)
    Time,      ///< Time axis (MJD, seconds, etc.)
    Spatial,   ///< Spatial axis (degrees, radians, etc.)
    Label     ///< Label axis (discrete categories)
};

/**
 * @brief Coordinate interpolation method
 */
enum class InterpolationMethod {
    Linear,    ///< Linear interpolation
    Log,       ///< Logarithmic interpolation
    Sqrt       ///< Square root interpolation
};

/**
 * @brief Map axis class for handling 1D coordinate axes
 *
 * Represents a coordinate axis with bin edges and centers,
 * supporting various interpolation methods and coordinate conversions.
 *
 * Supports both regular (evenly spaced) and irregular grids.
 *
 * Example:
 * @code
 * // Create energy axis with log-spaced bins
 * Eigen::ArrayXd edges(6);
 * edges << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0;  // TeV
 *
 * MapAxis energy_axis(edges, "energy", "TeV");
 * energy_axis.set_interpolation_method(InterpolationMethod::Log);
 *
 * // Get bin properties
 * int n_bins = energy_axis.nbin();
 * double center = energy_axis.center(2);      // Center of bin 2
 * double width = energy_axis.width(2);        // Width of bin 2
 *
 * // Coordinate conversion
 * double pix = 2.5;  // Pixel coordinate
 * double energy = energy_axis.pix_to_coord(pix);  // Energy in TeV
 * double pix_back = energy_axis.coord_to_pix(energy);  // Back to pixel
 * @endcode
 */
class MapAxis {
public:
    /**
     * @brief Construct map axis from bin edges
     * @param edges Bin edges (length = n_bins + 1)
     * @param name Axis name
     * @param unit Unit string
     * @param type Axis type
     *
     * The edges define n_bins = edges.size() - 1 bins.
     * Bins are half-open: [edges[i], edges[i+1])
     */
    MapAxis(const Eigen::ArrayXd& edges,
           const std::string& name = "axis",
           const std::string& unit = "",
           AxisType type = AxisType::Spatial)
        : edges_(edges),
          name_(name),
          unit_(unit),
          type_(type),
          interp_method_(InterpolationMethod::Linear) {

        if (edges.size() < 2) {
            throw std::invalid_argument(
                "MapAxis: edges must have at least 2 elements");
        }

        // Check for monotonically increasing edges
        for (int i = 1; i < edges.size(); ++i) {
            if (edges[i] <= edges[i - 1]) {
                throw std::invalid_argument(
                    "MapAxis: edges must be monotonically increasing");
            }
        }

        // Compute bin centers
        compute_centers();
        compute_widths();

        // Check if regularly spaced
        is_regular_ = check_regular();
    }

    /**
     * @brief Construct regular axis with equal bin widths
     * @param n_bins Number of bins
     * @param lo Lower edge
     * @param hi Upper edge
     * @param name Axis name
     * @param unit Unit string
     * @param type Axis type
     */
    MapAxis(int n_bins, double lo, double hi,
           const std::string& name = "axis",
           const std::string& unit = "",
           AxisType type = AxisType::Spatial)
        : name_(name),
          unit_(unit),
          type_(type),
          interp_method_(InterpolationMethod::Linear),
          is_regular_(true) {

        if (n_bins <= 0) {
            throw std::invalid_argument("MapAxis: n_bins must be positive");
        }

        if (lo >= hi) {
            throw std::invalid_argument("MapAxis: lo must be < hi");
        }

        // Create regular edges
        edges_.resize(n_bins + 1);
        double bin_width = (hi - lo) / n_bins;
        for (int i = 0; i <= n_bins; ++i) {
            edges_[i] = lo + i * bin_width;
        }

        compute_centers();
        compute_widths();
    }

    // ==================== Properties ====================

    /**
     * @brief Get number of bins
     * @return Number of bins
     */
    int nbin() const { return edges_.size() - 1; }

    /**
     * @brief Get bin edges
     * @return Array of bin edges
     */
    const Eigen::ArrayXd& edges() const { return edges_; }

    /**
     * @brief Get bin centers
     * @return Array of bin centers
     */
    const Eigen::ArrayXd& centers() const { return centers_; }

    /**
     * @brief Get bin widths
     * @return Array of bin widths
     */
    const Eigen::ArrayXd& widths() const { return widths_; }

    /**
     * @brief Get axis name
     * @return Axis name
     */
    const std::string& name() const { return name_; }

    /**
     * @brief Get axis unit
     * @return Unit string
     */
    const std::string& unit() const { return unit_; }

    /**
     * @brief Get axis type
     * @return Axis type
     */
    AxisType type() const { return type_; }

    /**
     * @brief Check if axis is regularly spaced
     * @return True if bins have equal width
     */
    bool is_regular() const { return is_regular_; }

    /**
     * @brief Set interpolation method
     * @param method Interpolation method
     */
    void set_interpolation_method(InterpolationMethod method) {
        interp_method_ = method;
    }

    /**
     * @brief Get interpolation method
     * @return Current interpolation method
     */
    InterpolationMethod interpolation_method() const { return interp_method_; }

    // ==================== Bin Properties ====================

    /**
     * @brief Get center of a specific bin
     * @param bin Bin index (0-based)
     * @return Center coordinate of the bin
     */
    double center(int bin) const {
        if (bin < 0 || bin >= nbin()) {
            throw std::out_of_range("MapAxis::center: bin index out of range");
        }
        return centers_[bin];
    }

    /**
     * @brief Get width of a specific bin
     * @param bin Bin index (0-based)
     * @return Width of the bin
     */
    double width(int bin) const {
        if (bin < 0 || bin >= nbin()) {
            throw std::out_of_range("MapAxis::width: bin index out of range");
        }
        return widths_[bin];
    }

    /**
     * @brief Get edges of a specific bin
     * @param bin Bin index (0-based)
     * @return Pair of (lower_edge, upper_edge)
     */
    std::pair<double, double> bin_edges(int bin) const {
        if (bin < 0 || bin >= nbin()) {
            throw std::out_of_range("MapAxis::bin_edges: bin index out of range");
        }
        return {edges_[bin], edges_[bin + 1]};
    }

    // ==================== Coordinate Conversion ====================

    /**
     * @brief Convert pixel coordinate to world coordinate
     * @param pix Pixel coordinate (can be fractional)
     * @return World coordinate
     *
     * For integer pixels, this returns the bin center.
     * For fractional pixels, this interpolates.
     */
    double pix_to_coord(double pix) const {
        // Clamp to valid range
        pix = std::max(0.0, std::min(pix, static_cast<double>(nbin())));

        // For regular axes, use direct calculation
        if (is_regular_) {
            return edges_[0] + (pix + 0.5) * widths_[0];
        }

        // For irregular axes, interpolate
        return pix_to_coord_interpolate(pix);
    }

    /**
     * @brief Convert world coordinate to pixel coordinate
     * @param coord World coordinate
     * @return Pixel coordinate
     *
     * Returns the pixel coordinate where the bin center
     * is closest to the given world coordinate.
     */
    double coord_to_pix(double coord) const {
        // Clamp to valid range
        coord = std::max(edges_[0], std::min(coord, edges_[nbin()]));

        // For regular axes, use direct calculation
        if (is_regular_) {
            return (coord - edges_[0]) / widths_[0] - 0.5;
        }

        // For irregular axes, search
        return coord_to_pix_search(coord);
    }

    /**
     * @brief Convert pixel to coordinate with interpolation
     * @param pix Pixel coordinate
     * @return World coordinate with interpolation method applied
     */
    double pix_to_coord_interpolated(double pix) const {
        // Get base coordinate
        double coord = pix_to_coord(pix);

        // Apply interpolation method
        switch (interp_method_) {
            case InterpolationMethod::Linear:
                return coord;
            case InterpolationMethod::Log:
                if (coord <= 0) {
                    throw std::runtime_error("MapAxis: cannot apply log scale to non-positive values");
                }
                return std::log(coord);
            case InterpolationMethod::Sqrt:
                if (coord < 0) {
                    throw std::runtime_error("MapAxis: cannot apply sqrt scale to negative values");
                }
                return std::sqrt(coord);
        }
        return coord;  // Should not reach here
    }

    /**
     * @brief Find bin index containing a coordinate
     * @param coord World coordinate
     * @return Bin index (0-based)
     */
    int coord_to_bin(double coord) const {
        if (coord < edges_[0] || coord > edges_[nbin()]) {
            throw std::out_of_range("MapAxis::coord_to_bin: coordinate out of range");
        }

        // For regular axes, use direct calculation
        if (is_regular_) {
            int bin = static_cast<int>((coord - edges_[0]) / widths_[0]);
            return std::min(bin, nbin() - 1);
        }

        // For irregular axes, search
        return coord_to_pix_search(coord);
    }

    /**
     * @brief Get extent of the axis
     * @return Pair of (lower_edge, upper_edge)
     */
    std::pair<double, double> extent() const {
        return {edges_[0], edges_[nbin()]};
    }

    /**
     * @brief Create a sliced view of this axis
     * @param lo Lower bound (pixel coordinates)
     * @param hi Upper bound (pixel coordinates)
     * @return New MapAxis with sliced range
     */
    MapAxis slice(double lo, double hi) const {
        int lo_bin = static_cast<int>(std::floor(lo));
        int hi_bin = static_cast<int>(std::ceil(hi));

        lo_bin = std::max(0, lo_bin);
        hi_bin = std::min(nbin(), hi_bin);

        // Extract edges for the slice
        int n_slice_edges = hi_bin - lo_bin + 1;
        Eigen::ArrayXd slice_edges(n_slice_edges);

        for (int i = 0; i < n_slice_edges; ++i) {
            slice_edges[i] = edges_[lo_bin + i];
        }

        return MapAxis(slice_edges, name_, unit_, type_);
    }

private:
    Eigen::ArrayXd edges_;     ///< Bin edges
    Eigen::ArrayXd centers_;   ///< Bin centers
    Eigen::ArrayXd widths_;    ///< Bin widths
    std::string name_;         ///< Axis name
    std::string unit_;         ///< Axis unit
    AxisType type_;            ///< Axis type
    InterpolationMethod interp_method_;  ///< Interpolation method
    bool is_regular_;          ///< Whether bins are regularly spaced

    /**
     * @brief Compute bin centers from edges
     */
    void compute_centers() {
        int n = nbin();
        centers_.resize(n);
        for (int i = 0; i < n; ++i) {
            centers_[i] = (edges_[i] + edges_[i + 1]) / 2.0;
        }
    }

    /**
     * @brief Compute bin widths from edges
     */
    void compute_widths() {
        int n = nbin();
        widths_.resize(n);
        for (int i = 0; i < n; ++i) {
            widths_[i] = edges_[i + 1] - edges_[i];
        }
    }

    /**
     * @brief Check if bins are regularly spaced
     * @return True if all bins have approximately equal width
     */
    bool check_regular() const {
        if (widths_.size() == 0) return true;

        double first_width = widths_[0];
        double tolerance = 1e-10;

        for (int i = 1; i < widths_.size(); ++i) {
            if (std::abs(widths_[i] - first_width) > tolerance) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Interpolated pixel to coordinate conversion for irregular axes
     * @param pix Pixel coordinate
     * @return World coordinate
     */
    double pix_to_coord_interpolate(double pix) const {
        if (pix <= 0) {
            return edges_[0];
        }
        if (pix >= nbin()) {
            return edges_[nbin()];
        }

        int bin = static_cast<int>(std::floor(pix));
        double frac = pix - bin;

        // Linear interpolation between bin edges
        return edges_[bin] + frac * (edges_[bin + 1] - edges_[bin]);
    }

    /**
     * @brief Search-based coordinate to pixel conversion for irregular axes
     * @param coord World coordinate
     * @return Pixel coordinate
     */
    double coord_to_pix_search(double coord) const {
        // Binary search for the containing bin
        int lo = 0;
        int hi = nbin();

        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (coord < centers_[mid]) {
                hi = mid;
            } else {
                lo = mid + 1;
            }
        }

        // Linear interpolation within the bin
        int bin = std::min(lo, nbin() - 1);
        double bin_center = centers_[bin];
        double bin_width = widths_[bin];

        if (bin_width > 0) {
            return bin + (coord - bin_center) / bin_width;
        } else {
            return bin;
        }
    }
};

/**
 * @brief Multi-dimensional map axes container
 *
 * Manages multiple coordinate axes for multi-dimensional maps.
 * Provides convenient access to individual axes and global properties.
 *
 * Example:
 * @code
 * // Create 3D map (energy, longitude, latitude)
 * MapAxis energy_axis(...);
 * MapAxis lon_axis(...);
 * MapAxis lat_axis(...);
 *
 * MapAxes axes({energy_axis, lon_axis, lat_axis});
 *
 * int n_dim = axes.n_dim();
 * int n_bins_energy = axes[0].nbin();
 * int n_bins_lon = axes[1].nbin();
 * int n_bins_lat = axes[2].nbin();
 * @endcode
 */
class MapAxes {
public:
    /**
     * @brief Construct map axes from vector of individual axes
     * @param axes Vector of MapAxis objects
     */
    explicit MapAxes(const std::vector<MapAxis>& axes)
        : axes_(axes) {

        if (axes_.empty()) {
            throw std::invalid_argument("MapAxes: cannot construct with empty axes vector");
        }
    }

    /**
     * @brief Construct map axes from initializer list
     * @param axes Initializer list of MapAxis objects
     */
    MapAxes(std::initializer_list<MapAxis> axes)
        : axes_(axes) {

        if (axes_.empty()) {
            throw std::invalid_argument("MapAxes: cannot construct with empty axes list");
        }
    }

    /**
     * @brief Get number of dimensions
     * @return Number of axes
     */
    size_t n_dim() const { return axes_.size(); }

    /**
     * @brief Get total number of bins across all dimensions
     * @return Product of bin counts
     */
    size_t n_bins() const {
        size_t total = 1;
        for (const auto& axis : axes_) {
            total *= axis.nbin();
        }
        return total;
    }

    /**
     * @brief Access axis by index
     * @param i Axis index
     * @return Reference to the axis
     */
    const MapAxis& operator[](size_t i) const {
        return axes_.at(i);
    }

    /**
     * @brief Access axis by index
     * @param i Axis index
     * @return Reference to the axis
     */
    MapAxis& operator[](size_t i) {
        return axes_.at(i);
    }

    /**
     * @brief Get vector of all axes
     * @return Const reference to axes vector
     */
    const std::vector<MapAxis>& get_axes() const { return axes_; }

    /**
     * @brief Check if all axes are regularly spaced
     * @return True if all axes have regular spacing
     */
    bool is_regular() const {
        for (const auto& axis : axes_) {
            if (!axis.is_regular()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Get shape of the map
     * @return Vector of bin counts per dimension
     */
    std::vector<int> shape() const {
        std::vector<int> shape_vec;
        for (const auto& axis : axes_) {
            shape_vec.push_back(axis.nbin());
        }
        return shape_vec;
    }

    /**
     * @brief Convert multi-dimensional pixel to world coordinates
     * @param pix Pixel coordinates (one per dimension)
     * @return World coordinates
     */
    std::vector<double> pix_to_coord(const std::vector<double>& pix) const {
        if (pix.size() != n_dim()) {
            throw std::invalid_argument(
                "MapAxes::pix_to_coord: pixel coordinates dimension mismatch");
        }

        std::vector<double> coord(n_dim());
        for (size_t i = 0; i < n_dim(); ++i) {
            coord[i] = axes_[i].pix_to_coord(pix[i]);
        }
        return coord;
    }

    /**
     * @brief Convert world to pixel coordinates
     * @param coord World coordinates (one per dimension)
     * @return Pixel coordinates
     */
    std::vector<double> coord_to_pix(const std::vector<double>& coord) const {
        if (coord.size() != n_dim()) {
            throw std::invalid_argument(
                "MapAxes::coord_to_pix: world coordinates dimension mismatch");
        }

        std::vector<double> pix(n_dim());
        for (size_t i = 0; i < n_dim(); ++i) {
            pix[i] = axes_[i].coord_to_pix(coord[i]);
        }
        return pix;
    }

    /**
     * @brief Convert multi-dimensional pixel index to pixel coordinates
     * @param idx Flat pixel index
     * @return Multi-dimensional pixel coordinates
     */
    std::vector<int> idx_to_pix(size_t idx) const {
        std::vector<int> pix(n_dim());
        size_t temp = idx;

        for (int i = n_dim() - 1; i >= 0; --i) {
            int n_bins = axes_[i].nbin();
            pix[i] = temp % n_bins;
            temp /= n_bins;
        }

        return pix;
    }

    /**
     * @brief Convert multi-dimensional pixel coordinates to flat index
     * @param pix Pixel coordinates (one per dimension)
     * @return Flat pixel index
     */
    size_t pix_to_idx(const std::vector<int>& pix) const {
        if (pix.size() != n_dim()) {
            throw std::invalid_argument(
                "MapAxes::pix_to_idx: pixel coordinates dimension mismatch");
        }

        size_t idx = 0;
        size_t stride = 1;

        for (int i = n_dim() - 1; i >= 0; --i) {
            idx += pix[i] * stride;
            stride *= axes_[i].nbin();
        }

        return idx;
    }

private:
    std::vector<MapAxis> axes_;  ///< Vector of coordinate axes
};

// ==================== Convenience Functions ====================

/**
 * @brief Create a regular energy axis
 * @param n_bins Number of bins
 * @param energy_min Minimum energy (TeV)
 * @param energy_max Maximum energy (TeV)
 * @return MapAxis for energy
 */
inline MapAxis create_energy_axis(int n_bins, double energy_min, double energy_max) {
    return MapAxis(n_bins, energy_min, energy_max, "energy", "TeV", AxisType::Energy);
}

/**
 * @brief Create a regular spatial axis
 * @param n_bins Number of bins
 * @param lon_min Minimum longitude (degrees)
 * @param lon_max Maximum longitude (degrees)
 * @return MapAxis for spatial coordinates
 */
inline MapAxis create_spatial_axis(int n_bins, double lon_min, double lon_max) {
    return MapAxis(n_bins, lon_min, lon_max, "lon", "deg", AxisType::Spatial);
}

/**
 * @brief Create a time axis from MJD values
 * @param mjd_values Array of Modified Julian Date values
 * @param name Axis name (default: "time")
 * @return MapAxis for time
 */
inline MapAxis create_time_axis(const Eigen::ArrayXd& mjd_values,
                               const std::string& name = "time") {
    return MapAxis(mjd_values, name, "MJD", AxisType::Time);
}

} // namespace maps
} // namespace cxfunc

#endif // CXFUNC_MAPS_AXES_HPP