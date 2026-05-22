/**
 * @file spectral_example.cpp
 * @brief Example usage of spectral models for gamma-ray astronomy
 *
 * This example demonstrates how to use the various spectral models
 * implemented in the cxfunc modeling library for gamma-ray astronomy
 * applications.
 */

#include <cxfunc/modeling/spectral.hpp>
#include <iostream>
#include <vector>
#include <iomanip>

using namespace cxfunc::modeling::spectral;

void print_separator() {
    std::cout << std::string(80, '=') << std::endl;
}

void print_sub_separator() {
    std::cout << std::string(80, '-') << std::endl;
}

/**
 * @brief Example 1: Basic power law usage
 */
void example_1_basic_power_law() {
    std::cout << "\nExample 1: Basic Power Law" << std::endl;
    print_sub_separator();

    // Create a simple power law model
    // Typical for gamma-ray sources: dN/dE = amplitude * (E/E_ref)^(-index)
    PowerLaw source(1e-12, 2.3, 1.0);  // amplitude=1e-12, index=2.3, E_ref=1 TeV

    std::cout << "Source model: " << source.description() << std::endl;
    std::cout << std::scientific << std::setprecision(2);

    // Evaluate flux at different energies
    std::vector<double> energies = {0.1, 0.5, 1.0, 5.0, 10.0};  // TeV
    std::cout << "\nEnergy (TeV)  Flux (ph cm^-2 s^-1 TeV^-1)" << std::endl;
    for (auto energy : energies) {
        double flux = source.evaluate(energy);
        std::cout << std::setw(12) << energy << "  "
                  << std::setw(20) << flux << std::endl;
    }
}

/**
 * @brief Example 2: Pulsar spectrum with exponential cutoff
 */
void example_2_pulsar_spectrum() {
    std::cout << "\nExample 2: Pulsar Spectrum (Exponential Cutoff)" << std::endl;
    print_sub_separator();

    // Create a pulsar spectrum model using convenience function
    // Pulsars typically have spectra: dN/dE ~ E^(-index) * exp(-E/E_cutoff)
    auto crab_pulsar = create_pulsar_spectrum(2.0e-11, 1.5, 3.0);

    std::cout << "Crab-like pulsar model: " << crab_pulsar.description() << std::endl;
    std::cout << std::scientific << std::setprecision(2);

    // Create energy grid
    Eigen::ArrayXd energies(11);
    energies << 0.1, 0.316, 1.0, 3.16, 10.0, 31.6, 100.0, 316.0, 1000.0, 3160.0, 10000.0;

    // Evaluate flux at all energies (vectorized)
    Eigen::ArrayXd fluxes = crab_pulsar.evaluate(energies);

    std::cout << "\nEnergy (TeV)  Flux (ph cm^-2 s^-1 TeV^-1)" << std::endl;
    for (int i = 0; i < energies.size(); ++i) {
        std::cout << std::setw(12) << energies[i] << "  "
                  << std::setw(20) << fluxes[i] << std::endl;
    }

    // Compute integral flux in energy band 1-10 TeV (approximate)
    double integral_approx = 0.0;
    int n_points = 100;
    double e_min = 1.0, e_max = 10.0;
    double de = (e_max - e_min) / n_points;

    for (int i = 0; i < n_points; ++i) {
        double e = e_min + (i + 0.5) * de;
        integral_approx += crab_pulsar.evaluate(e) * de;
    }

    std::cout << "\nApproximate integral flux (1-10 TeV): "
              << std::scientific << integral_approx
              << " ph cm^-2 s^-1" << std::endl;
}

/**
 * @brief Example 3: Blazar spectrum with spectral break
 */
void example_3_blazar_spectrum() {
    std::cout << "\nExample 3: Blazar Spectrum (Broken Power Law)" << std::endl;
    print_sub_separator();

    // Create a blazar spectrum model
    // Blazars often show spectral breaks due to absorption or emission mechanisms
    auto blazar = create_blazar_spectrum(1e-12, 2.0, 3.5, 0.3);

    std::cout << "Blazar model: " << blazar.description() << std::endl;
    std::cout << std::scientific << std::setprecision(2);

    // Evaluate below and above break energy
    double e_below = 0.1;  // Below break (0.3 TeV)
    double e_at_break = 0.3;  // At break energy
    double e_above = 1.0;  // Above break

    double flux_below = blazar.evaluate(e_below);
    double flux_at_break = blazar.evaluate(e_at_break);
    double flux_above = blazar.evaluate(e_above);

    std::cout << "\nFlux below break (0.1 TeV):  " << flux_below << std::endl;
    std::cout << "Flux at break (0.3 TeV):     " << flux_at_break << std::endl;
    std::cout << "Flux above break (1.0 TeV):  " << flux_above << std::endl;

    // Compute spectral index above and below break
    double index_below = -std::log(flux_at_break / flux_below) / std::log(e_at_break / e_below);
    double index_above = -std::log(flux_above / flux_at_break) / std::log(e_above / e_at_break);

    std::cout << "\nEffective spectral index below break:  " << std::fixed
              << std::setprecision(2) << index_below << std::endl;
    std::cout << "Effective spectral index above break:  " << index_above << std::endl;
}

/**
 * @brief Example 4: Gamma-ray burst with log parabola
 */
