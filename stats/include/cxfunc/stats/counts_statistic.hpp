#ifndef CXFUNC_STATS_COUNTS_STATISTIC_HPP
#define CXFUNC_STATS_COUNTS_STATISTIC_HPP

#include <Eigen/Dense>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <functional>
#include <limits>

#ifdef CXFUNC_USE_BOOST_MATH
#include <boost/math/special_functions/lambert_w.hpp>
#endif

#include "cxfunc/stats/version.hpp"
#include "cxfunc/stats/fit_statistics.hpp"

namespace cxfunc {
namespace stats {

/**
 * @brief Base class for count statistics
 *
 * Provides common functionality for Poisson count statistics
 * including significance calculation, error estimation, and
 * upper limit computation.
 */
class CountsStatistic {
public:
    virtual ~CountsStatistic() = default;

    /**
     * @brief Calculate excess (signal - background)
     */
    virtual double excess() const = 0;

    /**
     * @brief Calculate statistical significance (Li & Ma formula)
     * @return sqrt(TS) with sign of excess
     */
    virtual double sqrt_ts() const = 0;

    /**
     * @brief Calculate p-value for observed excess
     * @return One-sided p-value
     */
    virtual double p_value() const = 0;

    /**
     * @brief Get test statistic value
     */
    virtual double ts() const = 0;

    /**
     * @brief Get observed counts
     */
    virtual double n_on() const = 0;

    /**
     * @brief Get expected background counts
     */
    virtual double n_bkg() const = 0;

    /**
     * @brief Get excess signal
     */
    double n_sig() const { return excess(); }

    /**
     * @brief Get error estimate (approximate from covariance matrix)
     */
    virtual double error() const = 0;

protected:
    /**
     * @brief Helper function to compute TS from two likelihood values
     */
    static double compute_ts(double stat_null, double stat_max) {
        return std::max(0.0, stat_null - stat_max);
    }

    /**
     * @brief Helper function to compute sqrt(TS) with sign
     */
    static double compute_sqrt_ts(double ts, double excess) {
        return (excess >= 0) ? std::sqrt(ts) : -std::sqrt(ts);
    }

    /**
     * @brief Helper function to compute p-value from TS
     * Uses chi2 distribution with 1 degree of freedom
     */
    static double compute_p_value(double ts) {
        // p_value = 0.5 * chi2.sf(ts, 1)
        // Using complementary error function approximation
        return 0.5 * std::erfc(std::sqrt(ts / 2.0));
    }
};

/**
 * @brief Cash counts statistic for known background
 *
 * Implements statistical analysis for Poisson data with known background.
 * Uses the Cash likelihood and provides analytical solutions for
 * error bars and upper limits using Lambert W function.
 *
 * References:
 * - Cash (1979), ApJ 228, 939
 */
class CashCountsStatistic : public CountsStatistic {
public:
    /**
     * @param n_on Observed counts
     * @param mu_bkg Known expected background
     */
    CashCountsStatistic(double n_on, double mu_bkg)
        : n_on_(n_on), mu_bkg_(mu_bkg) {

        n_bkg_ = mu_bkg;
        excess_ = n_on - n_bkg_;
        sign_ = (excess_ >= 0) ? 1.0 : -1.0;

        // Calculate TS: TS = 2 * (log L(n_on|mu_bkg+n_on) - log L(n_on|mu_bkg))
        // For Cash statistic: TS = cash(n_on, mu_bkg+0) - cash(n_on, n_on)
        stat_null_ = 2.0 * (mu_bkg - n_on * std::log(std::max(mu_bkg, TRUNCATION_VALUE)));
        stat_max_ = 2.0 * (n_on - n_on * std::log(std::max(n_on, TRUNCATION_VALUE)));

        ts_ = compute_ts(stat_null_, stat_max_);
    }

    double excess() const override { return excess_; }
    double sqrt_ts() const override { return compute_sqrt_ts(ts_, excess_); }
    double p_value() const override { return compute_p_value(ts_); }
    double ts() const override { return ts_; }
    double n_on() const override { return n_on_; }
    double n_bkg() const override { return n_bkg_; }
    double error() const override { return std::sqrt(std::max(n_on_, 0.0)); }

