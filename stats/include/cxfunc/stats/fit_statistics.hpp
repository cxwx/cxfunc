#ifndef CXFUNC_STATS_FIT_STATISTICS_HPP
#define CXFUNC_STATS_FIT_STATISTICS_HPP

#include <Eigen/Dense>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "cxfunc/stats/version.hpp"

#ifdef CXFUNC_USE_BOOST_MATH
#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/special_functions/expm1.hpp>
#endif

namespace cxfunc {
namespace stats {

/**
 * @brief Cash statistic per bin: C = 2 * (mu - n * log(mu))
 *
 * The Cash statistic is used for Poisson data with known background.
 * It is defined as: C = 2 * (mu_on - n_on * log(mu_on))
 *
 * @param n_on Observed counts
 * @param mu_on Expected counts
 * @param truncation_value Minimum value for mu_on (default 1e-25)
 * @return Vector of stat values per bin
 *
 * References:
 * - Cash (1979), ApJ 228, 939
 * - http://cxc.cfa.harvard.edu/sherpa/statistics/#cash
 */
inline Eigen::VectorXd cash(const Eigen::VectorXd& n_on,
                            const Eigen::VectorXd& mu_on,
                            double truncation_value = TRUNCATION_VALUE) {
    if (n_on.size() != mu_on.size()) {
        throw std::invalid_argument("n_on and mu_on must have the same size");
    }

    int n = n_on.size();
    Eigen::VectorXd stat(n);

    for (int i = 0; i < n; ++i) {
        double mu = std::max(mu_on[i], truncation_value);
        stat[i] = 2.0 * (mu - n_on[i] * std::log(mu));
    }

    return stat;
}

/**
 * @brief Cash statistic sum (scalar)
 */
inline double cash_sum(const Eigen::VectorXd& n_on,
                       const Eigen::VectorXd& mu_on,
                       double truncation_value = TRUNCATION_VALUE) {
    return cash(n_on, mu_on, truncation_value).sum();
}

/**
 * @brief Weighted Cash statistic sum
 *
 * S = sum_i [ w_i * (mu_i - n_i * log(mu_i)) ]
 * Used in UNIRAW method for weighted likelihood analysis.
 */
inline double weighted_cash_sum(const Eigen::VectorXd& counts,
                                const Eigen::VectorXd& npred,
                                const Eigen::VectorXd& weights) {
    if (counts.size() != npred.size() || counts.size() != weights.size()) {
        throw std::invalid_argument("counts, npred, and weights must have the same size");
    }

    double sum = 0.0;
    for (int i = 0; i < counts.size(); ++i) {
        double mu = std::max(npred[i], TRUNCATION_VALUE);
        sum += weights[i] * (mu - counts[i] * std::log(mu));
    }

    return 2.0 * sum;
}

/**
 * @brief C statistic per bin: C = 2 * [mu - n + n * (log(n) - log(mu))]
 *
 * The C statistic is an alternative to Cash statistic for Poisson data.
 * Better handling of low-count regimes.
 *
 * @param n_on Observed counts
 * @param mu_on Expected counts
 * @param truncation_value Minimum value for n_on and mu_on (default 1e-25)
 * @return Vector of stat values per bin
 *
 * References:
 * - http://cxc.harvard.edu/sherpa/statistics/#cstat
 */
inline Eigen::VectorXd cstat(const Eigen::VectorXd& n_on,
                             const Eigen::VectorXd& mu_on,
                             double truncation_value = TRUNCATION_VALUE) {
    if (n_on.size() != mu_on.size()) {
        throw std::invalid_argument("n_on and mu_on must have the same size");
    }

    int n = n_on.size();
    Eigen::VectorXd stat(n);

    for (int i = 0; i < n; ++i) {
        double n_val = std::max(n_on[i], truncation_value);
        double mu = std::max(mu_on[i], truncation_value);
        double term1 = std::log(n_val) - std::log(mu);
        stat[i] = 2.0 * (mu - n_val + n_val * term1);
    }

    return stat;
}

/**
 * @brief W statistic per bin (Poisson data with unknown background)
 *
 * This is the implementation of the Li & Ma formula (Eq. 17) for
 * Poisson data with Poisson background. It uses profile likelihood
 * to eliminate the background nuisance parameter.
 *
 * @param n_on Total observed counts in ON region
 * @param n_off Observed counts in OFF region
 * @param alpha Acceptance ratio (ON/OFF exposure ratio)
 * @param mu_sig Signal expected counts
 * @param extra_terms Add goodness-of-fit terms (default true)
 * @return Vector of stat values per bin
 *
 * References:
 * - Li & Ma (1983), ApJ 272, 317 (Eq. 17)
 * - http://heasarc.gsfc.nasa.gov/xanadu/xspec/manual/XSappendixStatistics.html
 */
inline Eigen::VectorXd wstat(const Eigen::VectorXd& n_on,
                             const Eigen::VectorXd& n_off,
                             const Eigen::VectorXd& alpha,
                             const Eigen::VectorXd& mu_sig,
                             bool extra_terms = true) {
    if (n_on.size() != n_off.size() ||
        n_on.size() != alpha.size() ||
        n_on.size() != mu_sig.size()) {
        throw std::invalid_argument("All input vectors must have the same size");
    }

    int n = n_on.size();
    Eigen::VectorXd stat(n);

    for (int i = 0; i < n; ++i) {
        // Profile likelihood solution for mu_bkg
        double C = alpha[i] * (n_on[i] + n_off[i]) - (1.0 + alpha[i]) * mu_sig[i];
        double D2 = C * C + 4.0 * alpha[i] * (alpha[i] + 1.0) * n_off[i] * mu_sig[i];
        double D = std::sqrt(std::max(D2, 0.0));

        double mu_bkg = (C + D) / (2.0 * alpha[i] * (alpha[i] + 1.0));
        mu_bkg = std::max(mu_bkg, 0.0);

        // Calculate W statistic terms
        double term1 = mu_sig[i] + (1.0 + alpha[i]) * mu_bkg;

        // Handle n_on == 0 case: -n_on * log(...) = 0
        double term2 = 0.0;
        if (n_on[i] > 0) {
            double log_arg = mu_sig[i] + alpha[i] * mu_bkg;
            if (log_arg > 0) {
                term2 = -n_on[i] * std::log(log_arg);
            }
        }

        // Handle n_off == 0 case: -n_off * log(mu_bkg) = 0
        double term3 = 0.0;
        if (n_off[i] > 0) {
            if (mu_bkg > 0) {
                term3 = -n_off[i] * std::log(mu_bkg);
            }
        }

        stat[i] = 2.0 * (term1 + term2 + term3);

        // Add goodness-of-fit terms if requested
        if (extra_terms) {
            double gof = 0.0;
            if (n_on[i] > 0) {
                gof += -n_on[i] * (1.0 - std::log(n_on[i]));
            }
            if (n_off[i] > 0) {
                gof += -n_off[i] * (1.0 - std::log(n_off[i]));
            }
            stat[i] += 2.0 * gof;
        }
    }

    return stat;
}

/**
 * @brief Background estimate for WSTAT (profile likelihood solution)
 *
 * Solves the profile likelihood for background mu_bkg given the signal model.
 * The solution is obtained analytically using the quadratic formula.
 *
 * @param n_on Observed ON counts
 * @param n_off Observed OFF counts
 * @param alpha Acceptance ratio
 * @param mu_sig Signal expected counts
 * @return Background estimate per bin
 */
inline Eigen::VectorXd get_wstat_mu_bkg(const Eigen::VectorXd& n_on,
                                        const Eigen::VectorXd& n_off,
                                        const Eigen::VectorXd& alpha,
                                        const Eigen::VectorXd& mu_sig) {
    if (n_on.size() != n_off.size() ||
        n_on.size() != alpha.size() ||
        n_on.size() != mu_sig.size()) {
        throw std::invalid_argument("All input vectors must have the same size");
    }

    int n = n_on.size();
    Eigen::VectorXd mu_bkg(n);

    for (int i = 0; i < n; ++i) {
        double C = alpha[i] * (n_on[i] + n_off[i]) - (1.0 + alpha[i]) * mu_sig[i];
        double D2 = C * C + 4.0 * alpha[i] * (alpha[i] + 1.0) * n_off[i] * mu_sig[i];
        double D = std::sqrt(std::max(D2, 0.0));

        double val = (C + D) / (2.0 * alpha[i] * (alpha[i] + 1.0));
        mu_bkg[i] = std::max(val, 0.0);
    }

    return mu_bkg;
}

/**
 * @brief Goodness of fit terms for WSTAT
 *
 * These are the model-independent terms that convert the likelihood ratio
 * into a goodness-of-fit statistic.
 *
 * @param n_on Observed ON counts
 * @param n_off Observed OFF counts
 * @return Goodness-of-fit terms per bin
 */
inline Eigen::VectorXd get_wstat_gof_terms(const Eigen::VectorXd& n_on,
                                           const Eigen::VectorXd& n_off) {
    if (n_on.size() != n_off.size()) {
        throw std::invalid_argument("n_on and n_off must have the same size");
    }

    int n = n_on.size();
    Eigen::VectorXd terms(n);

    for (int i = 0; i < n; ++i) {
        double t = 0.0;
        if (n_on[i] > 0) {
            t += -n_on[i] * (1.0 - std::log(n_on[i]));
        }
        if (n_off[i] > 0) {
            t += -n_off[i] * (1.0 - std::log(n_off[i]));
        }
        terms[i] = 2.0 * t;
    }

    return terms;
}

} // namespace stats
} // namespace cxfunc

#endif // CXFUNC_STATS_FIT_STATISTICS_HPP