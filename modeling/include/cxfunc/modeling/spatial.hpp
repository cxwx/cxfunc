#ifndef CXFUNC_MODELING_SPATIAL_HPP
#define CXFUNC_MODELING_SPATIAL_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>
#include <limits>

#include "cxfunc/modeling/version.hpp"

namespace cxfunc {
namespace modeling {
namespace spatial {

// Constants for coordinate conversion
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

constexpr double DEG_TO_RAD = M_PI / 180.0;
constexpr double RAD_TO_DEG = 180.0 / M_PI;

/**
 * @brief Compute angular distance between two points on a sphere
 * @param lon1, lat1 First point coordinates (degrees)
 * @param lon2, lat2 Second point coordinates (degrees)
 * @return Angular separation (degrees)
 *
 * Uses the Haversine formula for accurate distance calculation on a sphere.
 */
inline double compute_angular_distance(double lon1, double lat1,
                                       double lon2, double lat2) {
    double lon1_rad = lon1 * DEG_TO_RAD;
    double lat1_rad = lat1 * DEG_TO_RAD;
    double lon2_rad = lon2 * DEG_TO_RAD;
    double lat2_rad = lat2 * DEG_TO_RAD;

    // Haversine formula
    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;

    double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
               std::cos(lat1_rad) * std::cos(lat2_rad) *
               std::sin(dlon / 2.0) * std::sin(dlon / 2.0);

    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));

    return c * RAD_TO_DEG;  // Convert to degrees
}

/**
 * @brief Abstract base class for spatial models
 *
 * All spatial models should inherit from this class and implement
 * the evaluate() method to compute spatial probability density at given coordinates.
 */
class SpatialModel {
public:
    virtual ~SpatialModel() = default;

    /**
     * @brief Evaluate spatial model at given coordinates
     * @param lon Longitude coordinate (degrees)
     * @param lat Latitude coordinate (degrees)
     * @return Spatial probability density (dimensionless, normalized to 1)
     */
    virtual double evaluate(double lon, double lat = 0.0) const = 0;

    /**
     * @brief Evaluate spatial model at multiple coordinates
     * @param lon Array of longitude values (degrees)
     * @param lat Array of latitude values (degrees, optional)
     * @return Array of spatial probability density values
     */
    virtual Eigen::ArrayXd evaluate(const Eigen::ArrayXd& lon,
                                    const Eigen::ArrayXd& lat = Eigen::ArrayXd()) const {
        if (lat.size() == 0) {
            Eigen::ArrayXd result(lon.size());
            for (int i = 0; i < lon.size(); ++i) {
                result[i] = evaluate(lon[i], 0.0);
            }
            return result;
        }

        if (lon.size() != lat.size()) {
            throw std::invalid_argument("SpatialModel: lon and lat arrays must have same size");
        }

        Eigen::ArrayXd result(lon.size());
        for (int i = 0; i < lon.size(); ++i) {
            result[i] = evaluate(lon[i], lat[i]);
        }
        return result;
    }

    /**
     * @brief Get model description
     * @return Human-readable model description
     */
    virtual std::string description() const = 0;

    /**
     * @brief Get model type name
     * @return Model type identifier
     */
    virtual std::string type() const = 0;
};

/**
 * @brief Point source spatial model
 *
 * Simplest spatial model: all flux concentrated at a single point.
 * Uses Dirac delta function approximation with small Gaussian width.
 *
 * Formula: delta(r) where r is angular distance from source position
 *
 * Commonly used for:
 * - Distant point sources (AGN, pulsars)
 * - Sources with size smaller than instrument PSF
 *
 * Example:
 * @code
 * PointSource ps(83.63, 22.01);  // Crab Nebula position (lon, lat in degrees)
 *
 * // At source position (within numerical tolerance)
 * double flux_at_source = ps.evaluate(83.63, 22.01);  // ≈ 1.0
 *
 * // Away from source
 * double flux_away = ps.evaluate(84.0, 22.0);  // ≈ 0.0
 * @endcode
 */
class PointSource : public SpatialModel {
public:
    /**
     * @brief Construct point source model
     * @param lon_0 Source longitude (degrees)
     * @param lat_0 Source latitude (degrees)
     * @param epsilon Small numerical width for Dirac delta approximation (default 1e-6)
     */
    PointSource(double lon_0, double lat_0, double epsilon = 1e-6)
        : lon_0_(lon_0), lat_0_(lat_0), epsilon_(epsilon) {

        if (std::abs(lat_0) > 90.0) {
            throw std::invalid_argument("PointSource: latitude must be between -90 and 90 degrees");
        }
    }

