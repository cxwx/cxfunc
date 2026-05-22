/**
 * @file test_gauss.cpp
 * @brief Unit tests for Gaussian PDF classes
 */

#include <cxfunc/math.hpp>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace cxfunc::math;

// Test helper functions
bool approx_equal(double a, double b, double tol = 1e-10) {
    return std::abs(a - b) < tol;
}

void test_gauss2d_basic() {
    std::cout << "Testing Gauss2DPDF basic functionality..." << std::endl;

    // Create Gaussian with sigma = 1.0
    gauss::Gauss2DPDF gauss(1.0);

    // Test 1: Value at center
    std::cout << "  Value at center (0, 0): ";
    double center_value = gauss(0.0, 0.0);
    double expected_center = 1.0 / (2.0 * M_PI);  // 1/(2π*1²)
    if (approx_equal(center_value, expected_center, 1e-10)) {
        std::cout << "PASS (" << center_value << " ≈ " << expected_center << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_center << ", got " << center_value << ")" << std::endl;
    }

    // Test 2: Value at (1, 0)
    std::cout << "  Value at (1, 0): ";
    double value_1_0 = gauss(1.0, 0.0);
    double expected_1_0 = expected_center * std::exp(-0.5);  // exp(-1²/(2*1²))
    if (approx_equal(value_1_0, expected_1_0, 1e-10)) {
        std::cout << "PASS (" << value_1_0 << " ≈ " << expected_1_0 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_1_0 << ", got " << value_1_0 << ")" << std::endl;
    }

    // Test 3: Value at (1, 1) (distance = sqrt(2))
    std::cout << "  Value at (1, 1): ";
    double value_1_1 = gauss(1.0, 1.0);
    double theta2 = 2.0;  // 1² + 1²
    double expected_1_1 = expected_center * std::exp(-theta2 / 2.0);
    if (approx_equal(value_1_1, expected_1_1, 1e-10)) {
        std::cout << "PASS (" << value_1_1 << " ≈ " << expected_1_1 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_1_1 << ", got " << value_1_1 << ")" << std::endl;
    }

    // Test 4: Symmetry
    std::cout << "  Symmetry check: ";
    double val_pos = gauss(1.0, 0.5);
    double val_neg = gauss(-1.0, -0.5);
    if (approx_equal(val_pos, val_neg, 1e-15)) {
        std::cout << "PASS (gauss(1, 0.5) = gauss(-1, -0.5) = " << val_pos << ")" << std::endl;
    } else {
        std::cout << "FAIL (gauss(1, 0.5) != gauss(-1, -0.5))" << std::endl;
    }
}

void test_gauss2d_containment() {
    std::cout << "Testing Gauss2DPDF containment..." << std::endl;

    gauss::Gauss2DPDF gauss(1.0);

    // Test 1: Containment radius for 68% (≈1 sigma for 2D)
    std::cout << "  Containment radius (68%): ";
    double r68 = gauss.containment_radius(0.68);
    // For 2D Gaussian, 68% containment is approximately 1.51 sigma
    if (r68 > 1.4 && r68 < 1.6) {
        double frac_back = gauss.containment_fraction(r68);
        std::cout << "PASS (radius = " << r68 << " sigma, fraction = " << frac_back << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected ~1.5 sigma, got " << r68 << ")" << std::endl;
    }

    // Test 2: Containment radius for 95%
    std::cout << "  Containment radius (95%): ";
    double r95 = gauss.containment_radius(0.95);
    // For 2D Gaussian, 95% containment is approximately 2.45 sigma
    if (r95 > 2.3 && r95 < 2.6) {
        double frac_back = gauss.containment_fraction(r95);
        std::cout << "PASS (radius = " << r95 << " sigma, fraction = " << frac_back << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected ~2.45 sigma, got " << r95 << ")" << std::endl;
    }

    // Test 3: Round-trip consistency
    std::cout << "  Round-trip consistency: ";
    std::vector<double> fractions = {0.5, 0.68, 0.95, 0.99};
    bool all_pass = true;
    for (double frac : fractions) {
        double radius = gauss.containment_radius(frac);
        double frac_back = gauss.containment_fraction(radius);
        if (!approx_equal(frac, frac_back, 1e-6)) {
            std::cout << "FAIL (frac=" << frac << ", radius=" << radius
                      << ", frac_back=" << frac_back << ")" << std::endl;
            all_pass = false;
            break;
        }
    }
    if (all_pass) {
        std::cout << "PASS (all round-trips consistent)" << std::endl;
    }

    // Test 4: Containment fraction increases with radius
    std::cout << "  Monotonicity check: ";
    double f1 = gauss.containment_fraction(0.5);
    double f2 = gauss.containment_fraction(1.0);
    double f3 = gauss.containment_fraction(2.0);
    if (f1 < f2 && f2 < f3 && f1 < f3) {
        std::cout << "PASS (f(0.5)=" << f1 << " < f(1.0)=" << f2 << " < f(2.0)=" << f3 << ")" << std::endl;
    } else {
        std::cout << "FAIL (containment fraction not monotonically increasing)" << std::endl;
    }
}

