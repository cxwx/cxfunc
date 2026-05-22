/**
 * @file basic_example.cpp
 * @brief Basic example of using cxfunc statistics library
 *
 * This example demonstrates:
 * - Calculating fit statistics (Cash, W-stat)
 * - Computing count statistics with significance
 * - Performing variability analysis
 */

#include <iostream>
#include <iomanip>
#include <Eigen/Dense>
#include <cxfunc/stats.hpp>

int main() {
    std::cout << "CxFunc Stats Library - Basic Example" << std::endl;
    std::cout << "=====================================" << std::endl;
    std::cout << "Version: " << cxfunc::stats::version_string() << std::endl;
    std::cout << std::endl;

    using namespace cxfunc::stats;

    // ====== Example 1: W-statistic (Li & Ma formula) ======
    std::cout << "1. W-statistic (Li & Ma Formula)" << std::endl;
    std::cout << "   --------------------------------" << std::endl;

    // Create some sample ON-OFF measurement data
    Eigen::VectorXd n_on(3);    // Observed counts in ON region
    Eigen::VectorXd n_off(3);   // Observed counts in OFF region
    Eigen::VectorXd alpha(3);   // Acceptance ratio (ON/OFF exposure)
    Eigen::VectorXd mu_sig(3);  // Expected signal (usually 0 for background estimation)

    n_on << 10, 15, 8;
    n_off << 20, 18, 25;
    alpha << 0.5, 0.5, 0.5;
    mu_sig << 0, 0, 0;

    // Calculate W-statistic
    Eigen::VectorXd wstat_results = wstat(n_on, n_off, alpha, mu_sig);

    std::cout << "   Input data:" << std::endl;
    std::cout << "   n_on   : " << n_on.transpose() << std::endl;
    std::cout << "   n_off  : " << n_off.transpose() << std::endl;
    std::cout << "   alpha  : " << alpha.transpose() << std::endl;
    std::cout << "   W-stat : " << wstat_results.transpose() << std::endl;
    std::cout << std::endl;

    // ====== Example 2: Cash count statistics ======
    std::cout << "2. Cash Count Statistics" << std::endl;
    std::cout << "   ----------------------" << std::endl;

    // Analyze a single observation
    double observed_counts = 10.0;
    double expected_background = 5.0;

    CashCountsStatistic cash_stat(observed_counts, expected_background);

    std::cout << "   Observation: " << observed_counts << " counts" << std::endl;
    std::cout << "   Background:  " << expected_background << " counts" << std::endl;
    std::cout << "   Excess:     " << cash_stat.excess() << " counts" << std::endl;
    std::cout << "   Significance: " << cash_stat.sqrt_ts() << " sigma" << std::endl;
    std::cout << "   p-value:     " << cash_stat.p_value() << std::endl;
    std::cout << std::endl;

    // ====== Example 3: Error estimation and upper limits ======
    std::cout << "3. Error Estimation and Upper Limits" << std::endl;
    std::cout << "   ----------------------------------" << std::endl;

    double errn = cash_stat.compute_errn(1.0);  // 1-sigma downward error
    double errp = cash_stat.compute_errp(1.0);  // 1-sigma upward error
    double ul = cash_stat.compute_upper_limit(3.0);  // 3-sigma upper limit

    std::cout << "   1-sigma errors: -" << errn << ", +" << errp << std::endl;
    std::cout << "   3-sigma upper limit: " << ul << " counts" << std::endl;
    std::cout << std::endl;

    // ====== Example 4: W-stat count statistics ======
    std::cout << "4. W-stat Count Statistics (ON-OFF)" << std::endl;
    std::cout << "   --------------------------------" << std::endl;

    double n_on_val = 10.0, n_off_val = 20.0, alpha_val = 0.5;
    WStatCountsStatistic wstat_stat(n_on_val, n_off_val, alpha_val);

    std::cout << "   ON counts:   " << n_on_val << std::endl;
    std::cout << "   OFF counts:  " << n_off_val << std::endl;
    std::cout << "   Alpha:       " << alpha_val << std::endl;
    std::cout << "   Background:  " << wstat_stat.n_bkg() << " counts" << std::endl;
    std::cout << "   Excess:      " << wstat_stat.excess() << " counts" << std::endl;
    std::cout << "   Significance: " << wstat_stat.sqrt_ts() << " sigma" << std::endl;
    std::cout << std::endl;

    // ====== Example 5: Variability analysis ======
    std::cout << "5. Variability Analysis" << std::endl;
    std::cout << "   --------------------" << std::endl;

    // Create a simple light curve
    Eigen::VectorXd flux(5), flux_err(5);
    flux << 1.0, 1.2, 0.8, 1.1, 0.9;
    flux_err << 0.1, 0.1, 0.1, 0.1, 0.1;

    auto [fvar, fvar_err] = compute_fvar(flux, flux_err);
    auto [fpp, fpp_err] = compute_fpp(flux, flux_err);
    auto [chi2, p_value] = compute_chisq(flux);

    std::cout << "   Light curve: " << flux.transpose() << std::endl;
    std::cout << "   F_var:       " << fvar << " ± " << fvar_err << std::endl;
    std::cout << "   F_pp:        " << fpp << " ± " << fpp_err << std::endl;
    std::cout << "   Chi²:        " << chi2 << " (p=" << p_value << ")" << std::endl;
    std::cout << std::endl;

    // ====== Example 6: TS/Sigma conversion ======
    std::cout << "6. TS/Sigma Conversion" << std::endl;
    std::cout << "   --------------------" << std::endl;

    double n_sigma = 5.0;
    double ts = sigma_to_ts(n_sigma);
    double sigma_back = ts_to_sigma(ts);

    std::cout << "   Input:       " << n_sigma << " sigma" << std::endl;
    std::cout << "   TS value:    " << ts << std::endl;
    std::cout << "   Back to sigma: " << sigma_back << " sigma" << std::endl;
    std::cout << std::endl;

    // ====== Example 7: Finding excess for given significance ======
    std::cout << "7. Inverse: Find Excess for Given Significance" << std::endl;
    std::cout << "   --------------------------------------------" << std::endl;

    double target_significance = 5.0;  // 5 sigma
    double background = 100.0;

    CashCountsStatistic inverse_stat(0.0, background);  // Zero observation
    double required_excess = inverse_stat.n_sig_matching_significance(target_significance);

    std::cout << "   For " << target_significance << " sigma discovery with "
              << background << " background counts:" << std::endl;
    std::cout << "   Required excess: " << required_excess << " counts" << std::endl;
    std::cout << "   Total required:  " << (background + required_excess) << " counts" << std::endl;

    std::cout << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Example completed successfully!" << std::endl;

    return 0;
}