    /**
     * @brief Evaluate point source at given coordinates
     * @param lon Longitude (degrees)
     * @param lat Latitude (degrees)
     * @return Spatial probability density
     */
    double evaluate(double lon, double lat = 0.0) const override {
        double dr = compute_angular_distance(lon, lat, lon_0_, lat_0_);

        // Dirac delta approximation using narrow Gaussian
        if (dr < epsilon_) {
            // Approximate delta function: peak value ≈ 1/(2*pi*epsilon^2)
            double sigma = epsilon_;
            double norm = 1.0 / (2.0 * M_PI * sigma * sigma);
            return norm * std::exp(-0.5 * (dr * dr) / (sigma * sigma));
        }
        return 0.0;
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Point Source at (lon=" + std::to_string(lon_0_) +
               " deg, lat=" + std::to_string(lat_0_) + " deg)";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "PointSource";
    }

    /**
     * @brief Get source longitude
     * @return Longitude in degrees
     */
    double lon_0() const { return lon_0_; }

    /**
     * @brief Get source latitude
     * @return Latitude in degrees
     */
    double lat_0() const { return lat_0_; }

private:
    double lon_0_;      ///< Source longitude (degrees)
    double lat_0_;      ///< Source latitude (degrees)
    double epsilon_;    ///< Numerical width for delta approximation
};

/**
 * @brief Gaussian spatial model
 *
 * Symmetric Gaussian (2D normal distribution) on a sphere:
 * PSF(r) = (1 / (2*pi*sigma^2)) * exp(-r^2 / (2*sigma^2))
 *
 * where r is angular distance from source center.
 *
 * Commonly used for:
 * - Instrument PSF (Point Spread Function)
 * - Extended sources with Gaussian morphology
 * - Source convolution with PSF
 *
 * Example:
 * @code
 * Gaussian gauss(83.63, 22.01, 0.1);  // lon, lat, sigma (degrees)
 *
 * // At source center
 * double flux_center = gauss.evaluate(83.63, 22.01);  // Peak value
 *
 * // At 1 sigma from center
 * double flux_1sigma = gauss.evaluate(83.63, 22.1);  // exp(-0.5) ≈ 0.607 * peak
 *
 * // Containment fraction
 * double frac = gauss.containment_fraction(0.2);  // Fraction within 0.2 deg
 * @endcode
 */
class Gaussian : public SpatialModel {
public:
    /**
     * @brief Construct Gaussian spatial model
     * @param lon_0 Center longitude (degrees)
     * @param lat_0 Center latitude (degrees)
     * @param sigma Gaussian width (degrees)
     */
    Gaussian(double lon_0, double lat_0, double sigma)
        : lon_0_(lon_0), lat_0_(lat_0), sigma_(sigma) {

        if (sigma <= 0) {
            throw std::invalid_argument("Gaussian: sigma must be positive");
        }

        if (std::abs(lat_0) > 90.0) {
            throw std::invalid_argument("Gaussian: latitude must be between -90 and 90 degrees");
        }
    }

    /**
     * @brief Evaluate Gaussian at given coordinates
     * @param lon Longitude (degrees)
     * @param lat Latitude (degrees)
     * @return Spatial probability density
     */
    double evaluate(double lon, double lat = 0.0) const override {
        double dr = compute_angular_distance(lon, lat, lon_0_, lat_0_);

        double norm = 1.0 / (2.0 * M_PI * sigma_ * sigma_);
        double exponent = -0.5 * (dr * dr) / (sigma_ * sigma_);

        return norm * std::exp(exponent);
    }