void test_gauss2d_convolution() {
    std::cout << "Testing Gauss2DPDF convolution..." << std::endl;

    gauss::Gauss2DPDF gauss1(1.0);

    // Test 1: Convolve with smaller Gaussian
    std::cout << "  Convolution sigma=1.0 ⊗ sigma=0.5: ";
    auto convolved1 = gauss1.convolve(0.5);
    double expected_sigma1 = std::sqrt(1.0 * 1.0 + 0.5 * 0.5);  // sqrt(1² + 0.5²)
    if (approx_equal(convolved1.sigma(), expected_sigma1, 1e-10)) {
        std::cout << "PASS (new sigma = " << convolved1.sigma() << " ≈ " << expected_sigma1 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_sigma1 << ", got " << convolved1.sigma() << ")" << std::endl;
    }

    // Test 2: Convolve with equal Gaussian
    std::cout << "  Convolution sigma=1.0 ⊗ sigma=1.0: ";
    auto convolved2 = gauss1.convolve(1.0);
    double expected_sigma2 = std::sqrt(2.0);  // sqrt(1² + 1²)
    if (approx_equal(convolved2.sigma(), expected_sigma2, 1e-10)) {
        std::cout << "PASS (new sigma = " << convolved2.sigma() << " ≈ " << expected_sigma2 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_sigma2 << ", got " << convolved2.sigma() << ")" << std::endl;
    }

    // Test 3: Multiple convolutions
    std::cout << "  Multiple convolutions: ";
    auto g1 = gauss1.convolve(0.5);
    auto g2 = g1.convolve(0.5);
    auto g3 = g2.convolve(0.5);
    double expected_sigma3 = std::sqrt(1.0 * 1.0 + 3 * 0.5 * 0.5);  // sqrt(1² + 3*0.5²)
    if (approx_equal(g3.sigma(), expected_sigma3, 1e-10)) {
        std::cout << "PASS (sigma = " << g3.sigma() << " ≈ " << expected_sigma3 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_sigma3 << ", got " << g3.sigma() << ")" << std::endl;
    }
}

void test_multigauss2d_basic() {
    std::cout << "Testing MultiGauss2D basic functionality..." << std::endl;

    // Create multi-Gaussian with 3 components
    std::vector<double> sigmas = {0.1, 0.5, 2.0};
    std::vector<double> norms = {0.6, 0.3, 0.1};

    gauss::MultiGauss2D multi(sigmas, norms);

    // Test 1: Number of components
    std::cout << "  Number of components: ";
    if (multi.n_components() == 3) {
        std::cout << "PASS (3 components)" << std::endl;
    } else {
        std::cout << "FAIL (expected 3, got " << multi.n_components() << ")" << std::endl;
    }

    // Test 2: Value at center
    std::cout << "  Value at center: ";
    double center_val = multi(0.0, 0.0);
    // Calculate expected: sum of individual amplitudes weighted by norms
    double expected = 0.0;
    for (size_t i = 0; i < sigmas.size(); ++i) {
        gauss::Gauss2DPDF g(sigmas[i]);
        expected += norms[i] * g(0.0, 0.0);
    }
    if (approx_equal(center_val, expected, 1e-10)) {
        std::cout << "PASS (" << center_val << " ≈ " << expected << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected << ", got " << center_val << ")" << std::endl;
    }

    // Test 3: Total integral
    std::cout << "  Total integral: ";
    double integral = multi.total_integral();
    double expected_integral = 0.0;
    for (auto norm : norms) {
        expected_integral += norm;
    }
    if (approx_equal(integral, expected_integral, 1e-10)) {
        std::cout << "PASS (" << integral << " ≈ " << expected_integral << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_integral << ", got " << integral << ")" << std::endl;
    }

    // Test 4: Effective sigma
    std::cout << "  Effective sigma: ";
    double eff_sigma = multi.eff_sigma();
    // Expected: sqrt(sum(norms[i] * sigmas[i]^2))
    double expected_eff = 0.0;
    for (size_t i = 0; i < sigmas.size(); ++i) {
        expected_eff += norms[i] * sigmas[i] * sigmas[i];
    }
    expected_eff = std::sqrt(expected_eff);
    if (approx_equal(eff_sigma, expected_eff, 1e-10)) {
        std::cout << "PASS (" << eff_sigma << " ≈ " << expected_eff << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_eff << ", got " << eff_sigma << ")" << std::endl;
    }
}

