/**
 * @file test_spectral.cpp
 * @brief Unit tests for spectral models
 */

#include <cxfunc/modeling/spectral.hpp>
#include <iostream>
#include <cassert>
#include <cmath>

using namespace cxfunc::modeling::spectral;

// Tolerance for floating point comparisons
constexpr double TOLERANCE = 1e-10;

void test_power_law() {
    std::cout << "Testing PowerLaw..." << std::endl;

    // Create power law model: amplitude=1e-12, index=2.0, reference=1.0 TeV
    PowerLaw pl(1e-12, 2.0, 1.0);

    // Test at reference energy (should return amplitude)
    double flux_at_ref = pl.evaluate(1.0);
    assert(std::abs(flux_at_ref - 1e-12) < TOLERANCE);

    // Test at different energy: E = 10 TeV
    // flux = amplitude * (E/E_ref)^(-index) = 1e-12 * 10^(-2) = 1e-14
    double flux_at_10 = pl.evaluate(10.0);
    assert(std::abs(flux_at_10 - 1e-14) < TOLERANCE * 10);

    // Test at lower energy: E = 0.1 TeV
    // flux = 1e-12 * 0.1^(-2) = 1e-12 * 100 = 1e-10
    double flux_at_01 = pl.evaluate(0.1);
    assert(std::abs(flux_at_01 - 1e-10) < TOLERANCE * 100);

    // Test at non-positive energy (should return 0)
    double flux_zero = pl.evaluate(0.0);
    assert(flux_zero == 0.0);

    double flux_negative = pl.evaluate(-1.0);
    assert(flux_negative == 0.0);

    // Test getters
    assert(std::abs(pl.amplitude() - 1e-12) < TOLERANCE);
    assert(std::abs(pl.index() - 2.0) < TOLERANCE);
    assert(std::abs(pl.reference() - 1.0) < TOLERANCE);

    // Test type and description
    assert(pl.type() == "PowerLaw");
    assert(pl.description().find("Power Law") != std::string::npos);

    // Test array evaluation
    Eigen::ArrayXd energies(3);
    energies << 0.1, 1.0, 10.0;
    Eigen::ArrayXd fluxes = pl.evaluate(energies);
    assert(std::abs(fluxes[0] - 1e-10) < TOLERANCE * 100);
    assert(std::abs(fluxes[1] - 1e-12) < TOLERANCE);
    assert(std::abs(fluxes[2] - 1e-14) < TOLERANCE * 10);

    std::cout << "  PowerLaw tests passed!" << std::endl;
}

void test_exp_cutoff_power_law() {
    std::cout << "Testing ExpCutoffPowerLaw..." << std::endl;

    // Create model: amplitude=1e-12, index=2.0, reference=1.0 TeV, cutoff=10.0 TeV
    ExpCutoffPowerLaw ec_pl(1e-12, 2.0, 1.0, 10.0);

    // Test at reference energy
    // flux = amplitude * (1.0/1.0)^(-2) * exp(-1.0/10) = 1e-12 * 1.0 * exp(-0.1)
    double flux_at_ref = ec_pl.evaluate(1.0);
    double expected = 1e-12 * std::exp(-0.1);
    assert(std::abs(flux_at_ref - expected) < TOLERANCE);

    // Test at cutoff energy
    // flux = 1e-12 * 10^(-2) * exp(-1) = 1e-14 * exp(-1)
    double flux_at_cutoff = ec_pl.evaluate(10.0);
    expected = 1e-14 * std::exp(-1.0);
    assert(std::abs(flux_at_cutoff - expected) < TOLERANCE * 10);

    // Test at high energy (strong suppression)
    double flux_high = ec_pl.evaluate(100.0);
    assert(flux_high < 1e-20);  // Should be very small

    // Test at non-positive energy
    double flux_zero = ec_pl.evaluate(0.0);
    assert(flux_zero == 0.0);

    // Test getters
    assert(std::abs(ec_pl.cutoff() - 10.0) < TOLERANCE);

    // Test type
    assert(ec_pl.type() == "ExpCutoffPowerLaw");

    std::cout << "  ExpCutoffPowerLaw tests passed!" << std::endl;
}