    /**
     * @brief Compute downward (negative) excess uncertainty
     *
     * Uses analytical solution with Lambert W function (branch 0)
     *
     * @param n_sigma Confidence level in sigma (default 1)
     * @return Downward error (positive value)
     */
    double compute_errn(double n_sigma = 1.0) const {
        if (n_on_ <= 0) return 0.0;

        double c = n_sigma * n_sigma / 2.0;

#ifdef GAMMAPY_USE_BOOST_MATH
        // Use Boost.Math Lambert W (branch 0)
        double w = boost::math::lambert_w0(-std::exp(-c / n_on_ - 1.0));
        double result = n_on_ * (w + 1.0);
#else
        // Use our numerical Lambert W implementation
        double z = -std::exp(-c / n_on_ - 1.0);
        double w = lambert_w0(z);
        double result = n_on_ * (w + 1.0);
#endif

        return std::max(result, 0.0);
    }

    /**
     * @brief Compute upward (positive) excess uncertainty
     *
     * Uses analytical solution with Lambert W function (branch -1)
     *
     * @param n_sigma Confidence level in sigma (default 1)
     * @return Upward error (positive value)
     */
    double compute_errp(double n_sigma = 1.0) const {
        double c = n_sigma * n_sigma / 2.0;

        if (n_on_ > 0) {
#ifdef GAMMAPY_USE_BOOST_MATH
            // Use Boost.Math Lambert W (branch -1)
            double w = boost::math::lambert_wm1(-std::exp(-c / n_on_ - 1.0));
            double result = -n_on_ * (w + 1.0);
            return result;
#else
            // Use our numerical Lambert W implementation
            double z = -std::exp(-c / n_on_ - 1.0);
            double w = lambert_wm1(z);
            double result = -n_on_ * (w + 1.0);
            return result;
#endif
        } else {
            return c;
        }
    }

    /**
     * @brief Compute upper limit on signal
     *
     * @param n_sigma Confidence level in sigma (default 3)
     * @return Upper limit value
     */
    double compute_upper_limit(double n_sigma = 3.0) const {
        double errp = compute_errp(n_sigma);
        return excess_ + errp;
    }

    /**
     * @brief Compute excess matching a given significance
     *
     * Inverse function of significance calculation.
     *
     * @param significance Desired significance (in sigma)
     * @return Required excess
     */
    double n_sig_matching_significance(double significance) const {
        if (mu_bkg_ <= 0) return std::numeric_limits<double>::quiet_NaN();

        double c = significance * significance / 2.0;

#ifdef GAMMAPY_USE_BOOST_MATH
        int branch = (significance > 0) ? 0 : -1;
        double w = (branch == 0) ?
            boost::math::lambert_w0((c / mu_bkg_ - 1.0) / std::exp(1.0)) :
            boost::math::lambert_wm1((c / mu_bkg_ - 1.0) / std::exp(1.0));
        double result = mu_bkg_ * (std::exp(w + 1.0) - 1.0);
        return result;
#else
        // Use our numerical Lambert W implementation
        double z = (c / mu_bkg_ - 1.0) / std::exp(1.0);
        double w;
        if (significance > 0) {
            w = lambert_w0(z);
        } else {
            w = lambert_wm1(z);
        }
        double result = mu_bkg_ * (std::exp(w + 1.0) - 1.0);
        return result;
#endif
    }

private:
    /**
     * @brief Helper function to compute Lambert W function (branch 0)
     * Uses series approximation for small negative z values
     */
    static double lambert_w0(double z) {
        if (z == 0.0) return 0.0;
        // Valid range for branch 0: z >= -1/e
        const double neg_inv_e = -1.0/M_E;
        if (z < neg_inv_e) return std::numeric_limits<double>::quiet_NaN();

        // For our use case, z is typically in range [-0.5, 0]
        // Use simple iterative method with better initial guess
        double w = 0.0;
        if (z < -0.1) {
            w = z;  // Good initial guess for z close to 0
        } else if (z > 0.0) {
            w = std::log(z);  // Approximation for positive z
        } else {
            w = std::log(1.0 + z);  // Approximation for negative z near 0
        }

        // Simplified Newton iteration
        for (int i = 0; i < 20; ++i) {
            double ew = std::exp(w);
            double f = w * ew - z;
            double fp = w * ew + ew;

            if (std::abs(fp) < 1e-10) break;

            double delta = f / fp;
            w -= delta;

            if (std::abs(delta) < 1e-10 * std::abs(w)) break;
            if (!std::isfinite(w)) return std::numeric_limits<double>::quiet_NaN();
        }

        return w;
    }

