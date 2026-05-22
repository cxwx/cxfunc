/**
 * @file test_interpolation.cpp
 * @brief Unit tests for interpolation functions
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

void test_scale_transformations() {
    std::cout << "Testing scale transformations..." << std::endl;

    // Test 1: Linear scale (identity)
    std::cout << "  Linear scale: ";
    double val = 5.0;
    double scaled = interpolation::apply_scale(val, interpolation::Scale::Linear);
    double unscaled = interpolation::apply_inverse_scale(scaled, interpolation::Scale::Linear);
    if (approx_equal(val, unscaled, 1e-15)) {
        std::cout << "PASS (identity)" << std::endl;
    } else {
        std::cout << "FAIL (round-trip failed)" << std::endl;
    }

    // Test 2: Log scale
    std::cout << "  Log scale: ";
    double val_log = 10.0;
    double scaled_log = interpolation::apply_scale(val_log, interpolation::Scale::Log);
    double unscaled_log = interpolation::apply_inverse_scale(scaled_log, interpolation::Scale::Log);
    if (approx_equal(val_log, unscaled_log, 1e-10)) {
        std::cout << "PASS (log(10) = " << scaled_log << ", exp(log) = " << unscaled_log << ")" << std::endl;
    } else {
        std::cout << "FAIL (round-trip failed)" << std::endl;
    }

    // Test 3: Sqrt scale
    std::cout << "  Sqrt scale: ";
    double val_sqrt = 16.0;
    double scaled_sqrt = interpolation::apply_scale(val_sqrt, interpolation::Scale::Sqrt);
    double unscaled_sqrt = interpolation::apply_inverse_scale(scaled_sqrt, interpolation::Scale::Sqrt);
    if (approx_equal(val_sqrt, unscaled_sqrt, 1e-10)) {
        std::cout << "PASS (sqrt(16) = " << scaled_sqrt << ", (sqrt)² = " << unscaled_sqrt << ")" << std::endl;
    } else {
        std::cout << "FAIL (round-trip failed)" << std::endl;
    }

    // Test 4: Array scaling
    std::cout << "  Array log scale: ";
    Eigen::ArrayXd values(5);
    values << 1.0, 2.0, 5.0, 10.0, 20.0;
    auto scaled_values = interpolation::apply_scale(values, interpolation::Scale::Log);
    auto unscaled_values = interpolation::apply_inverse_scale(scaled_values, interpolation::Scale::Log);

    bool all_pass = true;
    for (int i = 0; i < values.size(); ++i) {
        if (!approx_equal(values[i], unscaled_values[i], 1e-10)) {
            all_pass = false;
            break;
        }
    }
    if (all_pass) {
        std::cout << "PASS (array round-trip successful)" << std::endl;
    } else {
        std::cout << "FAIL (array round-trip failed)" << std::endl;
    }
}

void test_interpolate_1d() {
    std::cout << "Testing 1D interpolation..." << std::endl;

    // Create test data: y = x^2
    Eigen::ArrayXd x(5);
    Eigen::ArrayXd y(5);
    x << 0.0, 1.0, 2.0, 3.0, 4.0;
    y << 0.0, 1.0, 4.0, 9.0, 16.0;

    // Test 1: Interpolation at grid point
    std::cout << "  Interpolation at grid point (x=2): ";
    double y_at_2 = interpolation::interpolate_1d(x, y, 2.0);
    if (approx_equal(y_at_2, 4.0, 1e-10)) {
        std::cout << "PASS (y = " << y_at_2 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected 4.0, got " << y_at_2 << ")" << std::endl;
    }

    // Test 2: Linear interpolation between points
    std::cout << "  Linear interpolation (x=1.5): ";
    double y_at_1_5 = interpolation::interpolate_1d(x, y, 1.5);
    double expected = 2.5;  // Midpoint between 1 and 4
    if (approx_equal(y_at_1_5, expected, 1e-10)) {
        std::cout << "PASS (y = " << y_at_1_5 << " ≈ " << expected << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected << ", got " << y_at_1_5 << ")" << std::endl;
    }

    // Test 3: Extrapolation (use edge values)
    std::cout << "  Extrapolation below range (x=-1): ";
    double y_at_minus1 = interpolation::interpolate_1d(x, y, -1.0);
    if (approx_equal(y_at_minus1, 0.0, 1e-10)) {
        std::cout << "PASS (y = " << y_at_minus1 << " = first point)" << std::endl;
    } else {
        std::cout << "FAIL (expected 0.0, got " << y_at_minus1 << ")" << std::endl;
    }

    // Test 4: Extrapolation above range
    std::cout << "  Extrapolation above range (x=5): ";
    double y_at_5 = interpolation::interpolate_1d(x, y, 5.0);
    if (approx_equal(y_at_5, 16.0, 1e-10)) {
        std::cout << "PASS (y = " << y_at_5 << " = last point)" << std::endl;
    } else {
        std::cout << "FAIL (expected 16.0, got " << y_at_5 << ")" << std::endl;
    }
}

void test_scaled_interpolator_1d() {
    std::cout << "Testing ScaledRegularGridInterpolator (1D)..." << std::endl;

    // Power law: y = 1/x^2 (log-log scale)
    Eigen::ArrayXd energy(6);
    Eigen::ArrayXd flux(6);
    energy << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0;  // TeV
    flux = 1.0e-12 / energy.pow(2.0);  // E^-2 spectrum

    // Test 1: Linear interpolation
    std::cout << "  Linear interpolation at 1.5 TeV: ";
    std::vector<Eigen::ArrayXd> points = {energy};
    interpolation::ScaledRegularGridInterpolator interp_linear(
        points, flux,
        interpolation::Scale::Linear,
        interpolation::Scale::Linear);

    double flux_linear = interp_linear.interpolate(1.5);
    // Linear interpolation between 1.0 and 2.0 TeV
    double expected_linear = 1.0e-12 / (1.0 * 1.0) * 0.5 + 1.0e-12 / (2.0 * 2.0) * 0.5;
    if (approx_equal(flux_linear, expected_linear, 1e-15)) {
        std::cout << "PASS (" << flux_linear << " ≈ " << expected_linear << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_linear << ", got " << flux_linear << ")" << std::endl;
    }

    // Test 2: Log-log interpolation (better for power laws)
    std::cout << "  Log-log interpolation at 1.5 TeV: ";
    interpolation::ScaledRegularGridInterpolator interp_loglog(
        points, flux,
        interpolation::Scale::Log,
        interpolation::Scale::Log);

    double flux_loglog = interp_loglog.interpolate(1.5);
    double expected_exact = 1.0e-12 / (1.5 * 1.5);
    double rel_error = std::abs((flux_loglog - expected_exact) / expected_exact);

    if (rel_error < 0.01) {  // Less than 1% error
        std::cout << "PASS (" << flux_loglog << " ≈ " << expected_exact
                  << ", error = " << rel_error * 100 << "%)" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_exact << ", got " << flux_loglog
                  << ", error = " << rel_error * 100 << "%)" << std::endl;
    }

    // Test 3: Interpolation at grid points should be exact
    std::cout << "  Interpolation at grid points: ";
    bool all_exact = true;
    for (int i = 0; i < energy.size(); ++i) {
        double interp_val = interp_loglog.interpolate(energy[i]);
        if (!approx_equal(interp_val, flux[i], 1e-10)) {
            all_exact = false;
            std::cout << "FAIL (at index " << i << ": expected " << flux[i]
                      << ", got " << interp_val << ")" << std::endl;
            break;
        }
    }
    if (all_exact) {
        std::cout << "PASS (all grid points exact)" << std::endl;
    }

    // Test 4: Monotonicity check
    std::cout << "  Monotonicity check (flux decreases with energy): ";
    bool is_monotonic = true;
    for (int i = 1; i < 10; ++i) {
        double e = 0.1 + i * 0.5;  // 0.6 to 5.0 TeV
        double f1 = interp_loglog.interpolate(e);
        double f2 = interp_loglog.interpolate(e + 0.1);
        if (f2 > f1) {  // Flux should decrease with energy
            is_monotonic = false;
            std::cout << "FAIL (flux increased from " << f1 << " to " << f2
                      << " between " << e << " and " << e + 0.1 << " TeV)" << std::endl;
            break;
        }
    }
    if (is_monotonic) {
        std::cout << "PASS (flux is monotonically decreasing)" << std::endl;
    }
}

void test_scaled_interpolator_2d() {
    std::cout << "Testing ScaledRegularGridInterpolator (2D)..." << std::endl;

    // Create 2D grid: f(x, y) = x * y
    Eigen::ArrayXd x(3);
    Eigen::ArrayXd y(4);
    x << 1.0, 2.0, 3.0;
    y << 1.0, 2.0, 3.0, 4.0;

    // Create 2D values
    Eigen::ArrayXd values(12);  // 3 * 4
    int idx = 0;
    for (int i = 0; i < x.size(); ++i) {
        for (int j = 0; j < y.size(); ++j) {
            values[idx++] = x[i] * y[j];
        }
    }

    std::vector<Eigen::ArrayXd> points = {x, y};
    interpolation::ScaledRegularGridInterpolator interp_2d(
        points, values,
        interpolation::Scale::Linear,
        interpolation::Scale::Linear);

    // Test 1: Interpolation at grid point
    std::cout << "  Interpolation at grid point (x=2, y=3): ";
    double val_grid = interp_2d.interpolate(2.0, 3.0);
    double expected_grid = 6.0;  // 2 * 3
    if (approx_equal(val_grid, expected_grid, 1e-10)) {
        std::cout << "PASS (" << val_grid << " = " << expected_grid << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_grid << ", got " << val_grid << ")" << std::endl;
    }

    // Test 2: Interpolation between points
    std::cout << "  Interpolation between points (x=1.5, y=2.5): ";
    double val_interp = interp_2d.interpolate(1.5, 2.5);
    double expected_interp = 1.5 * 2.5;  // Should be exact for bilinear interpolation
    if (approx_equal(val_interp, expected_interp, 1e-10)) {
        std::cout << "PASS (" << val_interp << " ≈ " << expected_interp << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_interp << ", got " << val_interp << ")" << std::endl;
    }

    // Test 3: Edge cases
    std::cout << "  Edge case (x=1, y=4): ";
    double val_edge = interp_2d.interpolate(1.0, 4.0);
    double expected_edge = 4.0;  // 1 * 4
    if (approx_equal(val_edge, expected_edge, 1e-10)) {
        std::cout << "PASS (" << val_edge << " = " << expected_edge << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_edge << ", got " << val_edge << ")" << std::endl;
    }
}

void test_astronomy_application() {
    std::cout << "Testing astronomy application (flux point interpolation)..." << std::endl;

    // Simulate flux points with errors
    Eigen::ArrayXd energy(8);
    Eigen::ArrayXd dnde(8);
    Eigen::ArrayXd dnde_errp(8);
    Eigen::ArrayXd dnde_errn(8);

    // Typical gamma-ray spectrum
    energy << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0;  // TeV
    dnde = 1.0e-12 / energy.pow(2.0);  // E^-2 spectrum
    dnde_errp = dnde * 0.1;  // 10% positive errors
    dnde_errn = dnde * 0.15;  // 15% negative errors

    // Test interpolation at intermediate energy
    double target_energy = 3.16;  // sqrt(10) TeV
    std::cout << "  Interpolating flux at " << target_energy << " TeV: ";

    double flux_interp = interpolation::interpolate_profile(
        energy, dnde, dnde_errp, dnde_errn, target_energy,
        interpolation::Scale::Log);

    double expected = 1.0e-12 / (target_energy * target_energy);
    double rel_error = std::abs((flux_interp - expected) / expected);

    if (rel_error < 0.05) {  // Less than 5% error
        std::cout << "PASS (" << flux_interp << " ≈ " << expected
                  << ", error = " << rel_error * 100 << "%)" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected << ", got " << flux_interp
                  << ", error = " << rel_error * 100 << "%)" << std::endl;
    }

    // Test multiple interpolations
    std::cout << "  Multiple interpolations (energy range): ";
    bool all_good = true;
    std::vector<double> test_energies = {0.15, 0.7, 1.5, 3.0, 7.0, 15.0};
    for (double e : test_energies) {
        double f = interpolation::interpolate_profile(
            energy, dnde, dnde_errp, dnde_errn, e,
            interpolation::Scale::Log);
        double expected_f = 1.0e-12 / (e * e);
        double rel_err = std::abs((f - expected_f) / expected_f);

        if (rel_err > 0.1) {  // More than 10% error
            all_good = false;
            std::cout << "FAIL (at E=" << e << " TeV: error=" << rel_err * 100 << "%)" << std::endl;
            break;
        }
    }
    if (all_good) {
        std::cout << "PASS (all " << test_energies.size() << " points interpolated successfully)" << std::endl;
    }
}

void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;

    // Test 1: Mismatched array sizes
    std::cout << "  Mismatched array sizes: ";
    try {
        Eigen::ArrayXd x(5);
        Eigen::ArrayXd y(3);
        interpolation::interpolate_1d(x, y, 1.0);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 2: Non-monotonic points
    std::cout << "  Non-monotonic points: ";
    try {
        Eigen::ArrayXd x(3);
        Eigen::ArrayXd y(3);
        x << 1.0, 3.0, 2.0;  // Not monotonic
        y << 1.0, 2.0, 3.0;
        interpolation::interpolate_1d(x, y, 1.5);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 3: Log scale with negative values
    std::cout << "  Log scale with negative values: ";
    try {
        double result = interpolation::apply_scale(-1.0, interpolation::Scale::Log);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 4: Sqrt scale with negative values
    std::cout << "  Sqrt scale with negative values: ";
    try {
        double result = interpolation::apply_scale(-1.0, interpolation::Scale::Sqrt);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 5: Empty points vector
    std::cout << "  Empty points vector: ";
    try {
        std::vector<Eigen::ArrayXd> empty_points;
        Eigen::ArrayXd values(10);
        interpolation::ScaledRegularGridInterpolator interp(empty_points, values);
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 6: Wrong number of coordinates
    std::cout << "  Wrong number of coordinates: ";
    try {
        Eigen::ArrayXd x(5);
        Eigen::ArrayXd y(5);
        std::vector<Eigen::ArrayXd> points = {x};
        interpolation::ScaledRegularGridInterpolator interp(points, y);
        interp.interpolate(1.0, 2.0);  // 2 coordinates for 1D interpolator
        std::cout << "FAIL (should have thrown exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }
}

int main() {
    std::cout << "=== CxFunc Math Library - Interpolation Tests ===" << std::endl;
    std::cout << "Version: " << cxfunc::math::version() << std::endl;
    std::cout << std::endl;

    test_scale_transformations();
    std::cout << std::endl;

    test_interpolate_1d();
    std::cout << std::endl;

    test_scaled_interpolator_1d();
    std::cout << std::endl;

    test_scaled_interpolator_2d();
    std::cout << std::endl;

    test_astronomy_application();
    std::cout << std::endl;

    test_error_handling();
    std::cout << std::endl;

    std::cout << "=== All Tests Complete ===" << std::endl;

    return 0;
}