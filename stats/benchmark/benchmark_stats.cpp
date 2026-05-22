/**
 * @file benchmark_stats.cpp
 * @brief Performance benchmarks for statistics module
 */

#include <cxfunc/stats.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <iomanip>

using namespace cxfunc::stats;

// Timer class for performance measurement
class Timer {
    std::chrono::high_resolution_clock::time_point start_time_;
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}

    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end_time - start_time_).count();
    }

    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
};

// Generate random data
std::vector<double> generate_random_data(size_t n, double mean = 0.0, double stddev = 1.0) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(mean, stddev);

    std::vector<double> data(n);
    for (size_t i = 0; i < n; ++i) {
        data[i] = dist(gen);
    }
    return data;
}

void benchmark_basic_statistics(size_t n_samples, size_t n_iterations) {
    std::cout << "\n=== Basic Statistics ===" << std::endl;
    std::cout << "Data size: " << n_samples << ", Iterations: " << n_iterations << std::endl;

    auto data = generate_random_data(n_samples);

    Timer timer;

    // Test mean calculation
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile double result = compute_mean(data);
        (void)result;
    }
    double mean_time = timer.elapsed() / n_iterations;
    std::cout << "  Mean:           " << std::fixed << std::setprecision(6) << mean_time << " ms" << std::endl;

    // Test standard deviation
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile double result = compute_stddev(data);
        (void)result;
    }
    double stddev_time = timer.elapsed() / n_iterations;
    std::cout << "  StdDev:         " << stddev_time << " ms" << std::endl;

    // Test median
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile double result = compute_median(data);
        (void)result;
    }
    double median_time = timer.elapsed() / n_iterations;
    std::cout << "  Median:         " << median_time << " ms" << std::endl;

    // Test percentiles
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile double result = compute_percentile(data, 0.95);
        (void)result;
    }
    double percentile_time = timer.elapsed() / n_iterations;
    std::cout << "  Percentile(95): " << percentile_time << " ms" << std::endl;
}

void benchmark_probability_distributions(size_t n_samples, size_t n_iterations) {
    std::cout << "\n=== Probability Distributions ===" << std::endl;
    std::cout << "Samples: " << n_samples << ", Iterations: " << n_iterations << std::endl;

    Timer timer;

    // Test normal distribution
    NormalDist normal(0.0, 1.0);
    std::vector<double> x_values(n_samples);
    for (size_t i = 0; i < n_samples; ++i) {
        x_values[i] = (i - n_samples / 2.0) / (n_samples / 10.0);
    }

    timer.reset();
    double sum = 0.0;
    for (size_t iter = 0; iter < n_iterations; ++iter) {
        for (auto x : x_values) {
            sum += normal.pdf(x);
        }
    }
    double pdf_time = timer.elapsed() / n_iterations;
    std::cout << "  Normal PDF:     " << std::fixed << std::setprecision(6) << pdf_time << " ms" << std::endl;

    timer.reset();
    sum = 0.0;
    for (size_t iter = 0; iter < n_iterations; ++iter) {
        for (auto x : x_values) {
            sum += normal.cdf(x);
        }
    }
    double cdf_time = timer.elapsed() / n_iterations;
    std::cout << "  Normal CDF:     " << cdf_time << " ms" << std::endl;

    // Test Poisson distribution
    PoissonDist poisson(5.0);
    std::vector<int> k_values(100);
    for (size_t i = 0; i < 100; ++i) {
        k_values[i] = static_cast<int>(i);
    }

    timer.reset();
    sum = 0.0;
    for (size_t iter = 0; iter < n_iterations; ++iter) {
        for (auto k : k_values) {
            sum += poisson.pmf(k);
        }
    }
    double poisson_pmf_time = timer.elapsed() / n_iterations;
    std::cout << "  Poisson PMF:    " << poisson_pmf_time << " ms" << std::endl;
}

void benchmark_hypothesis_tests(size_t n_iterations) {
    std::cout << "\n=== Hypothesis Tests ===" << std::endl;
    std::cout << "Iterations: " << n_iterations << std::endl;

    auto data1 = generate_random_data(1000, 0.0, 1.0);
    auto data2 = generate_random_data(1000, 0.5, 1.0);

    Timer timer;

    // Test t-test
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile auto result = t_test(data1, data2);
        (void)result;
    }
    double ttest_time = timer.elapsed() / n_iterations;
    std::cout << "  T-test:         " << std::fixed << std::setprecision(6) << ttest_time << " ms" << std::endl;

    // Test KS test
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile auto result = ks_test(data1, data2);
        (void)result;
    }
    double ks_time = timer.elapsed() / n_iterations;
    std::cout << "  KS test:        " << ks_time << " ms" << std::endl;
}

void benchmark_correlation(size_t n_samples, size_t n_iterations) {
    std::cout << "\n=== Correlation Analysis ===" << std::endl;
    std::cout << "Data size: " << n_samples << ", Iterations: " << n_iterations << std::endl;

    auto x = generate_random_data(n_samples);
    auto y = generate_random_data(n_samples);

    Timer timer;

    // Pearson correlation
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile double result = compute_correlation(x, y);
        (void)result;
    }
    double pearson_time = timer.elapsed() / n_iterations;
    std::cout << "  Pearson:        " << std::fixed << std::setprecision(6) << pearson_time << " ms" << std::endl;

    // Spearman correlation
    timer.reset();
    for (size_t i = 0; i < n_iterations; ++i) {
        volatile double result = compute_spearman_correlation(x, y);
        (void)result;
    }
    double spearman_time = timer.elapsed() / n_iterations;
    std::cout << "  Spearman:       " << spearman_time << " ms" << std::endl;
}

int main() {
    std::cout << "=============================================================" << std::endl;
    std::cout << "CXFUNC STATISTICS MODULE - PERFORMANCE BENCHMARKS" << std::endl;
    std::cout << "=============================================================" << std::endl;

    try {
        // Benchmark basic statistics
        benchmark_basic_statistics(10000, 1000);

        // Benchmark probability distributions
        benchmark_probability_distributions(10000, 1000);

        // Benchmark hypothesis tests
        benchmark_hypothesis_tests(1000);

        // Benchmark correlation
        benchmark_correlation(10000, 1000);

        std::cout << "\n=============================================================" << std::endl;
        std::cout << "BENCHMARKS COMPLETED" << std::endl;
        std::cout << "=============================================================" << std::endl;

        std::cout << "\nPython Comparison (estimated):" << std::endl;
        std::cout << "  - C++ implementation is typically 10-100x faster" << std::endl;
        std::cout << "  - Vectorized operations (Eigen3) provide significant speedup" << std::endl;
        std::cout << "  - Memory efficiency reduces cache misses" << std::endl;
        std::cout << "  - Direct computation avoids Python interpreter overhead" << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}