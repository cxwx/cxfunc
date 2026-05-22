/**
 * @file basic_example.cpp
 * @brief Basic examples demonstrating cxfunc math library usage
 */

#include <cxfunc/math.hpp>
#include <iostream>
#include <vector>

int main() {
    std::cout << "=== CxFunc Math Library - Basic Examples ===" << std::endl;
    std::cout << "Version: " << cxfunc::math::version() << std::endl;
    std::cout << std::endl;

    // ========================================
    // 1. Root Finding Examples
    // ========================================
    std::cout << "1. Root Finding Examples" << std::endl;
    std::cout << "-------------------------" << std::endl;

    // Simple function: f(x) = x^2 - 4
    auto f_quad = [](double x) { return x * x - 4.0; };

    // Bisection method
    std::cout << "Finding root of f(x) = x² - 4:" << std::endl;
    auto result_bisect = cxfunc::math::roots::bisection(f_quad, 0.0, 5.0);
    std::cout << "  Bisection: x = " << result_bisect.root
              << " (converged: " << result_bisect.converged
              << ", iterations: " << result_bisect.iterations << ")" << std::endl;

    // Brent's method
    auto result_brent = cxfunc::math::roots::brent(f_quad, 0.0, 5.0);
    std::cout << "  Brent:     x = " << result_brent.root
              << " (converged: " << result_brent.converged
              << ", iterations: " << result_brent.iterations << ")" << std::endl;

    // Secant method
    auto result_secant = cxfunc::math::roots::secant(f_quad, 0.0, 5.0);
    std::cout << "  Secant:    x = " << result_secant.root
              << " (converged: " << result_secant.converged
              << ", iterations: " << result_secant.iterations << ")" << std::endl;

    std::cout << std::endl;

    // ========================================
    // 2. Gaussian PDF Examples
    // ========================================
    std::cout << "2. Gaussian PDF Examples" << std::endl;
    std::cout << "-------------------------" << std::endl;

    // Create single Gaussian
    cxfunc::math::gauss::Gauss2DPDF gauss(0.5);  // sigma = 0.5 deg

    std::cout << "Gauss2DPDF with sigma = 0.5 deg:" << std::endl;
    std::cout << "  PDF at (1.0, 0.0): " << gauss(1.0, 0.0) << std::endl;
    std::cout << "  PDF at (0.5, 0.5): " << gauss(0.5, 0.5) << std::endl;
    std::cout << "  Containment radius (68%): " << gauss.containment_radius(0.68) << " deg" << std::endl;
    std::cout << "  Containment fraction at 1 deg: " << gauss.containment_fraction(1.0) << std::endl;

    // Convolution
    auto convolved = gauss.convolve(0.3);
    std::cout << "  Convolved with sigma=0.3: new sigma = " << convolved.sigma() << " deg" << std::endl;

    std::cout << std::endl;

    // Multi-Gaussian
    std::vector<double> sigmas = {0.1, 0.3, 1.0};  // core, halo, tail
    std::vector<double> norms = {0.7, 0.2, 0.1};   // relative weights

    cxfunc::math::gauss::MultiGauss2D multi(sigmas, norms);

    std::cout << "MultiGauss2D with 3 components:" << std::endl;
    std::cout << "  Effective sigma: " << multi.eff_sigma() << " deg" << std::endl;
    std::cout << "  PDF at (0, 0): " << multi.amplitude() << std::endl;
    std::cout << "  Containment radius (68%): " << multi.containment_radius(0.68) << " deg" << std::endl;
    std::cout << "  Containment fraction at 1 deg: " << multi.containment_fraction(1.0) << std::endl;

    std::cout << std::endl;

    // ========================================
    // 3. Interpolation Examples
    // ========================================
    std::cout << "3. Interpolation Examples" << std::endl;
    std::cout << "-------------------------" << std::endl;

    // Create energy axis (TeV)
    Eigen::ArrayXd energy(10);
    energy << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0;

    // Create flux values (power law: E^-2)
    Eigen::ArrayXd flux(10);
    flux = 1.0e-12 / energy.pow(2.0);  // ph cm^-2 s^-1 TeV^-1

    std::cout << "Power law spectrum (E^-2):" << std::endl;
    std::cout << "  Energy range: " << energy[0] << " - " << energy[energy.size()-1] << " TeV" << std::endl;
    std::cout << "  Flux at 1 TeV: " << flux[3] << " ph cm^-2 s^-1 TeV^-1" << std::endl;

    // Linear interpolation
    {
        std::vector<Eigen::ArrayXd> points = {energy};
        cxfunc::math::interpolation::ScaledRegularGridInterpolator interp_linear(
            points, flux,
            cxfunc::math::interpolation::Scale::Linear,
            cxfunc::math::interpolation::Scale::Linear);

        double e_interp = 3.16;  // sqrt(10) TeV
        double flux_interp = interp_linear.interpolate(e_interp);
        std::cout << "  Linear interpolation at " << e_interp << " TeV: "
                  << flux_interp << " ph cm^-2 s^-1 TeV^-1" << std::endl;
    }

    // Log-log interpolation (better for power laws)
    {
        std::vector<Eigen::ArrayXd> points = {energy};
        cxfunc::math::interpolation::ScaledRegularGridInterpolator interp_loglog(
            points, flux,
            cxfunc::math::interpolation::Scale::Log,
            cxfunc::math::interpolation::Scale::Log);

        double e_interp = 3.16;  // sqrt(10) TeV
        double flux_interp = interp_loglog.interpolate(e_interp);
        double expected = 1.0e-12 / std::pow(e_interp, 2.0);  // Exact value
        std::cout << "  Log-log interpolation at " << e_interp << " TeV: "
                  << flux_interp << " ph cm^-2 s^-1 TeV^-1" << std::endl;
        std::cout << "  Expected (exact): " << expected << " ph cm^-2 s^-1 TeV^-1" << std::endl;
        std::cout << "  Relative error: " << (flux_interp - expected) / expected * 100.0 << "%" << std::endl;
    }

    std::cout << std::endl;

    // ========================================
    // 4. Combined Example: PSF Containment
    // ========================================
    std::cout << "4. Combined Example: PSF Containment Analysis" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    // HESS-like PSF (multi-component)
    std::vector<double> psf_sigmas = {0.05, 0.15, 0.5};  // degrees
    std::vector<double> psf_norms = {0.6, 0.3, 0.1};

    cxfunc::math::gauss::MultiGauss2D psf(psf_sigmas, psf_norms);

    std::cout << "HESS-like PSF:" << std::endl;
    std::cout << "  Effective sigma: " << psf.eff_sigma() << " deg" << std::endl;

    // Calculate containment radii for different confidence levels
    std::vector<double> confidence_levels = {0.68, 0.95, 0.99};
    std::cout << "  Containment radii:" << std::endl;

    for (double conf : confidence_levels) {
        double radius = psf.containment_radius(conf);
        std::cout << "    " << int(conf * 100) << "%: " << radius << " deg" << std::endl;
    }

    std::cout << std::endl;

    return 0;
}