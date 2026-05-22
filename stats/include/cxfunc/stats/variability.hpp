#ifndef CXFUNC_STATS_VARIABILITY_HPP
#define CXFUNC_STATS_VARIABILITY_HPP

#include <Eigen/Dense>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits>
#include <complex>

#ifdef CXFUNC_USE_BOOST_MATH
#include <boost/math/distributions/chi_squared.hpp>
#endif

#include "cxfunc/stats/version.hpp"
#include "cxfunc/stats/utils.hpp"

namespace cxfunc {
namespace stats {

/**
 * @brief Calculate fractional excess variance F_var
 *
 * F_var is an intrinsic variability estimator that measures the
 * excess variance after accounting for measurement errors.
 *
 * Formula: F_var = sqrt((S^2 - mean(sigma^2)) / mean(x)^2)
 *
 * @param flux Measured fluxes
 * @param flux_err Error on measured fluxes
 * @return Pair of (fvar, fvar_err)
 *
 * References:
 * - Vaughan et al. (2003), MNRAS 345, 1271
 */
inline std::pair<double, double> compute_fvar(const Eigen::VectorXd& flux,
                                              const Eigen::VectorXd& flux_err) {
    if (flux.size() != flux_err.size()) {
        throw std::invalid_argument("flux and flux_err must have the same size");
    }

    int n = flux.size();

    // Calculate mean and count valid points
    double flux_mean = 0.0;
    int n_valid = 0;
    for (int i = 0; i < n; ++i) {
        if (!std::isnan(flux[i])) {
            flux_mean += flux[i];
            ++n_valid;
        }
    }

    if (n_valid < 2) {
        return {std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN()};
    }

    flux_mean /= n_valid;

    // Calculate variance and error variance
    double s_square = 0.0;
    double sig_square = 0.0;

    for (int i = 0; i < n; ++i) {
        if (!std::isnan(flux[i])) {
            s_square += (flux[i] - flux_mean) * (flux[i] - flux_mean);
            sig_square += flux_err[i] * flux_err[i];
        }
    }

    s_square /= (n_valid - 1);
    sig_square /= n_valid;

    double fvar = std::sqrt(std::max(s_square - sig_square, 0.0)) / std::abs(flux_mean);

    // Calculate error on F_var
    double sigxserr_a = std::sqrt(2.0 / n_valid) * sig_square / (flux_mean * flux_mean);
    double sigxserr_b = std::sqrt(sig_square / n_valid) * (2.0 * fvar / std::abs(flux_mean));
    double sigxserr = std::sqrt(sigxserr_a * sigxserr_a + sigxserr_b * sigxserr_b);
    double fvar_err = (fvar > 0) ? sigxserr / (2.0 * fvar) : 0.0;

    return {fvar, fvar_err};
}

/**
 * @brief Calculate point-to-point excess variance F_pp
 *
 * F_pp probes variability on shorter timescales than F_var.
 * For white noise, F_pp and F_var give the same value.
 *
 * @param flux Measured fluxes
 * @param flux_err Error on measured fluxes
 * @return Pair of (fpp, fpp_err)
 *
 * References:
 * - Edelson et al. (2002), ApJ 568, 684
 */
inline std::pair<double, double> compute_fpp(const Eigen::VectorXd& flux,
                                             const Eigen::VectorXd& flux_err) {
    if (flux.size() != flux_err.size()) {
        throw std::invalid_argument("flux and flux_err must have the same size");
    }

    int n = flux.size();

    // Calculate mean
    double flux_mean = 0.0;
    int n_valid = 0;
    for (int i = 0; i < n; ++i) {
        if (!std::isnan(flux[i])) {
            flux_mean += flux[i];
            ++n_valid;
        }
    }

    if (n_valid < 2) {
        return {std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN()};
    }

    flux_mean /= n_valid;

    // Calculate point-to-point variance
    double s_square = 0.0;
    double sig_square = 0.0;

    for (int i = 0; i < n; ++i) {
        if (!std::isnan(flux[i])) {
            sig_square += flux_err[i] * flux_err[i];
        }
    }
    sig_square /= n_valid;

    for (int i = 1; i < n; ++i) {
        if (!std::isnan(flux[i]) && !std::isnan(flux[i - 1])) {
            s_square += (flux[i] - flux[i - 1]) * (flux[i] - flux[i - 1]);
        }
    }
    s_square /= (n_valid - 1);

    double fpp = std::sqrt(std::max(s_square - sig_square, 0.0)) / std::abs(flux_mean);

    // Calculate error on F_pp
    double sigxserr_a = std::sqrt(2.0 / n_valid) * sig_square / (flux_mean * flux_mean);
    double sigxserr_b = std::sqrt(sig_square / n_valid) * (2.0 * fpp / std::abs(flux_mean));
    double sigxserr = std::sqrt(sigxserr_a * sigxserr_a + sigxserr_b * sigxserr_b);
    double fpp_err = (fpp > 0) ? sigxserr / (2.0 * fpp) : 0.0;

    return {fpp, fpp_err};
}

/**
 * @brief Calculate chi-square test for variability
 *
 * Tests if flux measurements are consistent with a constant source.
 *
 * @param flux Measured fluxes
 * @return Pair of (chi2, p_value)
 *
 * References:
 * - scipy.stats.chisquare documentation
 */
inline std::pair<double, double> compute_chisq(const Eigen::VectorXd& flux) {
    int n = flux.size();

    // Calculate expected value (mean)
    double y_exp = 0.0;
    int n_valid = 0;
    for (int i = 0; i < n; ++i) {
        if (!std::isnan(flux[i])) {
            y_exp += flux[i];
            ++n_valid;
        }
    }

    if (n_valid < 2) {
        return {std::numeric_limits<double>::quiet_NaN(),
                std::numeric_limits<double>::quiet_NaN()};
    }

    y_exp /= n_valid;

    // Calculate chi-square statistic
    double chi2 = 0.0;
    for (int i = 0; i < n; ++i) {
        if (!std::isnan(flux[i])) {
            double diff = flux[i] - y_exp;
            chi2 += (diff * diff) / std::max(y_exp, 1e-15);
        }
    }

    // Calculate p-value using chi2 distribution with n-1 degrees of freedom
    double p_value = 1.0;

#ifdef GAMMAPY_USE_BOOST_MATH
    using default_policy = boost::math::policies::policy<>;
    boost::math::chi_squared_distribution<double, default_policy> chi2_dist(n_valid - 1);
    p_value = 1.0 - boost::math::cdf(chi2_dist, chi2);
#else
    // Fallback: approximation using complementary error function
    // This is a rough approximation for large degrees of freedom
    double z = std::sqrt(2.0 * chi2) - std::sqrt(2.0 * (n_valid - 1) - 1);
    p_value = 0.5 * std::erfc(z / std::sqrt(2.0));
#endif

    p_value = std::max(0.0, std::min(1.0, p_value));

    return {chi2, p_value};
}

/**
 * @brief Compute minimum characteristic flux doubling and halving times
 *
 * @param flux Measured fluxes
 * @param flux_err Error on measured fluxes
 * @param coords Time coordinates
 * @return Vector of {doubling_time, doubling_err, doubling_coord,
 *                    halving_time, halving_err, halving_coord}
 */
inline std::vector<double> compute_flux_doubling(const Eigen::VectorXd& flux,
                                                 const Eigen::VectorXd& flux_err,
                                                 const Eigen::VectorXd& coords) {
    if (flux.size() != flux_err.size() || flux.size() != coords.size()) {
        throw std::invalid_argument("flux, flux_err, and coords must have the same size");
    }

    int n = coords.size();
    std::vector<double> result(6, std::numeric_limits<double>::quiet_NaN());

    double min_dt = std::numeric_limits<double>::infinity();
    double max_dt = -std::numeric_limits<double>::infinity();
    double min_dt_coord = 0.0, max_dt_coord = 0.0;
    double min_dt_err = 0.0, max_dt_err = 0.0;

    for (int i = 1; i < n; ++i) {
        if (std::isnan(flux[i]) || std::isnan(flux[i - 1])) continue;
        if (flux[i] <= 0 || flux[i - 1] <= 0) continue;

        double dt = coords[i] - coords[i - 1];
        double log_ratio = std::log(flux[i] / flux[i - 1]);
        if (std::abs(log_ratio) < 1e-15) continue;

        double doubling_time = dt / (log_ratio / std::log(2.0));

        // Error propagation
        double err1 = dt * std::log(2.0) / (flux[i] * log_ratio * log_ratio);
        double err2 = dt * std::log(2.0) / (flux[i - 1] * log_ratio * log_ratio);
        double err = std::sqrt((flux_err[i] * err1) * (flux_err[i] * err1) +
                              (flux_err[i - 1] * err2) * (flux_err[i - 1] * err2));

        if (doubling_time > 0 && doubling_time < min_dt) {
            min_dt = doubling_time;
            min_dt_coord = coords[i];
            min_dt_err = err;
        }

        if (doubling_time < 0 && doubling_time > max_dt) {
            max_dt = doubling_time;
            max_dt_coord = coords[i];
            max_dt_err = err;
        }
    }

    if (!std::isinf(min_dt)) {
        result[0] = min_dt;
        result[1] = min_dt_err;
        result[2] = min_dt_coord;
    }

    if (!std::isinf(max_dt)) {
        result[3] = std::abs(max_dt);
        result[4] = max_dt_err;
        result[5] = max_dt_coord;
    }

    return result;
}

/**
 * @brief Timmer-Koenig lightcurve simulator
 *
 * Simulates a time series from a given power spectrum using the
 * Timmer-Koenig algorithm.
 */
class TimmerKoeningSimulator {
public:
    /**
     * @param seed Random seed for reproducibility
     */
    explicit TimmerKoeningSimulator(unsigned int seed = 42) : rng_state_(seed) {
        // Initialize random number generator state
    }