void test_log_parabola() {
    std::cout << "Testing LogParabola..." << std::endl;

    // Create model: amplitude=1e-12, alpha=2.0, beta=-0.1, reference=1.0 TeV
    LogParabola lp(1e-12, 2.0, -0.1, 1.0);

    // Test at reference energy
    // log10(flux) = log10(1e-12) + 2.0*log10(1) + (-0.1)*log10(1)^2 = -12
    // flux = 10^(-12) = 1e-12
    double flux_at_ref = lp.evaluate(1.0);
    assert(std::abs(flux_at_ref - 1e-12) < TOLERANCE * 10);

    // Test at E = 10 TeV
    // log10(flux) = -12 + 2.0*1 + (-0.1)*1 = -12 + 2 - 0.1 = -10.1
    // flux = 10^(-10.1) ≈ 7.94e-11
    double flux_at_10 = lp.evaluate(10.0);
    double expected = std::pow(10.0, -10.1);
    assert(std::abs(flux_at_10 - expected) < TOLERANCE * 100);

    // Test getters
    assert(std::abs(lp.alpha() - 2.0) < TOLERANCE);
    assert(std::abs(lp.beta() - (-0.1)) < TOLERANCE);

    // Test type
    assert(lp.type() == "LogParabola");

    std::cout << "  LogParabola tests passed!" << std::endl;
}

void test_constant_flux() {
    std::cout << "Testing ConstantFlux..." << std::endl;

    // Create model: amplitude=1e-12
    ConstantFlux cf(1e-12);

    // Test at various energies (should all return same value)
    assert(std::abs(cf.evaluate(0.1) - 1e-12) < TOLERANCE);
    assert(std::abs(cf.evaluate(1.0) - 1e-12) < TOLERANCE);
    assert(std::abs(cf.evaluate(10.0) - 1e-12) < TOLERANCE);
    assert(std::abs(cf.evaluate(100.0) - 1e-12) < TOLERANCE);

    // Test at non-positive energy
    assert(cf.evaluate(0.0) == 0.0);
    assert(cf.evaluate(-1.0) == 0.0);

    // Test array evaluation
    Eigen::ArrayXd energies(4);
    energies << 0.1, 1.0, 10.0, 100.0;
    Eigen::ArrayXd fluxes = cf.evaluate(energies);
    for (int i = 0; i < fluxes.size(); ++i) {
        assert(std::abs(fluxes[i] - 1e-12) < TOLERANCE);
    }

    // Test getters
    assert(std::abs(cf.amplitude() - 1e-12) < TOLERANCE);

    // Test type
    assert(cf.type() == "ConstantFlux");

    std::cout << "  ConstantFlux tests passed!" << std::endl;
}

void test_broken_power_law() {
    std::cout << "Testing BrokenPowerLaw..." << std::endl;

    // Create model: amplitude=1e-12, index1=2.0, index2=3.0, reference=1.0, break=10.0, delta=1.0
    BrokenPowerLaw bpl(1e-12, 2.0, 3.0, 1.0, 10.0, 1.0);

    // Test below break energy (E << E_break)
    // flux ≈ amplitude * (E/E_ref)^(-index1)
    double flux_low = bpl.evaluate(1.0);
    double expected_low = 1e-12 * std::pow(1.0, -2.0);
    assert(std::abs(flux_low - expected_low) < TOLERANCE * 10);

    // Test at break energy
    // flux = amplitude * (E_break/E_ref)^(-index1) / (1 + 1)^((index1-index2)/delta)
    //     = 1e-12 * 10^(-2) / 2^(-1) = 1e-14 / 0.5 = 2e-14
    double flux_at_break = bpl.evaluate(10.0);
    double expected_break = 1e-14 * std::pow(1.0 + 1.0, (2.0 - 3.0) / 1.0);
    assert(std::abs(flux_at_break - expected_break) < TOLERANCE * 100);

    // Test above break energy (E >> E_break)
    // flux ≈ amplitude * (E/E_ref)^(-index2) * transition_factor
    double flux_high = bpl.evaluate(100.0);
    assert(flux_high > 0.0 && flux_high < flux_low);  // Should be positive but smaller than low-energy flux

    // Test getters
    assert(std::abs(bpl.break_energy() - 10.0) < TOLERANCE);
    assert(std::abs(bpl.delta() - 1.0) < TOLERANCE);

    // Test type
    assert(bpl.type() == "BrokenPowerLaw");

    std::cout << "  BrokenPowerLaw tests passed!" << std::endl;
}