void example_4_grb_spectrum() {
    std::cout << "\nExample 4: Gamma-Ray Burst (Log Parabola)" << std::endl;
    print_sub_separator();

    // Create a GRB spectrum model
    // GRBs often have curved spectra well described by log parabola
    LogParabola grb(1e-10, -2.0, -0.3, 1.0);  // amplitude, alpha, beta, E_ref

    std::cout << "GRB model: " << grb.description() << std::endl;
    std::cout << std::scientific << std::setprecision(2);

    // Evaluate at various energies
    std::vector<double> energies = {0.1, 1.0, 10.0, 100.0};  // TeV

    std::cout << "\nEnergy (TeV)  Flux (ph cm^-2 s^-1 TeV^-1)  Spectral index" << std::endl;

    double prev_flux = grb.evaluate(energies[0]);
    double prev_energy = energies[0];

    for (size_t i = 0; i < energies.size(); ++i) {
        double flux = grb.evaluate(energies[i]);

        // Compute local spectral index
        double index = 0.0;
        if (i > 0) {
            index = -std::log(flux / prev_flux) / std::log(energies[i] / prev_energy);
        }

        std::cout << std::setw(12) << energies[i] << "  "
                  << std::setw(30) << flux << "  "
                  << std::fixed << std::setprecision(3) << index << std::endl;

        prev_flux = flux;
        prev_energy = energies[i];
    }

    std::cout << "\nNote: The spectral index becomes steeper with energy due to beta < 0"
              << std::endl;
}

/**
 * @brief Example 5: Comparing different models
 */
void example_5_model_comparison() {
    std::cout << "\nExample 5: Comparing Different Spectral Models" << std::endl;
    print_sub_separator();

    // Create different models with similar normalization
    PowerLaw pl(1e-12, 2.0, 1.0);
    ExpCutoffPowerLaw ec_pl(1e-12, 2.0, 1.0, 10.0);
    LogParabola lp(1e-12, 2.0, -0.1, 1.0);

    std::cout << "Comparing flux at 1 TeV and 10 TeV:" << std::endl;
    std::cout << std::scientific << std::setprecision(3);

    double e1 = 1.0, e2 = 10.0;

    std::cout << "\nPowerLaw:" << std::endl;
    std::cout << "  At " << e1 << " TeV: " << pl.evaluate(e1) << std::endl;
    std::cout << "  At " << e2 << " TeV: " << pl.evaluate(e2) << std::endl;
    std::cout << "  Ratio (10/1 TeV): " << pl.evaluate(e2) / pl.evaluate(e1) << std::endl;

    std::cout << "\nExpCutoffPowerLaw (cutoff=10 TeV):" << std::endl;
    std::cout << "  At " << e1 << " TeV: " << ec_pl.evaluate(e1) << std::endl;
    std::cout << "  At " << e2 << " TeV: " << ec_pl.evaluate(e2) << std::endl;
    std::cout << "  Ratio (10/1 TeV): " << ec_pl.evaluate(e2) / ec_pl.evaluate(e1) << std::endl;

    std::cout << "\nLogParabola (beta=-0.1):" << std::endl;
    std::cout << "  At " << e1 << " TeV: " << lp.evaluate(e1) << std::endl;
    std::cout << "  At " << e2 << " TeV: " << lp.evaluate(e2) << std::endl;
    std::cout << "  Ratio (10/1 TeV): " << lp.evaluate(e2) / lp.evaluate(e1) << std::endl;
}

/**
 * @brief Example 6: Constant flux model
 */
void example_6_constant_flux() {
    std::cout << "\nExample 6: Constant Flux Model" << std::endl;
    print_sub_separator();

    // Create a constant flux model (useful for testing or baseline models)
    ConstantFlux cf(1e-13);

    std::cout << "Constant flux model: " << cf.description() << std::endl;
    std::cout << std::scientific << std::setprecision(2);

    std::cout << "\nNote: Flux is constant across all energies" << std::endl;
    std::cout << "At 0.1 TeV:  " << cf.evaluate(0.1) << std::endl;
    std::cout << "At 1.0 TeV:  " << cf.evaluate(1.0) << std::endl;
    std::cout << "At 10.0 TeV: " << cf.evaluate(10.0) << std::endl;
    std::cout << "At 100.0 TeV: " << cf.evaluate(100.0) << std::endl;
}

/**
 * @brief Example 7: Model properties and methods
 */
void example_7_model_properties() {
    std::cout << "\nExample 7: Model Properties and Methods" << std::endl;
    print_sub_separator();

    PowerLaw pl(1e-12, 2.3, 1.0);

    std::cout << "Model properties:" << std::endl;
    std::cout << "  Type: " << pl.type() << std::endl;
    std::cout << "  Description: " << pl.description() << std::endl;

    std::cout << "\nParameter access:" << std::endl;
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "  Amplitude: " << pl.amplitude() << " ph cm^-2 s^-1 TeV^-1" << std::endl;
    std::cout << "  Index: " << pl.index() << std::endl;
    std::cout << "  Reference energy: " << pl.reference() << " TeV" << std::endl;

    std::cout << "\nModel information:" << std::endl;
    std::cout << "  Model type: " << pl.type() << std::endl;
    std::cout << "  Has virtual evaluate(double): Yes" << std::endl;
    std::cout << "  Has vectorized evaluate(ArrayXd): Yes" << std::endl;
}

int main() {
    print_separator();
    std::cout << "CXFUNC MODELING LIBRARY - SPECTRAL MODELS EXAMPLE" << std::endl;
    print_separator();

    try {
        // Run all examples
        example_1_basic_power_law();
        example_2_pulsar_spectrum();
        example_3_blazar_spectrum();
        example_4_grb_spectrum();
        example_5_model_comparison();
        example_6_constant_flux();
        example_7_model_properties();

        print_separator();
        std::cout << "\nAll examples completed successfully!" << std::endl;
        std::cout << "These models can be used for:" << std::endl;
        std::cout << "  - Gamma-ray source analysis" << std::endl;
        std::cout << "  - Spectral fitting and modeling" << std::endl;
        std::cout << "  - Flux prediction and interpolation" << std::endl;
        std::cout << "  - Multi-wavelength studies" << std::endl;
        print_separator();

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}