    /**
     * @brief Generate a time series from a power spectrum
     *
     * @param power_spectrum Function that maps frequency -> power
     * @param npoints Number of output points
     * @param spacing Sample spacing (time interval)
     * @param n_chunks Extension factor to avoid red noise leakage (default 10)
     * @param mean Desired mean of output (default 0)
     * @param std_val Desired standard deviation of output (default 1)
     * @param poisson Apply Poisson sampling (default false)
     * @return Pair of (time_series, time_axis)
     */
    std::pair<Eigen::VectorXd, Eigen::VectorXd>
    operator()(const std::function<double(double)>& power_spectrum,
               int npoints, double spacing,
               int n_chunks = 10,
               double mean = 0.0,
               double std_val = 1.0,
               bool poisson = false) const {

        int npoints_ext = npoints * n_chunks;

        // Generate FFT frequencies
        Eigen::VectorXd freqs = Eigen::VectorXd::Zero(npoints_ext);
        for (int i = 0; i < npoints_ext; ++i) {
            freqs[i] = (i < npoints_ext / 2) ?
                       static_cast<double>(i) / spacing :
                       static_cast<double>(i - npoints_ext) / spacing;
        }

        // Sort positive frequencies by absolute value
        std::vector<int> pos_idx;
        for (int i = 0; i < npoints_ext; ++i) {
            if (freqs[i] < 0) pos_idx.push_back(i);
        }
        std::sort(pos_idx.begin(), pos_idx.end(),
                  [&freqs](int a, int b) { return std::abs(freqs[a]) < std::abs(freqs[b]); });

        // Evaluate power spectrum
        Eigen::VectorXd periodogram(pos_idx.size());
        for (size_t i = 0; i < pos_idx.size(); ++i) {
            periodogram[i] = power_spectrum(std::abs(freqs[pos_idx[i]]));
        }

        // Generate random Fourier coefficients
        std::vector<std::complex<double>> fourier_coeffs(npoints_ext);
        for (size_t i = 0; i < pos_idx.size() - 1; ++i) {
            double real_part = normal_random_();
            double imag_part = normal_random_();
            fourier_coeffs[i] = std::sqrt(0.5 * periodogram[i]) *
                               std::complex<double>(real_part, imag_part);
        }

        // Handle Nyquist frequency
        size_t last = pos_idx.size() - 1;
        if (npoints_ext % 2 == 0) {
            fourier_coeffs[last] = std::sqrt(0.5 * periodogram[last]) *
                                   std::complex<double>(normal_random_(), 0.0);
        } else {
            fourier_coeffs[last] = std::sqrt(0.5 * periodogram[last]) *
                                   std::complex<double>(normal_random_(), normal_random_());
        }

        // Conjugate symmetry for real output
        for (size_t i = 0; i < pos_idx.size(); ++i) {
            fourier_coeffs[npoints_ext - 1 - pos_idx[i]] = std::conj(fourier_coeffs[i]);
        }

        // Inverse FFT (simplified implementation)
        Eigen::VectorXd time_series_ext = inverse_fft_(fourier_coeffs);

        // Select center portion
        int ndiv = npoints_ext / (2 * n_chunks);
        int setstart = npoints_ext / 2 - ndiv;
        int setend = npoints_ext / 2 + ndiv;
        if (npoints % 2 != 0) setend += 1;

        Eigen::VectorXd time_series = time_series_ext.segment(setstart, setend - setstart);

        // Normalize to desired mean and std
        double ts_mean = time_series.mean();
        double ts_std = 0.0;
        for (int i = 0; i < time_series.size(); ++i) {
            ts_std += (time_series[i] - ts_mean) * (time_series[i] - ts_mean);
        }
        ts_std = std::sqrt(ts_std / time_series.size());

        time_series = ((time_series.array() - ts_mean) / std::max(ts_std, 1e-15) * std_val) + mean;

        // Apply Poisson noise if requested
        if (poisson) {
            for (int i = 0; i < time_series.size(); ++i) {
                double lambda = std::max(time_series[i], 0.0) * spacing;
                // Simple Poisson sampling using normal approximation
                time_series[i] = (lambda + std::sqrt(lambda) * normal_random_()) / spacing;
            }
        }

        // Generate time axis
        Eigen::VectorXd time_axis = Eigen::VectorXd::LinSpaced(npoints, 0.0,
                                                               static_cast<double>(npoints - 1) * spacing);

        return {time_series, time_axis};
    }

private:
    mutable unsigned int rng_state_;