void test_convenience_functions() {
    std::cout << "Testing convenience functions..." << std::endl;

    // Test create_power_law
    auto pl = create_power_law(1e-12, 2.0);
    assert(std::abs(pl.amplitude() - 1e-12) < TOLERANCE);
    assert(std::abs(pl.index() - 2.0) < TOLERANCE);
    assert(std::abs(pl.reference() - 1.0) < TOLERANCE);  // Default reference at 1 TeV

    // Test create_pulsar_spectrum
    auto pulsar = create_pulsar_spectrum(1e-12, 1.5, 5.0);
    assert(pulsar.type() == "ExpCutoffPowerLaw");
    assert(std::abs(pulsar.cutoff() - 5.0) < TOLERANCE);

    // Test create_blazar_spectrum
    auto blazar = create_blazar_spectrum(1e-12, 2.0, 3.5, 0.3);
    assert(blazar.type() == "BrokenPowerLaw");
    assert(std::abs(blazar.break_energy() - 0.3) < TOLERANCE);

    std::cout << "  Convenience functions tests passed!" << std::endl;
}

void test_parameter_validation() {
    std::cout << "Testing parameter validation..." << std::endl;

    // Test negative amplitude (should throw)
    bool caught = false;
    try {
        PowerLaw pl(-1e-12, 2.0, 1.0);
    } catch (const std::invalid_argument& e) {
        caught = true;
    }
    assert(caught);

    // Test non-positive reference energy (should throw)
    caught = false;
    try {
        PowerLaw pl(1e-12, 2.0, 0.0);
    } catch (const std::invalid_argument& e) {
        caught = true;
    }
    assert(caught);

    // Test negative cutoff energy (should throw)
    caught = false;
    try {
        ExpCutoffPowerLaw ec_pl(1e-12, 2.0, 1.0, -10.0);
    } catch (const std::invalid_argument& e) {
        caught = true;
    }
    assert(caught);

    std::cout << "  Parameter validation tests passed!" << std::endl;
}

void test_model_descriptions() {
    std::cout << "Testing model descriptions..." << std::endl;

    PowerLaw pl(1e-12, 2.0, 1.0);
    std::string pl_desc = pl.description();
    assert(pl_desc.find("1.000000") != std::string::npos);  // amplitude
    assert(pl_desc.find("2.000000") != std::string::npos);  // index

    ExpCutoffPowerLaw ec_pl(1e-12, 2.0, 1.0, 10.0);
    std::string ec_pl_desc = ec_pl.description();
    assert(ec_pl_desc.find("Exp. Cutoff") != std::string::npos);
    assert(ec_pl_desc.find("10.000000") != std::string::npos);  // cutoff

    std::cout << "  Model description tests passed!" << std::endl;
}

int main() {
    std::cout << "Running spectral model tests..." << std::endl;
    std::cout << "=================================" << std::endl;

    try {
        test_power_law();
        test_exp_cutoff_power_law();
        test_log_parabola();
        test_constant_flux();
        test_broken_power_law();
        test_convenience_functions();
        test_parameter_validation();
        test_model_descriptions();

        std::cout << "=================================" << std::endl;
        std::cout << "All tests passed successfully!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}