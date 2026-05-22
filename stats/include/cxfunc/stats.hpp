#ifndef CXFUNC_STATS_HPP
#define CXFUNC_STATS_HPP

/**
 * @file cxfunc/stats.hpp
 * @brief Main header file for cxfunc statistics library
 *
 * This library provides statistical analysis tools for gamma-ray astronomy,
 * including likelihood statistics, variability analysis, and count statistics.
 *
 * Key features:
 * - Fit statistics: Cash, C-stat, W-stat (Li & Ma formula)
 * - Count statistics: Significance calculation, error estimation, upper limits
 * - Variability analysis: F_var, F_pp, structure functions, lightcurve simulation
 * - Utility functions: TS/sigma conversion, special functions
 *
 * Dependencies:
 * - Eigen3 (required): Linear algebra and vector operations
 * - Boost.Math (optional): Special functions (Lambert W, non-central chi2)
 * - GSL (optional): Root finding algorithms
 *
 * Usage example:
 * @code
 * #include <cxfunc/stats.hpp>
 *
 * using namespace cxfunc::stats;
 *
 * // Calculate W-statistic (Li & Ma formula)
 * Eigen::VectorXd n_on(3), n_off(3), alpha(3), mu_sig(3);
 * n_on << 10, 15, 8;
 * n_off << 20, 18, 25;
 * alpha << 0.5, 0.5, 0.5;
 * mu_sig << 0, 0, 0;
 *
 * Eigen::VectorXd wstat_values = wstat(n_on, n_off, alpha, mu_sig);
 * std::cout << "W-statistic: " << wstat_values.transpose() << std::endl;
 *
 * // Count statistics with error estimation
 * CashCountsStatistic cash_stat(10.0, 5.0);
 * std::cout << "Significance: " << cash_stat.sqrt_ts() << " sigma" << std::endl;
 * std::cout << "Upper limit: " << cash_stat.compute_upper_limit(3.0) << std::endl;
 * @endcode
 */

// Version information and configuration
#include "cxfunc/stats/version.hpp"

// Core statistical functions
#include "cxfunc/stats/fit_statistics.hpp"
#include "cxfunc/stats/counts_statistic.hpp"

// Variability analysis tools
#include "cxfunc/stats/variability.hpp"

// Utility functions
#include "cxfunc/stats/utils.hpp"

/**
 * @brief Main namespace for cxfunc statistics library
 */
namespace cxfunc {
/**
 * @brief Statistics namespace containing all analysis functions
 */
namespace stats {
// All functions and classes are available in this namespace
}
}

#endif // CXFUNC_STATS_HPP