void test_multigauss2d_containment() {
    std::cout << "Testing MultiGauss2D containment..." << std::endl;

    // HESS-like PSF
    std::vector<double> sigmas = {0.05, 0.15, 0.5};
    std::vector<double> norms = {0.6, 0.3, 0.1};

    gauss::MultiGauss2D psf(sigmas, norms);

    // Test 1: Containment radius for 68%
    std::cout << "  Containment radius (68%): ";
    double r68 = psf.containment_radius(0.68);
    double frac_back = psf.containment_fraction(r68);
    if (std::abs(frac_back - 0.68) < 0.01 && r68 > 0 && r68 < 1.0) {
        std::cout << "PASS (radius = " << r68 << " deg, fraction = " << frac_back << ")" << std::endl;
    } else {
        std::cout << "FAIL (radius = " << r68 << ", fraction = " << frac_back << ")" << std::endl;
    }

    // Test 2: Containment increases with radius
    std::cout << "  Monotonicity check: ";
    double f1 = psf.containment_fraction(0.1);
    double f2 = psf.containment_fraction(0.3);
    double f3 = psf.containment_fraction(0.6);
    if (f1 < f2 && f2 < f3) {
        std::cout << "PASS (f(0.1)=" << f1 << " < f(0.3)=" << f2 << " < f(0.6)=" << f3 << ")" << std::endl;
    } else {
        std::cout << "FAIL (not monotonically increasing)" << std::endl;
    }
}

void test_multigauss2d_convolution() {
    std::cout << "Testing MultiGauss2D convolution..." << std::endl;

    std::vector<double> sigmas = {0.1, 0.3};
    std::vector<double> norms = {0.7, 0.3};

    gauss::MultiGauss2D multi(sigmas, norms);

    // Test 1: Convolution preserves component count
    std::cout << "  Component count preservation: ";
    auto convolved = multi.convolve(0.2);
    if (convolved.n_components() == multi.n_components()) {
        std::cout << "PASS (" << convolved.n_components() << " components)" << std::endl;
    } else {
        std::cout << "FAIL (expected " << multi.n_components()
                  << ", got " << convolved.n_components() << ")" << std::endl;
    }

    // Test 2: Convolution increases effective sigma
    std::cout << "  Effective sigma increase: ";
    double eff_before = multi.eff_sigma();
    double eff_after = convolved.eff_sigma();
    if (eff_after > eff_before) {
        std::cout << "PASS (" << eff_before << " → " << eff_after << ")" << std::endl;
    } else {
        std::cout << "FAIL (effective sigma decreased)" << std::endl;
    }

    // Test 3: Convolution with zero sigma doesn't change anything
    std::cout << "  Zero sigma convolution: ";
    auto no_change = multi.convolve(0.0);
    if (approx_equal(no_change.eff_sigma(), multi.eff_sigma(), 1e-10)) {
        std::cout << "PASS (sigma unchanged)" << std::endl;
    } else {
        std::cout << "FAIL (sigma changed)" << std::endl;
    }
}

void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;

    // Test 1: Invalid sigma for Gauss2DPDF
    std::cout << "  Invalid sigma for Gauss2DPDF: ";
    try {
        gauss::Gauss2DPDF g(-1.0);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 2: Invalid containment fraction for Gauss2DPDF
    std::cout << "  Invalid containment fraction: ";
    gauss::Gauss2DPDF g(1.0);
    try {
        double r = g.containment_radius(1.5);  // > 1.0
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown for fraction > 1)" << std::endl;
    }

    // Test 3: Empty sigmas for MultiGauss2D
    std::cout << "  Empty sigmas for MultiGauss2D: ";
    try {
        gauss::MultiGauss2D m({}, {});
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 4: Mismatched sizes for MultiGauss2D
    std::cout << "  Mismatched sizes for MultiGauss2D: ";
    try {
        std::vector<double> s = {0.1, 0.5};
        std::vector<double> n = {0.5};  // Different size
        gauss::MultiGauss2D m(s, n);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }
}

int main() {
    std::cout << "=== CxFunc Math Library - Gaussian PDF Tests ===" << std::endl;
    std::cout << "Version: " << cxfunc::math::version() << std::endl;
    std::cout << std::endl;

    test_gauss2d_basic();
    std::cout << std::endl;

    test_gauss2d_containment();
    std::cout << std::endl;

    test_gauss2d_convolution();
    std::cout << std::endl;

    test_multigauss2d_basic();
    std::cout << std::endl;

    test_multigauss2d_containment();
    std::cout << std::endl;

    test_multigauss2d_convolution();
    std::cout << std::endl;

    test_error_handling();
    std::cout << std::endl;

    std::cout << "=== All Tests Complete ===" << std::endl;

    return 0;
}