    // Simple linear congruential generator
    struct RNG {
        unsigned int& state;
        explicit RNG(unsigned int& s) : state(s) {}

        double operator()() {
            state = 1664525u * state + 1013904223u;
            return static_cast<double>(state) / 4294967296.0;
        }
    };

    mutable RNG rng_{rng_state_};

    double normal_random_() const {
        // Box-Muller transform
        double u1 = rng_();
        while (u1 < 1e-15) u1 = rng_();
        double u2 = rng_();
        return std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * PI * u2);
    }

    Eigen::VectorXd inverse_fft_(const std::vector<std::complex<double>>& coeffs) const {
        int n = coeffs.size();
        Eigen::VectorXd result(n);

        for (int k = 0; k < n; ++k) {
            std::complex<double> sum(0.0, 0.0);
            for (int j = 0; j < n; ++j) {
                double angle = 2.0 * PI * k * j / n;
                std::complex<double> phase(std::cos(angle), std::sin(angle));
                sum += coeffs[j] * phase;
            }
            result[k] = sum.real() / n;
        }

        return result;
    }
};

/**
 * @brief Compute discrete structure function
 *
 * @param flux Measured fluxes
 * @param flux_err Error on measured fluxes
 * @param time Time coordinates
 * @return Pair of (structure_function, distances)
 */
inline std::pair<Eigen::VectorXd, Eigen::VectorXd>
structure_function(const Eigen::VectorXd& flux,
                   const Eigen::VectorXd& flux_err,
                   const Eigen::VectorXd& time) {

    if (flux.size() != flux_err.size() || flux.size() != time.size()) {
        throw std::invalid_argument("flux, flux_err, and time must have the same size");
    }

    int n = flux.size();

    // Calculate distance matrix
    Eigen::MatrixXd dist(n, n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            dist(i, j) = std::abs(time[i] - time[j]);
        }
    }