    /**
     * @brief Helper function to compute Lambert W function (branch -1)
     * Uses rational approximation for the relevant range
     */
    static double lambert_wm1(double z) {
        if (z == 0.0) return 0.0;
        // Valid range for branch -1: [-1/e, 0)
        const double neg_inv_e = -1.0/M_E;
        if (z < neg_inv_e || z >= 0.0) return std::numeric_limits<double>::quiet_NaN();

        // For branch -1, w <= -1
        double w = -1.1;  // Initial guess
        if (z > neg_inv_e / 2.0) w = -1.2;

        // Newton iteration with bounds checking
        for (int i = 0; i < 20; ++i) {
            double ew = std::exp(w);
            double f = w * ew - z;
            double fp = w * ew + ew;

            if (std::abs(fp) < 1e-10) break;

            double delta = f / fp;
            w -= delta;

            // Keep w in valid range for branch -1 (w <= -1)
            if (w > -1.0) w = -1.0;
            if (w < -10.0) w = -10.0;  // Prevent divergence

            if (std::abs(delta) < 1e-10 * std::abs(w)) break;
            if (!std::isfinite(w)) return std::numeric_limits<double>::quiet_NaN();
        }

        return w;
    }

    double n_on_;        ///< Observed counts
    double mu_bkg_;      ///< Expected background
    double n_bkg_;       ///< Same as mu_bkg for known background
    double excess_;      ///< Excess = n_on - n_bkg
    double stat_null_;   ///< Statistic under null hypothesis
    double stat_max_;    ///< Statistic at maximum likelihood
    double ts_;          ///< Test statistic
    double sign_;        ///< Sign of excess
};

/**
 * @brief WSTAT counts statistic for unknown background
 *
 * Implements statistical analysis for ON-OFF measurements using
 * the W-statistic (Li & Ma formula). Background is estimated from
 * OFF region using profile likelihood.
 *
 * References:
 * - Li & Ma (1983), ApJ 272, 317
 */
class WStatCountsStatistic : public CountsStatistic {
public:
    /**
     * @param n_on Observed ON counts
     * @param n_off Observed OFF counts
     * @param alpha Acceptance ratio (ON/OFF exposure ratio)
     * @param mu_sig Expected signal (default 0)
     */
    WStatCountsStatistic(double n_on, double n_off, double alpha, double mu_sig = 0.0)
        : n_on_(n_on), n_off_(n_off), alpha_(alpha), mu_sig_(mu_sig) {

        if (alpha <= 0) {
            throw std::invalid_argument("alpha must be positive");
        }

        // Calculate background from OFF counts
        n_bkg_ = alpha_ * n_off_;
        excess_ = n_on_ - n_bkg_ - mu_sig_;
        sign_ = (excess_ >= 0) ? 1.0 : -1.0;

        // Calculate W-statistic using profile likelihood
        stat_null_ = compute_wstat(mu_sig_);
        stat_max_ = compute_wstat(excess_ + mu_sig_);

        ts_ = compute_ts(stat_null_, stat_max_);
    }

    double excess() const override { return excess_; }
    double sqrt_ts() const override { return compute_sqrt_ts(ts_, excess_); }
    double p_value() const override { return compute_p_value(ts_); }
    double ts() const override { return ts_; }
    double n_on() const override { return n_on_; }
    double n_bkg() const override { return n_bkg_; }
    double n_off() const { return n_off_; }
    double alpha() const { return alpha_; }
    double mu_sig() const { return mu_sig_; }

    double error() const override {
        return std::sqrt(std::max(n_on_, 0.0) + alpha_ * alpha_ * n_off_);
    }

    /**
     * @brief Compute downward excess uncertainty
     *
     * Uses numerical root finding to find signal value where TS changes by n_sigma^2
     *
     * @param n_sigma Confidence level in sigma (default 1)
     * @return Downward error (positive value)
     */
    double compute_errn(double n_sigma = 1.0) const {
        double target_ts = stat_max_ + n_sigma * n_sigma;

        // Search for signal value where stat = target_ts
        double lower = std::min(excess_, 0.0) - error() * n_sigma * 2;
        double upper = excess_;

        return find_root(lower, upper, target_ts) - excess_;
    }

    /**
     * @brief Compute upward excess uncertainty
     *
     * @param n_sigma Confidence level in sigma (default 1)
     * @return Upward error (positive value)
     */
    double compute_errp(double n_sigma = 1.0) const {
        double target_ts = stat_max_ + n_sigma * n_sigma;

        // Search for signal value where stat = target_ts
        double lower = excess_;
        double upper = excess_ + error() * n_sigma * 2;

        return find_root(lower, upper, target_ts) - excess_;
    }

