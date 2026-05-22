#ifndef CXFUNC_MATH_GAUSS_HPP
#define CXFUNC_MATH_GAUSS_HPP

#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include <algorithm>
#include <stdexcept>

#include "cxfunc/math/version.hpp"
#include "cxfunc/math/roots.hpp"

namespace cxfunc {
namespace math {
namespace gauss {

/**
 * @brief 2D symmetric Gaussian PDF
 *
 * Represents a 2D Gaussian probability density function used for
 * Point Spread Function (PSF) modeling in gamma-ray astronomy.
 *
 * Mathematical form:
 * P(x,y) = (1 / (2πσ²)) * exp(-(x² + y²) / (2σ²))
 *
 * References:
 * - http://en.wikipedia.org/wiki/Multivariate_normal_distribution#Bivariate_case
 *
 * Example:
 * @code
 * gauss::Gauss2DPDF gauss(0.5);  // sigma = 0.5 deg
 *
 * // Calculate containment radius for 68% confidence
 * double radius = gauss.containment_radius(0.68);
 *
 * // Evaluate PDF at position (1.0, 0.5)
 * double value = gauss(1.0, 0.5);
 *
 * // Convolve with another Gaussian
 * auto convolved = gauss.convolve(0.3);  // convolve with sigma=0.3
 * @endcode
 */
class Gauss2DPDF {
public:
    /**
     * @brief Construct 2D Gaussian PDF
     * @param sigma Gaussian width (same in both dimensions)
     */
    explicit Gauss2DPDF(double sigma = 1.0) : sigma_(sigma) {
        if (sigma <= 0) {
            throw std::invalid_argument("Gauss2DPDF: sigma must be positive");
        }
    }

    /**
     * @brief Evaluate PDF at position (x, y)
     * @param x x-coordinate
     * @param y y-coordinate
     * @return PDF value at (x, y)
     */
    double operator()(double x, double y = 0.0) const {
        double theta2 = x * x + y * y;
        double amplitude = 1.0 / (2.0 * M_PI * sigma2());
        double exponent = -0.5 * theta2 / sigma2();
        return amplitude * std::exp(exponent);
    }

    /**
     * @brief Evaluate PDF derivative with respect to theta²
     * @param theta2 Squared offset angle (θ² = x² + y²)
     * @return dP/dθ² at theta2
     */
    double dpdtheta2(double theta2) const {
        double amplitude = 1.0 / (2.0 * M_PI * sigma2());
        double exponent = -0.5 * theta2 / sigma2();
        return amplitude * std::exp(exponent);
    }

    /**
     * @brief Calculate containment fraction
     * @param rad Radius from center
     * @return Fraction of total probability contained within radius
     *
     * Formula: 1 - exp(-r² / (2σ²))
     */
    double containment_fraction(double rad) const {
        return 1.0 - std::exp(-0.5 * rad * rad / sigma2());
    }

    /**
     * @brief Calculate containment radius for a given fraction
     * @param fraction Desired containment fraction (0 < fraction < 1)
     * @return Radius containing specified fraction of probability
     *
     * Formula: σ * sqrt(-2 * ln(1 - fraction))
     */
    double containment_radius(double fraction) const {
        if (fraction <= 0 || fraction >= 1) {
            throw std::invalid_argument(
                "Gauss2DPDF: fraction must be in (0, 1)");
        }
        return sigma_ * std::sqrt(-2.0 * std::log(1.0 - fraction));
    }

    /**
     * @brief Convolve with another Gaussian
     * @param sigma Width of Gaussian to convolve with
     * @return New Gauss2DPDF representing the convolution
     *
     * The result has width: σ_new = sqrt(σ₁² + σ₂²)
     */
    Gauss2DPDF convolve(double sigma) const {
        double new_sigma = std::sqrt(sigma2() + sigma * sigma);
        return Gauss2DPDF(new_sigma);
    }

    /**
     * @brief Get Gaussian width
     * @return sigma value
     */
    double sigma() const { return sigma_; }

    /**
     * @brief Get sigma squared
     * @return sigma² value
     */
    double sigma2() const { return sigma_ * sigma_; }

    /**
     * @brief Get amplitude at center
     * @return PDF value at (0, 0)
     */
    double amplitude() const { return this->operator()(0, 0); }

private:
    double sigma_;
};

/**
 * @brief Sum of multiple 2D Gaussians
 *
 * Represents a multi-component Gaussian PDF, commonly used for
 * modeling complex PSFs with multiple components (e.g., core + tail).
 *
 * Note: This is NOT a normalized PDF - the sum of components is
 * not guaranteed to integrate to 1. The "norm" parameter represents
 * the 2D integral of each component, not the amplitude at origin.
 *
 * Example:
 * @code
 * std::vector<double> sigmas = {0.1, 0.3, 1.0};  // core, halo, tail
 * std::vector<double> norms = {0.7, 0.2, 0.1};   // relative weights
 *
 * gauss::MultiGauss2D multi(sigmas, norms);
 *
 * // Get effective sigma
 * double eff_sigma = multi.eff_sigma();
 *
 * // Calculate containment fraction
 * double fraction = multi.containment_fraction(0.5);
 * @endcode
 */
class MultiGauss2D {
public:
    /**
     * @brief Construct multi-Gaussian PDF
     * @param sigmas Widths of Gaussian components
     * @param norms Normalizations of components (2D integrals)
     *
     * If norms is empty, all components are assumed to have unit norm.
     */
    MultiGauss2D(const std::vector<double>& sigmas,
                const std::vector<double>& norms = {})
        : norms_(norms) {

        if (sigmas.empty()) {
            throw std::invalid_argument(
                "MultiGauss2D: sigmas vector cannot be empty");
        }

        // Create Gaussian components
        for (auto sigma : sigmas) {
            components_.emplace_back(sigma);
        }

        // If no norms provided, use unit norms
        if (norms_.empty()) {
            norms_.resize(components_.size(), 1.0);
        }

        // Validate dimensions
        if (norms_.size() != components_.size()) {
            throw std::invalid_argument(
                "MultiGauss2D: sigmas and norms must have same size");
        }
    }

