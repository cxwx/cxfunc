#ifndef CXFUNC_MATH_ROOTS_HPP
#define CXFUNC_MATH_ROOTS_HPP

#include <Eigen/Dense>
#include <functional>
#include <stdexcept>
#include <cmath>
#include <limits>

#include "cxfunc/math/version.hpp"

#ifdef CXFUNC_USE_GSL
#include <gsl/gsl_roots.h>
#endif

namespace cxfunc {
namespace math {
namespace roots {

/**
 * @brief Root finding result structure
 */
struct RootResult {
    double root;              ///< Found root value
    int iterations;          ///< Number of iterations used
    bool converged;          ///< Whether the method converged
    double residual;         ///< Final residual value |f(root)|

    RootResult() : root(0), iterations(0), converged(false), residual(0) {}
};

/**
 * @brief Configuration for root finding algorithms
 */
struct RootConfig {
    int max_iterations = 100;           ///< Maximum iterations
    double tolerance = 1e-10;           ///< Convergence tolerance
    double epsilon = std::numeric_limits<double>::epsilon(); ///< Machine epsilon

    RootConfig() = default;
};

/**
 * @brief Bisection method for root finding
 *
 * Robust but slower method that guarantees convergence if f(a)*f(b) < 0.
 * Simple and reliable for continuous functions.
 *
 * @param f Function to find root of (signature: double(double))
 * @param lower Lower bound of search interval
 * @param upper Upper bound of search interval
 * @param config Algorithm configuration
 * @return RootResult containing the found root and convergence info
 * @throws std::invalid_argument if interval doesn't bracket a root
 *
 * Example:
 * @code
 * auto f = [](double x) { return x * x - 4.0; };
 * auto result = bisection(f, 0.0, 5.0);
 * std::cout << "Root: " << result.root << std::endl;  // Output: 2.0
 * @endcode
 */
template<typename Func>
RootResult bisection(Func&& f, double lower, double upper,
                     const RootConfig& config = RootConfig()) {
    RootResult result;

    // Check if interval brackets a root
    double f_lower = f(lower);
    double f_upper = f(upper);

    if (std::isnan(f_lower) || std::isnan(f_upper)) {
        throw std::invalid_argument("Function returns NaN at interval boundaries");
    }

    if (f_lower * f_upper > 0) {
        throw std::invalid_argument(
            "Interval does not bracket a root: f(lower) * f(upper) > 0");
    }

    // Bisection iteration
    int iter = 0;
    double mid = 0.0;
    double f_mid = 0.0;

    while (iter < config.max_iterations) {
        mid = (lower + upper) / 2.0;
        f_mid = f(mid);

        // Check for convergence
        if (std::abs(f_mid) < config.tolerance ||
            (upper - lower) < config.tolerance) {
            result.root = mid;
            result.iterations = iter;
            result.converged = true;
            result.residual = std::abs(f_mid);
            return result;
        }

        // Update interval
        if (f_lower * f_mid < 0) {
            upper = mid;
            f_upper = f_mid;
        } else {
            lower = mid;
            f_lower = f_mid;
        }

        iter++;
    }

    // Did not converge within max iterations
    result.root = mid;
    result.iterations = iter;
    result.converged = false;
    result.residual = std::abs(f_mid);
    return result;
}

/**
 * @brief Brent's method for root finding
 *
 * Combines bisection, secant, and inverse quadratic interpolation.
 * Faster than bisection while maintaining robustness.
 *
 * @param f Function to find root of
 * @param lower Lower bound of search interval
 * @param upper Upper bound of search interval
 * @param config Algorithm configuration
 * @return RootResult containing the found root and convergence info
 *
 * This is the recommended method for most applications due to its
 * combination of speed and reliability.
 */
template<typename Func>
RootResult brent(Func&& f, double lower, double upper,
                const RootConfig& config = RootConfig()) {
    RootResult result;

    // Check if interval brackets a root
    double f_lower = f(lower);
    double f_upper = f(upper);

    if (std::isnan(f_lower) || std::isnan(f_upper)) {
        throw std::invalid_argument("Function returns NaN at interval boundaries");
    }

    if (f_lower * f_upper > 0) {
        throw std::invalid_argument(
            "Interval does not bracket a root: f(lower) * f(upper) > 0");
    }

    // Ensure f_lower < 0 and f_upper > 0
    if (f_lower > 0) {
        std::swap(lower, upper);
        std::swap(f_lower, f_upper);
    }

    double a = lower;      // Current lower bound
    double b = upper;      // Current upper bound
    double c = a;          // Previous iterate
    double fc = f_lower;   // Function value at c
    double d = b - a;      // Current step size
    double e = d;          // Previous step size

    int iter = 0;
    while (iter < config.max_iterations) {
        // Check for convergence
        if (std::abs(f_upper) < config.tolerance ||
            (upper - lower) < config.tolerance) {
            result.root = b;
            result.iterations = iter;
            result.converged = true;
            result.residual = std::abs(f_upper);
            return result;
        }

        // Choose interpolation method
        if (std::abs(e) >= config.tolerance &&
            std::abs(fc) > std::abs(f_upper)) {

            // Try inverse quadratic interpolation
            double tol = config.tolerance;
            double r = f_upper / fc;
            double p, q;

            if (a == c) {
                // Linear interpolation (secant method)
                p = (b - a) * r;
                q = 1.0 - r;
            } else {
                // Inverse quadratic interpolation
                double q1 = f_lower / fc;
                double q2 = f_upper / fc;
                double q3 = f_lower / f_upper;

                p = q3 * (b - a) * q1 * (q1 - q2);
                q = (q1 - 1.0) * (q2 - 1.0) * (q3 - 1.0);
            }

            if (p > 0) {
                q = -q;
            } else {
                p = -p;
            }

            // Check if IQI step is acceptable
            if (2.0 * p < 3.0 * (b - a) * q - std::abs(tol * q) &&
                p < std::abs(0.5 * e * q)) {
                e = d;
                d = p / q;
            } else {
                // Fall back to bisection
                d = e = 0.5 * (b - a);
            }
        } else {
            // Use bisection
            d = e = 0.5 * (b - a);
        }

        // Update bounds
        if (std::abs(d) > config.tolerance) {
            c = b;
            fc = f_upper;
        }

        a = b;
        f_lower = f_upper;

        if (std::abs(d) > config.tolerance) {
            b += d;
        } else {
            b += (b > a ? config.tolerance : -config.tolerance);
        }

        f_upper = f(b);

        // Update interval to maintain bracketing
        if (f_upper * fc > 0) {
            // Root not bracketed, use bisection fallback
            b = a;
            f_upper = f_lower;
        } else {
            c = a;
            fc = f_lower;
        }

        // Ensure f_lower < 0 and f_upper > 0
        if (f_lower * f_upper > 0 && f_lower > 0) {
            std::swap(a, b);
            std::swap(f_lower, f_upper);
        }

        iter++;
    }

    // Did not converge
    result.root = b;
    result.iterations = iter;
    result.converged = false;
    result.residual = std::abs(f_upper);
    return result;
}

/**
 * @brief Secant method for root finding
 *
 * Fast but less robust method that doesn't require bracketing.
 * Uses derivative approximation to converge quickly.
 *
 * @param f Function to find root of
 * @param x0 Initial guess
 * @param x1 Second initial guess
 * @param config Algorithm configuration
 * @return RootResult containing the found root and convergence info
 *
 * Note: This method can fail if the initial guesses are poor
 * or if the function has horizontal asymptotes.
 */
template<typename Func>
RootResult secant(Func&& f, double x0, double x1,
                 const RootConfig& config = RootConfig()) {
    RootResult result;

    double f_x0 = f(x0);
    double f_x1 = f(x1);

    if (std::abs(f_x1 - f_x0) < config.epsilon) {
        throw std::invalid_argument(
            "Initial guesses too close: |f(x1) - f(x0)| < epsilon");
    }

    int iter = 0;
    double x_new = 0.0;
    double f_new = 0.0;

    while (iter < config.max_iterations) {
        // Secant formula
        x_new = x1 - f_x1 * (x1 - x0) / (f_x1 - f_x0);

        // Check for convergence
        f_new = f(x_new);
        if (std::abs(f_new) < config.tolerance) {
            result.root = x_new;
            result.iterations = iter;
            result.converged = true;
            result.residual = std::abs(f_new);
            return result;
        }

        // Update points
        x0 = x1;
        f_x0 = f_x1;
        x1 = x_new;
        f_x1 = f_new;

        // Check for divergence
        if (!std::isfinite(x1) || !std::isfinite(f_x1)) {
            result.root = x1;
            result.iterations = iter;
            result.converged = false;
            result.residual = std::abs(f_x1);
            return result;
        }

        iter++;
    }

    // Did not converge
    result.root = x_new;
    result.iterations = iter;
    result.converged = false;
    result.residual = std::abs(f_new);
    return result;
}

/**
 * @brief Convenience function that automatically chooses method
 *
 * Uses Brent's method if interval brackets a root, otherwise falls back
 * to secant method with bisection as ultimate fallback.
 *
 * @param f Function to find root of
 * @param lower Lower bound or initial guess
 * @param upper Upper bound or second guess
 * @param config Algorithm configuration
 * @return RootResult containing the found root and convergence info
 */
template<typename Func>
RootResult find_root(Func&& f, double lower, double upper,
                   const RootConfig& config = RootConfig()) {
    try {
        // Try Brent's method first (requires bracketing)
        return brent(std::forward<Func>(f), lower, upper, config);
    } catch (const std::invalid_argument&) {
        // Fall back to secant method if no bracketing
        return secant(std::forward<Func>(f), lower, upper, config);
    }
}

} // namespace roots
} // namespace math
} // namespace cxfunc

#endif // CXFUNC_MATH_ROOTS_HPP