    /**
     * @brief Evaluate Gaussian at multiple coordinates
     * @param lon Array of longitude values (degrees)
     * @param lat Array of latitude values (degrees)
     * @return Array of spatial probability density values
     */
    Eigen::ArrayXd evaluate(const Eigen::ArrayXd& lon,
                            const Eigen::ArrayXd& lat) const override {
        Eigen::ArrayXd result(lon.size());

        for (int i = 0; i < lon.size(); ++i) {
            double dr = compute_angular_distance(lon[i], lat[i], lon_0_, lat_0_);

            double norm = 1.0 / (2.0 * M_PI * sigma_ * sigma_);
            double exponent = -0.5 * (dr * dr) / (sigma_ * sigma_);

            result[i] = norm * std::exp(exponent);
        }

        return result;
    }

    /**
     * @brief Compute containment fraction within radius
     * @param radius Radius from center (degrees)
     * @return Fraction of total flux contained within radius
     */
    double containment_fraction(double radius) const {
        double x = (radius * radius) / (sigma_ * sigma_);
        return 1.0 - std::exp(-0.5 * x);
    }

    /**
     * @brief Compute radius containing given fraction of flux
     * @param fraction Desired containment fraction (0-1)
     * @return Radius containing that fraction (degrees)
     */
    double containment_radius(double fraction) const {
        if (fraction <= 0) {
            return 0.0;
        }
        if (fraction >= 1.0) {
            return std::numeric_limits<double>::infinity();
        }

        return sigma_ * std::sqrt(-2.0 * std::log(1.0 - fraction));
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Gaussian: center=(" + std::to_string(lon_0_) + ", " + std::to_string(lat_0_) +
               ") deg, sigma=" + std::to_string(sigma_) + " deg";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "Gaussian";
    }

    /**
     * @brief Get center longitude
     * @return Longitude in degrees
     */
    double lon_0() const { return lon_0_; }

    /**
     * @brief Get center latitude
     * @return Latitude in degrees
     */
    double lat_0() const { return lat_0_; }

    /**
     * @brief Get Gaussian sigma
     * @return Sigma in degrees
     */
    double sigma() const { return sigma_; }

private:
    double lon_0_;   ///< Center longitude (degrees)
    double lat_0_;   ///< Center latitude (degrees)
    double sigma_;   ///< Gaussian width (degrees)
};

/**
 * @brief Disk spatial model
 *
 * Uniform brightness within a disk:
 * PSF(r) = 1 / (pi*R^2) for r <= R, 0 otherwise
 *
 * where R is the disk radius.
 *
 * Commonly used for:
 * - Extended sources with uniform morphology
 * - Shell-type supernova remnants
 * - Simple extended source models
 *
 * Example:
 * @code
 * Disk disk(83.63, 22.01, 0.5);  // lon, lat, radius (degrees)
 *
 * // At source center
 * double flux_center = disk.evaluate(83.63, 22.01);  // 1/(pi*R^2)
 *
 * // Just inside edge
 * double flux_edge = disk.evaluate(83.63, 22.5);  // Same as center
 *
 * // Outside disk
 * double flux_outside = disk.evaluate(84.0, 22.0);  // 0.0
 * @endcode
 */
class Disk : public SpatialModel {
public:
    /**
     * @brief Construct disk spatial model
     * @param lon_0 Center longitude (degrees)
     * @param lat_0 Center latitude (degrees)
     * @param radius Disk radius (degrees)
     */
    Disk(double lon_0, double lat_0, double radius)
        : lon_0_(lon_0), lat_0_(lat_0), radius_(radius) {

        if (radius <= 0) {
            throw std::invalid_argument("Disk: radius must be positive");
        }

        if (std::abs(lat_0) > 90.0) {
            throw std::invalid_argument("Disk: latitude must be between -90 and 90 degrees");
        }

        norm_ = 1.0 / (M_PI * radius * radius);
    }

