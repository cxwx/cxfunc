#ifndef CXFUNC_STATS_UTILS_HPP
#define CXFUNC_STATS_UTILS_HPP

#include <cmath>
#include <algorithm>

#ifdef CXFUNC_USE_BOOST_MATH
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#endif

#include "cxfunc/stats/version.hpp"

namespace cxfunc {
namespace stats {

/**
 * @brief Convert number of sigma to test statistic (TS) value
 *
 * This function implements the conversion between significance (in sigma)
 * and test statistic values, accounting for the non-centrality parameter
 * in the Asimov dataset.
 *
 * @param n_sigma Significance in number of sigma
 * @param df Number of degrees of freedom (default 1)
 * @param n_sigma_asimov Significance in Asimov dataset (default 0)
 * @return Test statistic value
 *
 * References:
 * - Wilks theorem: https://en.wikipedia.org/wiki/Wilks%27_theorem
 * - Cowan et al. (2011), European Physical Journal C, 71, 1554
 */
inline double sigma_to_ts(double n_sigma, int df = 1, double n_sigma_asimov = 0.0) {
    double ts_asimov = n_sigma_asimov * n_sigma_asimov;

#ifdef GAMMAPY_USE_BOOST_MATH
    // Use explicit template parameters for newer Boost versions
    using default_policy = boost::math::policies::policy<>;

    boost::math::non_central_chi_squared_distribution<double, default_policy> dist_nc(1.0, ts_asimov);
    boost::math::chi_squared_distribution<double, default_policy> dist_null(1.0);

    double p_value = 1.0 - boost::math::cdf(dist_nc, n_sigma * n_sigma);

    // Convert p-value back to TS with non-centrality
    boost::math::non_central_chi_squared_distribution<double, default_policy> dist_target(df, ts_asimov);
    double ts = boost::math::quantile(boost::math::complement(dist_target, p_value));

    return ts;
#else
    // Fallback: Asimov approximation (Wald test)
    // TS = n_sigma^2 when ts_asimov = 0
    // With non-centrality: TS ≈ (n_sigma + sqrt(ts_asimov))^2 - ts_asimov
    if (ts_asimov > 0) {
        return (n_sigma + std::sqrt(ts_asimov)) * (n_sigma + std::sqrt(ts_asimov)) - ts_asimov;
    } else {
        return n_sigma * n_sigma;
    }
#endif
}

/**
 * @brief Convert test statistic (TS) value to number of sigma
 *
 * This is the inverse function of sigma_to_ts.
 *
 * @param ts Test statistic value
 * @param df Number of degrees of freedom (default 1)
 * @param ts_asimov TS value in Asimov dataset (default 0)
 * @return Significance in number of sigma
 */
inline double ts_to_sigma(double ts, int df = 1, double ts_asimov = 0.0) {
#ifdef GAMMAPY_USE_BOOST_MATH
    using default_policy = boost::math::policies::policy<>;
    boost::math::non_central_chi_squared_distribution<double, default_policy> dist(df, ts_asimov);
    double p_value = 1.0 - boost::math::cdf(dist, ts);

    boost::math::non_central_chi_squared_distribution<double, default_policy> dist_sigma(1.0, ts_asimov);
    double chi2_sigma = boost::math::quantile(boost::math::complement(dist_sigma, p_value));

    return std::sqrt(chi2_sigma);
#else
    // Fallback: simple square root approximation
    if (ts_asimov > 0) {
        return std::sqrt(std::max(ts + ts_asimov, 0.0)) - std::sqrt(ts_asimov);
    } else {
        return std::sqrt(std::max(ts, 0.0));
    }
#endif
}

/**
 * @brief Regularized lower incomplete gamma function P(a, x)
 *
 * P(a, x) = γ(a, x) / Γ(a)
 *
 * This function is used in various statistical calculations including
 * chi-square and gamma distributions.
 *
 * @param a Shape parameter
 * @param x Input value
 * @return Regularized lower incomplete gamma function value
 */
inline double regularized_gamma_p(double a, double x) {
    if (x < 0) return 0.0;
    if (x == 0) return 0.0;

#ifdef GAMMAPY_USE_BOOST_MATH
    namespace bm = boost::math;
    return bm::gamma_p(a, x);
#else
    // Fallback: Series expansion implementation
    if (x < a + 1) {
        // Series expansion for small x
        double sum = 1.0 / a;
        double term = sum;
        for (int n = 1; n < 200; ++n) {
            term *= x / (a + n);
            sum += term;
            if (std::abs(term) < 1e-15 * std::abs(sum)) break;
        }
        return sum * std::exp(-x + a * std::log(x) - std::lgamma(a));
    } else {
        // Continued fraction for large x (Lentz's method)
        double f = std::exp(-x + a * std::log(x) - std::lgamma(a));
        double C = f, D = 0.0;

        for (int n = 1; n <= 200; ++n) {
            double an = n * (1.0 - a);
            double bn = (2.0 * n + 1.0 - a);

            D = bn + an * D;
            if (std::abs(D) < 1e-30) D = 1e-30;
            C = bn + an / C;
            if (std::abs(C) < 1e-30) C = 1e-30;

            D = 1.0 / D;
            double delta = C * D;
            f *= delta;

            if (std::abs(delta - 1.0) < 1e-15) break;
        }
        return f;
    }
#endif
}

/**
 * @brief Chi-square cumulative distribution function
 *
 * @param x Input value
 * @param df Degrees of freedom
 * @return CDF value
 */
inline double chi2_cdf(double x, double df) {
    if (x <= 0) return 0.0;
    return regularized_gamma_p(df / 2.0, x / 2.0);
}

/**
 * @brief Non-central chi-square survival function
 *
 * @param x Input value
 * @param df Degrees of freedom
 * @param nc Non-centrality parameter
 * @return Survival function (1 - CDF) value
 */
inline double ncx2_sf(double x, double df, double nc) {
    if (x <= 0) return 1.0;

#ifdef GAMMAPY_USE_BOOST_MATH
    using default_policy = boost::math::policies::policy<>;
    boost::math::non_central_chi_squared_distribution<double, default_policy> dist(df, nc);
    return 1.0 - boost::math::cdf(dist, x);
#else
    // Fallback: Poisson-weighted sum of central chi2 distributions
    double result = 0.0;
    for (int j = 0; j < 200; ++j) {
        double poisson = std::exp(-nc / 2.0) * std::pow(nc / 2.0, j) / std::tgamma(j + 1.0);
        result += poisson * chi2_cdf(x, df + 2.0 * j);
        if (poisson < 1e-15) break;
    }
    return 1.0 - result;
#endif
}

/**
 * @brief Inverse non-central chi-square survival function
 *
 * @param p Probability value
 * @param df Degrees of freedom
 * @param nc Non-centrality parameter
 * @return Quantile value
 */
inline double ncx2_isf(double p, double df, double nc) {
    if (p <= 0) return 0.0;
    if (p >= 1) return std::numeric_limits<double>::infinity();

#ifdef GAMMAPY_USE_BOOST_MATH
    using default_policy = boost::math::policies::policy<>;
    boost::math::non_central_chi_squared_distribution<double, default_policy> dist(df, nc);
    return boost::math::quantile(boost::math::complement(dist, p));
#else
    // Fallback: binary search
    double lo = 0.0;
    double hi = std::max(1000.0, nc + 10.0 * std::sqrt(2.0 * df + 4.0 * nc));

    for (int iter = 0; iter < MAX_ITERATIONS; ++iter) {
        double mid = (lo + hi) / 2.0;
        double sf = ncx2_sf(mid, df, nc);

        if (sf > p) {
            lo = mid;
        } else {
            hi = mid;
        }

        if ((hi - lo) < CONVERGENCE_TOLERANCE * std::max(std::abs(hi), 1e-15)) {
            break;
        }
    }

    return (lo + hi) / 2.0;
#endif
}

} // namespace stats
} // namespace cxfunc

#endif // CXFUNC_STATS_UTILS_HPP