    /**
     * @brief Evaluate multi-Gaussian PDF at position (x, y)
     * @param x x-coordinate
     * @param y y-coordinate
     * @return Weighted sum of component PDFs at (x, y)
     */
    double operator()(double x, double y = 0.0) const {
        double total = 0.0;
        for (size_t i = 0; i < components_.size(); ++i) {
            total += norms_[i] * components_[i](x, y);
        }
        return total;
    }

    /**
     * @brief Evaluate derivative with respect to theta²
     * @param theta2 Squared offset angle
     * @return Weighted sum of component derivatives
     */
    double dpdtheta2(double theta2) const {
        double total = 0.0;
        for (size_t i = 0; i < components_.size(); ++i) {
            total += norms_[i] * components_[i].dpdtheta2(theta2);
        }
        return total;
    }

    /**
     * @brief Calculate containment fraction
     * @param rad Radius from center
     * @return Fraction of total probability contained within radius
     *
     * Formula: sum_i (norm_i * containment_fraction_i(rad))
     */
    double containment_fraction(double rad) const {
        double total = 0.0;
        for (size_t i = 0; i < components_.size(); ++i) {
            total += norms_[i] * components_[i].containment_fraction(rad);
        }
        return total;
    }

    /**
     * @brief Calculate containment radius for a given fraction
     * @param fraction Desired containment fraction (0 < fraction < 1)
     * @return Radius containing specified fraction of probability
     *
     * Uses numerical root finding to solve for radius.
     */
    double containment_radius(double fraction) const {
        if (fraction <= 0 || fraction >= 1) {
            throw std::invalid_argument(
                "MultiGauss2D: fraction must be in (0, 1)");
        }

        // Find upper bound
        double rad_max = 1e3;
        while (containment_fraction(rad_max) < fraction && rad_max < 1e9) {
            rad_max *= 10;
        }

        if (containment_fraction(rad_max) < fraction) {
            throw std::runtime_error(
                "MultiGauss2D: cannot achieve desired containment fraction");
        }

        // Root finding: find radius where containment_fraction(radius) = fraction
        auto f = [this, fraction](double rad) {
            return this->containment_fraction(rad) - fraction;
        };

        // Use bisection method for robustness
        auto result = roots::bisection(f, 0.0, rad_max);

        if (!result.converged) {
            throw std::runtime_error(
                "MultiGauss2D: failed to converge on containment radius");
        }

        return result.root;
    }

    /**
     * @brief Convolve with another Gaussian
     * @param sigma Width of Gaussian to convolve with
     * @param norm Normalization of convolving Gaussian
     * @return New MultiGauss2D representing the convolution
     *
     * Each component is convolved separately, producing a new
     * multi-Gaussian with the same number of components.
     */
    MultiGauss2D convolve(double sigma, double norm = 1.0) const {
        std::vector<double> new_sigmas;
        std::vector<double> new_norms;

        for (size_t i = 0; i < n_components(); ++i) {
            auto convolved = components_[i].convolve(sigma);
            new_sigmas.push_back(convolved.sigma());
            new_norms.push_back(norms_[i] * norm);
        }

        return MultiGauss2D(new_sigmas, new_norms);
    }

    /**
     * @brief Get number of components
     * @return Number of Gaussian components
     */
    size_t n_components() const { return components_.size(); }

    /**
     * @brief Get component widths
     * @return Vector of sigma values for each component
     */
    std::vector<double> sigmas() const {
        std::vector<double> result;
        for (const auto& component : components_) {
            result.push_back(component.sigma());
        }
        return result;
    }

    /**
     * @brief Get largest sigma (widest component)
     * @return Maximum sigma value
     */
    double max_sigma() const {
        auto sigma_vec = sigmas();
        return *std::max_element(sigma_vec.begin(), sigma_vec.end());
    }

    /**
     * @brief Calculate effective Gaussian width
     * @return Effective sigma for single-Gaussian approximation
     *
     * Formula: σ_eff = sqrt(Σ(N_i * σ_i²))
     *
     * This provides a single-Gaussian approximation that matches
     * the second moment of the multi-Gaussian distribution.
     */
    double eff_sigma() const {
        double sum = 0.0;
        for (size_t i = 0; i < components_.size(); ++i) {
            sum += norms_[i] * components_[i].sigma2();
        }
        return std::sqrt(sum);
    }

    /**
     * @brief Get total integral
     * @return Sum of all component normalizations
     */
    double total_integral() const {
        double sum = 0.0;
        for (auto norm : norms_) {
            sum += norm;
        }
        return sum;
    }

    /**
     * @brief Get amplitude at center
     * @return PDF value at (0, 0)
     */
    double amplitude() const { return this->operator()(0, 0); }

    /**
     * @brief Normalize the multi-Gaussian
     *
     * Scales norms so that total_integral() = 1.0
     */
    void normalize() {
        double total = total_integral();
        if (total > 0) {
            for (auto& norm : norms_) {
                norm /= total;
            }
        }
    }

private:
    std::vector<Gauss2DPDF> components_;
    std::vector<double> norms_;
};

} // namespace gauss
} // namespace math
} // namespace cxfunc

#endif // CXFUNC_MATH_GAUSS_HPP