    /**
     * @brief Evaluate disk at given coordinates
     * @param lon Longitude (degrees)
     * @param lat Latitude (degrees)
     * @return Spatial probability density
     */
    double evaluate(double lon, double lat = 0.0) const override {
        double dr = compute_angular_distance(lon, lat, lon_0_, lat_0_);

        if (dr <= radius_) {
            return norm_;
        }
        return 0.0;
    }

    /**
     * @brief Evaluate disk at multiple coordinates
     * @param lon Array of longitude values (degrees)
     * @param lat Array of latitude values (degrees)
     * @return Array of spatial probability density values
     */
    Eigen::ArrayXd evaluate(const Eigen::ArrayXd& lon,
                            const Eigen::ArrayXd& lat) const override {
        Eigen::ArrayXd result(lon.size());

        for (int i = 0; i < lon.size(); ++i) {
            double dr = compute_angular_distance(lon[i], lat[i], lon_0_, lat_0_);

            result[i] = (dr <= radius_) ? norm_ : 0.0;
        }

        return result;
    }

    /**
     * @brief Compute containment fraction within radius
     * @param radius Radius from center (degrees)
     * @return Fraction of total flux contained within radius
     */
    double containment_fraction(double radius) const {
        if (radius >= radius_) {
            return 1.0;
        }
        return (radius * radius) / (radius_ * radius_);
    }

    /**
     * @brief Compute radius containing given fraction of flux
     * @param fraction Desired containment fraction (0-1)
     * @return Radius containing that fraction (degrees)
     */
    double containment_radius(double fraction) const {
        if (fraction <= 0) {
            return 0.0;
        }
        if (fraction >= 1.0) {
            return radius_;
        }
        return radius_ * std::sqrt(fraction);
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Disk: center=(" + std::to_string(lon_0_) + ", " + std::to_string(lat_0_) +
               ") deg, radius=" + std::to_string(radius_) + " deg";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "Disk";
    }

    /**
     * @brief Get center longitude
     * @return Longitude in degrees
     */
    double lon_0() const { return lon_0_; }

    /**
     * @brief Get center latitude
     * @return Latitude in degrees
     */
    double lat_0() const { return lat_0_; }

    /**
     * @brief Get disk radius
     * @return Radius in degrees
     */
    double radius() const { return radius_; }

private:
    double lon_0_;     ///< Center longitude (degrees)
    double lat_0_;     ///< Center latitude (degrees)
    double radius_;    ///< Disk radius (degrees)
    double norm_;      ///< Normalization factor
};

/**
 * @brief Shell spatial model
 *
 * Uniform brightness on a spherical shell:
 * PSF(r) = 1 / (4*pi*R^2*width) for |r - R| <= width/2, 0 otherwise
 *
 * Commonly used for:
 * - Shell-type supernova remnants
 * - Bubble structures
 * - Shock fronts
 *
 * Example:
 * @code
 * Shell shell(83.63, 22.01, 1.0, 0.1);  // lon, lat, radius, width (degrees)
 *
 * // On the shell
 * double flux_shell = shell.evaluate(83.63, 23.0);  // Non-zero
 *
 * // At center (inside shell, but not on shell surface)
 * double flux_center = shell.evaluate(83.63, 22.01);  // ≈ 0.0
 * @endcode
 */
class Shell : public SpatialModel {
public:
    /**
     * @brief Construct shell spatial model
     * @param lon_0 Center longitude (degrees)
     * @param lat_0 Center latitude (degrees)
     * @param radius Shell radius (degrees)
     * @param width Shell width (degrees)
     */
    Shell(double lon_0, double lat_0, double radius, double width)
        : lon_0_(lon_0), lat_0_(lat_0), radius_(radius), width_(width) {

        if (radius <= 0) {
            throw std::invalid_argument("Shell: radius must be positive");
        }

        if (width <= 0) {
            throw std::invalid_argument("Shell: width must be positive");
        }

        if (std::abs(lat_0) > 90.0) {
            throw std::invalid_argument("Shell: latitude must be between -90 and 90 degrees");
        }

        // Volume of spherical shell: 4*pi*R^2*width (approximation for small width)
        norm_ = 1.0 / (4.0 * M_PI * radius_ * radius_ * width_);
    }

