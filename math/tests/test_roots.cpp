/**
 * @file test_roots.cpp
 * @brief Unit tests for root finding algorithms
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

void test_bisection() {
    std::cout << "Testing bisection method..." << std::endl;

    // Test 1: Simple quadratic f(x) = x^2 - 4, root at x = 2
    {
        auto f = [](double x) { return x * x - 4.0; };
        auto result = roots::bisection(f, 0.0, 5.0);

        std::cout << "  f(x) = x² - 4 on [0, 5]: ";
        if (result.converged && approx_equal(result.root, 2.0, 1e-6)) {
            std::cout << "PASS (root = " << result.root << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected 2.0, got " << result.root << ")" << std::endl;
        }
    }

    // Test 2: Linear function f(x) = 2x - 1, root at x = 0.5
    {
        auto f = [](double x) { return 2.0 * x - 1.0; };
        auto result = roots::bisection(f, 0.0, 2.0);

        std::cout << "  f(x) = 2x - 1 on [0, 2]: ";
        if (result.converged && approx_equal(result.root, 0.5, 1e-6)) {
            std::cout << "PASS (root = " << result.root << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected 0.5, got " << result.root << ")" << std::endl;
        }
    }

    // Test 3: Negative root f(x) = x^2 - 9, root at x = -3
    {
        auto f = [](double x) { return x * x - 9.0; };
        auto result = roots::bisection(f, -5.0, 0.0);

        std::cout << "  f(x) = x² - 9 on [-5, 0]: ";
        if (result.converged && approx_equal(result.root, -3.0, 1e-6)) {
            std::cout << "PASS (root = " << result.root << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected -3.0, got " << result.root << ")" << std::endl;
        }
    }

    // Test 4: Trigonometric function f(x) = cos(x), root at x = π/2
    {
        auto f = [](double x) { return std::cos(x); };
        auto result = roots::bisection(f, 0.0, 2.0);

        double expected = M_PI / 2.0;
        std::cout << "  f(x) = cos(x) on [0, 2]: ";
        if (result.converged && approx_equal(result.root, expected, 1e-6)) {
            std::cout << "PASS (root = " << result.root << ", expected π/2 = " << expected << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected " << expected << ", got " << result.root << ")" << std::endl;
        }
    }
}

void test_brent() {
    std::cout << "Testing Brent's method..." << std::endl;

    // Test 1: Simple quadratic
    {
        auto f = [](double x) { return x * x - 4.0; };
        auto result = roots::brent(f, 0.0, 5.0);

        std::cout << "  f(x) = x² - 4 on [0, 5]: ";
        if (result.converged && approx_equal(result.root, 2.0, 1e-10)) {
            std::cout << "PASS (root = " << result.root << ", iterations: " << result.iterations << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected 2.0, got " << result.root << ")" << std::endl;
        }
    }

    // Test 2: Linear function
    {
        auto f = [](double x) { return 2.0 * x - 1.0; };
        auto result = roots::brent(f, 0.0, 2.0);

        std::cout << "  f(x) = 2x - 1 on [0, 2]: ";
        if (result.converged && approx_equal(result.root, 0.5, 1e-10)) {
            std::cout << "PASS (root = " << result.root << ", iterations: " << result.iterations << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected 0.5, got " << result.root << ")" << std::endl;
        }
    }

    // Test 3: Cubic function f(x) = x^3 - 2x - 5, root near x = 2
    {
        auto f = [](double x) { return x * x * x - 2.0 * x - 5.0; };
        auto result = roots::brent(f, 1.0, 3.0);

        std::cout << "  f(x) = x³ - 2x - 5 on [1, 3]: ";
        if (result.converged && result.root > 2.0 && result.root < 2.5) {
            std::cout << "PASS (root = " << std::setprecision(10) << result.root << ", iterations: " << result.iterations << ")" << std::endl;
        } else {
            std::cout << "FAIL (got " << result.root << ")" << std::endl;
        }
    }
}

void test_secant() {
    std::cout << "Testing secant method..." << std::endl;

    // Test 1: Simple quadratic
    {
        auto f = [](double x) { return x * x - 4.0; };
        auto result = roots::secant(f, 0.0, 5.0);

        std::cout << "  f(x) = x² - 4 with x0=0, x1=5: ";
        if (result.converged && approx_equal(result.root, 2.0, 1e-8)) {
            std::cout << "PASS (root = " << result.root << ", iterations: " << result.iterations << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected 2.0, got " << result.root << ")" << std::endl;
        }
    }

    // Test 2: Exponential function f(x) = exp(x) - 2, root at x = ln(2)
    {
        auto f = [](double x) { return std::exp(x) - 2.0; };
        auto result = roots::secant(f, 0.0, 2.0);

        double expected = std::log(2.0);
        std::cout << "  f(x) = exp(x) - 2 with x0=0, x1=2: ";
        if (result.converged && approx_equal(result.root, expected, 1e-8)) {
            std::cout << "PASS (root = " << result.root << ", expected ln(2) = " << expected << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected " << expected << ", got " << result.root << ")" << std::endl;
        }
    }

    // Test 3: Square root f(x) = x^2 - 2, root at x = sqrt(2)
    {
        auto f = [](double x) { return x * x - 2.0; };
        auto result = roots::secant(f, 1.0, 2.0);

        double expected = std::sqrt(2.0);
        std::cout << "  f(x) = x² - 2 with x0=1, x1=2: ";
        if (result.converged && approx_equal(result.root, expected, 1e-8)) {
            std::cout << "PASS (root = " << result.root << ", expected √2 = " << expected << ")" << std::endl;
        } else {
            std::cout << "FAIL (expected " << expected << ", got " << result.root << ")" << std::endl;
        }
    }
}

void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;

    // Test 1: Interval that doesn't bracket a root
    {
        auto f = [](double x) { return x * x + 1.0; };  // Always positive
        try {
            auto result = roots::bisection(f, 0.0, 5.0);
            std::cout << "  Non-bracketing interval: FAIL (should have thrown exception)" << std::endl;
        } catch (const std::invalid_argument&) {
            std::cout << "  Non-bracketing interval: PASS (exception thrown)" << std::endl;
        }
    }

    // Test 2: Function returning NaN
    {
        auto f = [](double x) { return std::sqrt(-1.0); };  // NaN
        try {
            auto result = roots::brent(f, 0.0, 5.0);
            std::cout << "  NaN function: FAIL (should have thrown exception)" << std::endl;
        } catch (const std::invalid_argument&) {
            std::cout << "  NaN function: PASS (exception thrown)" << std::endl;
        }
    }

    // Test 3: find_root with fallback to secant
    {
        auto f = [](double x) { return x * x - 4.0; };
        // Interval that brackets root
        auto result1 = roots::find_root(f, 0.0, 5.0);
        std::cout << "  find_root (bracketing): ";
        if (result1.converged && approx_equal(result1.root, 2.0, 1e-6)) {
            std::cout << "PASS" << std::endl;
        } else {
            std::cout << "FAIL" << std::endl;
        }

        // Interval that doesn't bracket (falls back to secant)
        auto result2 = roots::find_root(f, 3.0, 10.0);
        std::cout << "  find_root (non-bracketing): ";
        if (result2.converged && approx_equal(result2.root, 2.0, 1e-3)) {
            std::cout << "PASS (secant fallback)" << std::endl;
        } else {
            std::cout << "FAIL" << std::endl;
        }
    }
}

int main() {
    std::cout << "=== CxFunc Math Library - Root Finding Tests ===" << std::endl;
    std::cout << "Version: " << cxfunc::math::version() << std::endl;
    std::cout << std::endl;

    test_bisection();
    std::cout << std::endl;

    test_brent();
    std::cout << std::endl;

    test_secant();
    std::cout << std::endl;

    test_error_handling();
    std::cout << std::endl;

    std::cout << "=== All Tests Complete ===" << std::endl;

    return 0;
}