    // Find unique positive distances
    std::vector<double> unique_distances;
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (dist(i, j) > 1e-10) {
                unique_distances.push_back(dist(i, j));
            }
        }
    }

    std::sort(unique_distances.begin(), unique_distances.end());
    unique_distances.erase(std::unique(unique_distances.begin(), unique_distances.end()),
                          unique_distances.end());

    // Calculate structure function for each distance
    Eigen::VectorXd sf(unique_distances.size());

    for (size_t d = 0; d < unique_distances.size(); ++d) {
        double distance = unique_distances[d];
        double factor = 0.0, norm = 0.0;

        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (std::abs(dist(i, j) - distance) < 1e-10) {
                    double f_val = (flux[j] - flux[i]) * (flux[j] - flux[i]);
                    double w = (flux[j] / std::max(flux_err[j], 1e-15)) *
                             (flux[i] / std::max(flux_err[i], 1e-15));

                    if (!std::isnan(f_val) && !std::isnan(w)) {
                        factor += f_val * w;
                        norm += w;
                    }
                }
            }
        }

        sf[d] = (norm > 0) ? factor / norm : 0.0;
    }

    Eigen::VectorXd distances = Eigen::VectorXd::Map(unique_distances.data(),
                                                     unique_distances.size());

    return {sf, distances};
}