    /**
     * @brief Evaluate shell at given coordinates
     * @param lon Longitude (degrees)
     * @param lat Latitude (degrees)
     * @return Spatial probability density
     */
    double evaluate(double lon, double lat = 0.0) const override {
        double dr = compute_angular_distance(lon, lat, lon_0_, lat_0_);

        double dist_from_shell = std::abs(dr - radius_);

        if (dist_from_shell <= width_ / 2.0) {
            return norm_;
        }
        return 0.0;
    }

    /**
     * @brief Evaluate shell at multiple coordinates
     * @param lon Array of longitude values (degrees)
     * @param lat Array of latitude values (degrees)
     * @return Array of spatial probability density values
     */
    Eigen::ArrayXd evaluate(const Eigen::ArrayXd& lon,
                            const Eigen::ArrayXd& lat) const override {
        Eigen::ArrayXd result(lon.size());

        for (int i = 0; i < lon.size(); ++i) {
            double dr = compute_angular_distance(lon[i], lat[i], lon_0_, lat_0_);

            double dist_from_shell = std::abs(dr - radius_);

            result[i] = (dist_from_shell <= width_ / 2.0) ? norm_ : 0.0;
        }

        return result;
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Shell: center=(" + std::to_string(lon_0_) + ", " + std::to_string(lat_0_) +
               ") deg, radius=" + std::to_string(radius_) + " deg, width=" +
               std::to_string(width_) + " deg";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "Shell";
    }

    /**
     * @brief Get center longitude
     * @return Longitude in degrees
     */
    double lon_0() const { return lon_0_; }

    /**
     * @brief Get center latitude
     * @return Latitude in degrees
     */
    double lat_0() const { return lat_0_; }

    /**
     * @brief Get shell radius
     * @return Radius in degrees
     */
    double radius() const { return radius_; }

    /**
     * @brief Get shell width
     * @return Width in degrees
     */
    double width() const { return width_; }

private:
    double lon_0_;     ///< Center longitude (degrees)
    double lat_0_;     ///< Center latitude (degrees)
    double radius_;    ///< Shell radius (degrees)
    double width_;     ///< Shell width (degrees)
    double norm_;      ///< Normalization factor
};

// ==================== Convenience Functions ====================

/**
 * @brief Create point source at given coordinates
 * @param lon Source longitude (degrees)
 * @param lat Source latitude (degrees)
 * @return PointSource model
 */
inline PointSource create_point_source(double lon, double lat) {
    return PointSource(lon, lat);
}

/**
 * @brief Create Gaussian model for typical PSF
 * @param lon Center longitude (degrees)
 * @param lat Center latitude (degrees)
 * @param sigma PSF sigma (degrees, default 0.1 deg)
 * @return Gaussian model
 */
inline Gaussian create_psf(double lon, double lat, double sigma = 0.1) {
    return Gaussian(lon, lat, sigma);
}

/**
 * @brief Create extended source model with disk morphology
 * @param lon Center longitude (degrees)
 * @param lat Center latitude (degrees)
 * @param radius Disk radius (degrees)
 * @return Disk model
 */
inline Disk create_extended_source(double lon, double lat, double radius) {
    return Disk(lon, lat, radius);
}

/**
 * @brief Create shell-type source model
 * @param lon Center longitude (degrees)
 * @param lat Center latitude (degrees)
 * @param radius Shell radius (degrees)
 * @param width Shell width (degrees, default 0.1*radius)
 * @return Shell model
 */
inline Shell create_shell_source(double lon, double lat, double radius, double width = 0.0) {
    double actual_width = (width > 0) ? width : 0.1 * radius;
    return Shell(lon, lat, radius, actual_width);
}

} // namespace spatial
} // namespace modeling
} // namespace cxfunc

#endif // CXFUNC_MODELING_SPATIAL_HPP