    /**
     * @brief Compute upper limit on signal
     *
     * @param n_sigma Confidence level in sigma (default 3)
     * @return Upper limit value
     */
    double compute_upper_limit(double n_sigma = 3.0) const {
        double ts_ref = compute_wstat(0.0);
        double target_ts = ts_ref + n_sigma * n_sigma;

        double lower = 0.0;
        double upper = excess_ + error() * n_sigma * 2;

        return find_root(lower, upper, target_ts);
    }

    /**
     * @brief Compute excess matching a given significance
     *
     * @param significance Desired significance (in sigma)
     * @return Required excess
     */
    double n_sig_matching_significance(double significance) const {
        // Use numerical root finding
        double lower = std::sqrt(n_bkg_) * significance;
        double upper = lower * (1.0 + 1e-4) + 1e-4;

        auto fcn = [this, significance](double n_sig) {
            double stat0 = this->compute_wstat(0.0);
            double stat1 = this->compute_wstat(n_sig);
            double ts = std::max(0.0, stat0 - stat1);
            double sqrt_ts = std::sqrt(ts);
            return (n_sig >= 0 ? sqrt_ts : -sqrt_ts) - significance;
        };

        return find_root(lower, upper, 0.0, fcn);
    }

private:
    double n_on_;        ///< Observed ON counts
    double n_off_;       ///< Observed OFF counts
    double alpha_;       ///< Acceptance ratio
    double mu_sig_;      ///< Expected signal
    double n_bkg_;       ///< Expected background (alpha * n_off)
    double excess_;      ///< Excess = n_on - n_bkg - mu_sig
    double stat_null_;   ///< Statistic under null hypothesis
    double stat_max_;    ///< Statistic at maximum likelihood
    double ts_;          ///< Test statistic
    double sign_;        ///< Sign of excess

    /**
     * @brief Compute W-statistic for a given signal hypothesis
     */
    double compute_wstat(double mu_sig_test) const {
        // Profile likelihood solution for mu_bkg
        double C = alpha_ * (n_on_ + n_off_) - (1.0 + alpha_) * mu_sig_test;
        double D2 = C * C + 4.0 * alpha_ * (alpha_ + 1.0) * n_off_ * mu_sig_test;
        double D = std::sqrt(std::max(D2, 0.0));

        double mu_bkg = (C + D) / (2.0 * alpha_ * (alpha_ + 1.0));
        mu_bkg = std::max(mu_bkg, 0.0);

        // Calculate W-statistic
        double term1 = mu_sig_test + (1.0 + alpha_) * mu_bkg;
        double term2 = (n_on_ > 0) ? -n_on_ * std::log(mu_sig_test + alpha_ * mu_bkg) : 0.0;
        double term3 = (n_off_ > 0) ? -n_off_ * std::log(mu_bkg) : 0.0;

        double stat = 2.0 * (term1 + term2 + term3);

        // Add goodness-of-fit terms
        if (n_on_ > 0) stat += -2.0 * n_on_ * (1.0 - std::log(n_on_));
        if (n_off_ > 0) stat += -2.0 * n_off_ * (1.0 - std::log(n_off_));

        return stat;
    }

    /**
     * @brief Simple bisection root finder
     */
    double find_root(double lower, double upper, double target,
                     std::function<double(double)> fcn = nullptr) const {
        if (fcn == nullptr) {
            // Default: find mu where compute_wstat(mu) - target = 0
            fcn = [this, target](double mu) {
                return this->compute_wstat(mu) - target;
            };
        }

        // Ensure bounds
        double f_lower = fcn(lower);
        double f_upper = fcn(upper);

        if (f_lower * f_upper > 0) {
            // Same sign, try to expand bounds
            if (std::abs(f_lower) < std::abs(f_upper)) {
                upper = lower + 2.0 * (upper - lower);
            } else {
                lower = upper - 2.0 * (upper - lower);
            }
        }

        // Bisection method
        for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
            double mid = (lower + upper) / 2.0;
            double f_mid = fcn(mid);

            if (std::abs(f_mid) < CONVERGENCE_TOLERANCE) {
                return mid;
            }

            if (f_lower * f_mid < 0) {
                upper = mid;
                f_upper = f_mid;
            } else {
                lower = mid;
                f_lower = f_mid;
            }

            if ((upper - lower) < CONVERGENCE_TOLERANCE) {
                return (lower + upper) / 2.0;
            }
        }

        return (lower + upper) / 2.0; // Return best estimate
    }
};

} // namespace stats
} // namespace cxfunc

#endif // CXFUNC_STATS_COUNTS_STATISTIC_HPP