/**
 * @brief Compute discrete correlation function (DCF)
 *
 * @param flux1 First flux series
 * @param flux_err1 First flux error series
 * @param flux2 Second flux series
 * @param flux_err2 Second flux error series
 * @param time1 First time series
 * @param time2 Second time series
 * @param tau Time bin size
 * @return Tuple of (bin_centers, dcf, dcf_err)
 */
inline std::tuple<Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd>
discrete_correlation(const Eigen::VectorXd& flux1,
                     const Eigen::VectorXd& flux_err1,
                     const Eigen::VectorXd& flux2,
                     const Eigen::VectorXd& flux_err2,
                     const Eigen::VectorXd& time1,
                     const Eigen::VectorXd& time2,
                     double tau) {

    // Calculate means and standard deviations
    double mean1 = 0, mean2 = 0;
    int n1_valid = 0, n2_valid = 0;

    for (int i = 0; i < flux1.size(); ++i) {
        if (!std::isnan(flux1[i])) {
            mean1 += flux1[i];
            ++n1_valid;
        }
    }
    for (int i = 0; i < flux2.size(); ++i) {
        if (!std::isnan(flux2[i])) {
            mean2 += flux2[i];
            ++n2_valid;
        }
    }

    mean1 /= std::max(n1_valid, 1);
    mean2 /= std::max(n2_valid, 1);

    double std1 = 0, std2 = 0;
    for (int i = 0; i < flux1.size(); ++i) {
        if (!std::isnan(flux1[i])) {
            std1 += (flux1[i] - mean1) * (flux1[i] - mean1);
        }
    }
    for (int i = 0; i < flux2.size(); ++i) {
        if (!std::isnan(flux2[i])) {
            std2 += (flux2[i] - mean2) * (flux2[i] - mean2);
        }
    }

    std1 = std::sqrt(std1 / std::max(n1_valid, 1));
    std2 = std::sqrt(std2 / std::max(n2_valid, 1));

    // Calculate unbiased DCF
    std::vector<double> bin_centers, dcf_values, dcf_errors;

    double min_time = std::min(time1.minCoeff(), time2.minCoeff());
    double max_time = std::max(time1.maxCoeff(), time2.maxCoeff());
    int n_bins = static_cast<int>(std::abs(max_time - min_time) / tau) + 1;

    for (int b = -n_bins; b <= n_bins; ++b) {
        double lo = static_cast<double>(b) * tau;
        double hi = static_cast<double>(b + 1) * tau;
        double mid = (lo + hi) / 2.0;

        std::vector<double> udcf_values;

        for (int i = 0; i < flux1.size(); ++i) {
            if (std::isnan(flux1[i])) continue;
            for (int j = 0; j < flux2.size(); ++j) {
                if (std::isnan(flux2[j])) continue;

                double dt = time1[i] - time2[j];
                if (dt >= lo && dt < hi) {
                    double udcf1 = (flux1[i] - mean1) / std::max(std1, 1e-15);
                    double udcf2 = (flux2[j] - mean2) / std::max(std2, 1e-15);
                    udcf_values.push_back(udcf1 * udcf2);
                }
            }
        }

        if (!udcf_values.empty()) {
            double sum = 0, sq_sum = 0;
            for (double v : udcf_values) {
                sum += v;
                sq_sum += v * v;
            }

            double mean_v = sum / udcf_values.size();
            double variance = (sq_sum / udcf_values.size()) - mean_v * mean_v;
            double error = std::sqrt(std::max(variance, 0.0)) /
                          std::max(static_cast<int>(udcf_values.size()) - 1, 1);

            bin_centers.push_back(mid);
            dcf_values.push_back(mean_v);
            dcf_errors.push_back(error);
        }
    }

    auto to_vector = [](const std::vector<double>& v) {
        return Eigen::VectorXd::Map(v.data(), v.size());
    };

    return {to_vector(bin_centers), to_vector(dcf_values), to_vector(dcf_errors)};
}

} // namespace stats
} // namespace cxfunc

#endif // CXFUNC_STATS_VARIABILITY_HPP