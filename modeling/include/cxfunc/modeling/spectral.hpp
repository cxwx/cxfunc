#ifndef CXFUNC_MODELING_SPECTRAL_HPP
#define CXFUNC_MODELING_SPECTRAL_HPP

#include <Eigen/Dense>
#include <vector>
#include <string>
#include <functional>
#include <cmath>
#include <stdexcept>

#include "cxfunc/modeling/version.hpp"

namespace cxfunc {
namespace modeling {
namespace spectral {

/**
 * @brief Abstract base class for spectral models
 *
 * All spectral models should inherit from this class and implement
 * the evaluate() method to compute flux at a given energy.
 */
class SpectralModel {
public:
    virtual ~SpectralModel() = default;

    /**
     * @brief Evaluate spectral model at given energy
     * @param energy Energy value (same unit as reference energy)
     * @return Differential flux (dN/dE) at given energy
     *
     * Units: ph cm^-2 s^-1 TeV^-1 (or equivalent)
     */
    virtual double evaluate(double energy) const = 0;

    /**
     * @brief Evaluate spectral model at multiple energies
     * @param energies Vector of energy values
     * @return Vector of differential flux values
     */
    virtual Eigen::ArrayXd evaluate(const Eigen::ArrayXd& energies) const {
        Eigen::ArrayXd result(energies.size());
        for (int i = 0; i < energies.size(); ++i) {
            result[i] = evaluate(energies[i]);
        }
        return result;
    }

    /**
     * @brief Get model description
     * @return Human-readable model description
     */
    virtual std::string description() const = 0;

    /**
     * @brief Get model type name
     * @return Model type identifier
     */
    virtual std::string type() const = 0;
};

/**
 * @brief Power law spectral model
 *
 * One of the most common spectral models in gamma-ray astronomy:
 * dN/dE = amplitude * (E / E_ref)^(-index)
 *
 * Commonly used for describing non-thermal emission from cosmic rays
 * and astrophysical sources.
 *
 * Example:
 * @code
 * PowerLaw pl(1e-12, 2.0, 1.0);  // amplitude=1e-12, index=2.0, E_ref=1.0 TeV
 *
 * // Evaluate at 1 TeV
 * double flux = pl.evaluate(1.0);  // Should return 1e-12
 *
 * // Evaluate at multiple energies
 * Eigen::ArrayXd energies(5);
 * energies << 0.1, 0.5, 1.0, 5.0, 10.0;
 * Eigen::ArrayXd fluxes = pl.evaluate(energies);
 * @endcode
 */
class PowerLaw : public SpectralModel {
public:
    // Bring base class overloads into scope
    using SpectralModel::evaluate;

    /**
     * @brief Construct power law model
     * @param amplitude Amplitude at reference energy (ph cm^-2 s^-1 TeV^-1)
     * @param index Spectral index (typically 1.5 - 3.5 for gamma-ray sources)
     * @param reference Reference energy in TeV
     */
    PowerLaw(double amplitude, double index, double reference)
        : amplitude_(amplitude), index_(index), reference_(reference) {

        if (amplitude < 0) {
            throw std::invalid_argument("PowerLaw: amplitude must be non-negative");
        }

        if (reference <= 0) {
            throw std::invalid_argument("PowerLaw: reference energy must be positive");
        }
    }

    /**
     * @brief Evaluate power law at given energy
     * @param energy Energy in TeV
     * @return Differential flux
     */
    double evaluate(double energy) const override {
        if (energy <= 0) {
            return 0.0;  // No flux at non-positive energies
        }

        double ratio = energy / reference_;
        return amplitude_ * std::pow(ratio, -index_);
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Power Law: dN/dE = " + std::to_string(amplitude_) +
               " * (E / " + std::to_string(reference_) + ")^-" +
               std::to_string(index_);
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "PowerLaw";
    }

    /**
     * @brief Get amplitude parameter
     * @return Amplitude value
     */
    double amplitude() const { return amplitude_; }

    /**
     * @brief Get spectral index
     * @return Index value
     */
    double index() const { return index_; }

    /**
     * @brief Get reference energy
     * @return Reference energy in TeV
     */
    double reference() const { return reference_; }

private:
    double amplitude_;   ///< Amplitude at reference energy
    double index_;       ///< Spectral index
    double reference_;   ///< Reference energy (TeV)
};

/**
 * @brief Power law with exponential cutoff spectral model
 *
 * Common model for gamma-ray pulsars and sources with spectral cutoff:
 * dN/dE = amplitude * (E / E_ref)^(-index) * exp(-E / E_cutoff)
 *
 * The exponential cutoff provides a natural cutoff at high energies,
 * commonly observed in pulsar spectra.
 *
 * Example:
 * @code
 * ExpCutoffPowerLaw ec_pl(1e-12, 2.0, 1.0, 10.0);
 * // amplitude, index, E_ref, E_cutoff
 *
 * // Evaluate at 1 TeV (should be lower than pure power law)
 * double flux = ec_pl.evaluate(1.0);
 * @endcode
 */
class ExpCutoffPowerLaw : public SpectralModel {
public:
    // Bring base class overloads into scope
    using SpectralModel::evaluate;

    /**
     * @brief Construct power law with exponential cutoff
     * @param amplitude Amplitude at reference energy (ph cm^-2 s^-1 TeV^-1)
     * @param index Spectral index
     * @param reference Reference energy in TeV
     * @param cutoff Cutoff energy in TeV
     */
    ExpCutoffPowerLaw(double amplitude, double index, double reference, double cutoff)
        : amplitude_(amplitude), index_(index), reference_(reference), cutoff_(cutoff) {

        if (amplitude < 0) {
            throw std::invalid_argument("ExpCutoffPowerLaw: amplitude must be non-negative");
        }

        if (reference <= 0 || cutoff <= 0) {
            throw std::invalid_argument("ExpCutoffPowerLaw: energies must be positive");
        }
    }

    /**
     * @brief evaluate exponential cutoff power law
     * @param energy Energy in TeV
     * @return Differential flux
     */
    double evaluate(double energy) const override {
        if (energy <= 0) {
            return 0.0;
        }

        double power_law = amplitude_ * std::pow(energy / reference_, -index_);
        double cutoff_term = std::exp(-energy / cutoff_);

        return power_law * cutoff_term;
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Exp. Cutoff Power Law: dN/dE = " + std::to_string(amplitude_) +
               " * (E / " + std::to_string(reference_) + ")^-" +
               std::to_string(index_) + " * exp(-E / " +
               std::to_string(cutoff_) + ")";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "ExpCutoffPowerLaw";
    }

    /**
     * @brief Get cutoff energy
     * @return Cutoff energy in TeV
     */
    double cutoff() const { return cutoff_; }

private:
    double amplitude_;   ///< Amplitude at reference energy
    double index_;       ///< Spectral index
    double reference_;   ///< Reference energy (TeV)
    double cutoff_;      ///< Cutoff energy (TeV)
};

/**
 * @brief Log parabola spectral model
 *
 * Used to model curved spectra with parabolic shape in log-log space:
 * log10(dN/dE) = a + b*log10(E/E_ref) + c*log10(E/E_ref)^2
 *
 * Often used for describing gamma-ray bursts and time-variable sources.
 *
 * Example:
 * @code
 * LogParabola lp(1e-12, 2.0, -0.1, 1.0);
 * // amplitude, alpha, beta, E_ref
 *
 * // Evaluate at 1 TeV
 * double flux = lp.evaluate(1.0);
 * @endcode
 */
class LogParabola : public SpectralModel {
public:
    // Bring base class overloads into scope
    using SpectralModel::evaluate;

    /**
     * @brief Construct log parabola model
     * @param amplitude Amplitude at reference energy (ph cm^-2 s^-1 TeV^-1)
     * @param alpha Linear term coefficient
     * @param beta Quadratic term coefficient
     * @param reference Reference energy in TeV
     */
    LogParabola(double amplitude, double alpha, double beta, double reference)
        : amplitude_(amplitude), alpha_(alpha), beta_(beta), reference_(reference) {

        if (amplitude < 0) {
            throw std::invalid_argument("LogParabola: amplitude must be non-negative");
        }

        if (reference <= 0) {
            throw std::invalid_argument("LogParabola: reference energy must be positive");
        }
    }

    /**
     * @brief Evaluate log parabola at given energy
     * @param energy Energy in TeV
     * @return Differential flux
     */
    double evaluate(double energy) const override {
        if (energy <= 0) {
            return 0.0;
        }

        double log_energy = std::log10(energy / reference_);
        double log_flux = std::log10(amplitude_) + alpha_ * log_energy + beta_ * log_energy * log_energy;

        return std::pow(10.0, log_flux);
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Log Parabola: log10(flux) = log10(" + std::to_string(amplitude_) +
               ") + " + std::to_string(alpha_) + "*log10(E/" + std::to_string(reference_) +
               ") + " + std::to_string(beta_) + "*log10(E/" + std::to_string(reference_) + ")^2";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "LogParabola";
    }

    /**
     * @brief Get curvature parameter
     * @return Beta value (curvature)
     */
    double beta() const { return beta_; }

    /**
     * @brief Get linear parameter
     * @return Alpha value (slope)
     */
    double alpha() const { return alpha_; }

private:
    double amplitude_;   ///< Amplitude at reference energy
    double alpha_;       ///< Linear coefficient
    double beta_;        ///< Quadratic coefficient
    double reference_;   ///< Reference energy (TeV)
};

/**
 * @brief Constant flux spectral model
 *
 * Simple model with energy-independent flux:
 * dN/dE = amplitude
 *
 * Used for testing or as a baseline model.
 *
 * Example:
 * @code
 * ConstantFlux cf(1e-12);  // Constant flux of 1e-12 ph cm^-2 s^-1 TeV^-1
 *
 * // Evaluate at any energy (should return same value)
 * double flux1 = cf.evaluate(0.1);
 * double flux2 = cf.evaluate(10.0);
 * // Both should return 1e-12
 * @endcode
 */
class ConstantFlux : public SpectralModel {
public:
    // Bring base class overloads into scope
    using SpectralModel::evaluate;

    /**
     * @brief Construct constant flux model
     * @param amplitude Constant differential flux (ph cm^-2 s^-1 TeV^-1)
     */
    explicit ConstantFlux(double amplitude)
        : amplitude_(amplitude) {

        if (amplitude < 0) {
            throw std::invalid_argument("ConstantFlux: amplitude must be non-negative");
        }
    }

    /**
     * @brief Evaluate constant flux at given energy
     * @param energy Energy in TeV (ignored)
     * @return Constant differential flux
     */
    double evaluate(double energy) const override {
        if (energy <= 0) {
            return 0.0;
        }
        return amplitude_;
    }

    /**
     * @brief Evaluate constant flux at multiple energies
     * @param energies Vector of energy values
     * @return Vector of constant flux values
     */
    Eigen::ArrayXd evaluate(const Eigen::ArrayXd& energies) const override {
        Eigen::ArrayXd result(energies.size());
        for (int i = 0; i < energies.size(); ++i) {
            result[i] = (energies[i] > 0) ? amplitude_ : 0.0;
        }
        return result;
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Constant Flux: dN/dE = " + std::to_string(amplitude_);
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "ConstantFlux";
    }

    /**
     * @brief Get amplitude
     * @return Constant amplitude value
     */
    double amplitude() const { return amplitude_; }

private:
    double amplitude_;   ///< Constant differential flux
};

/**
 * @brief Broken power law spectral model
 *
 * Power law with both low and high energy breaks:
 * dN/dE = amplitude * (E / E_ref)^(-index1) / (1 + (E / E_break)^((index2-index1)/delta))
 *
 * Used to model spectra with curvature or breaks, commonly observed in
 * gamma-ray bursts and active galactic nuclei.
 *
 * Example:
 * @code
 * BrokenPowerLaw bpl(1e-12, 2.0, 3.0, 100.0, 1.0);
 * // amplitude, index1, index2, E_break, delta
 *
 * // Evaluate at different energies
 * double flux_1 = bpl.evaluate(1.0);   // Below break energy
 * double flux_100 = bpl.evaluate(100.0); // Above break energy
 * @endcode
 */
class BrokenPowerLaw : public SpectralModel {
public:
    // Bring base class overloads into scope
    using SpectralModel::evaluate;

    /**
     * @brief Construct broken power law model
     * @param amplitude Amplitude at reference energy (ph cm^-2 s^-1 TeV^-1)
     * @param index1 Spectral index below break energy
     * @param index2 Spectral index above break energy
     * @param reference Reference energy in TeV
     * @param break_energy Break energy in TeV
     * @param delta Transition sharpness (dimensionless)
     */
    BrokenPowerLaw(double amplitude, double index1, double index2,
                   double reference, double break_energy, double delta)
        : amplitude_(amplitude), index1_(index1), index2_(index2),
          reference_(reference), break_energy_(break_energy), delta_(delta) {

        if (amplitude < 0) {
            throw std::invalid_argument("BrokenPowerLaw: amplitude must be non-negative");
        }

        if (reference <= 0 || break_energy <= 0) {
            throw std::invalid_argument("BrokenPowerLaw: energies must be positive");
        }

        if (delta <= 0) {
            throw std::invalid_argument("BrokenPowerLaw: delta must be positive");
        }
    }

    /**
     * @brief Evaluate broken power law at given energy
     * @param energy Energy in TeV
     * @return Differential flux
     */
    double evaluate(double energy) const override {
        if (energy <= 0) {
            return 0.0;
        }

        double power_law = amplitude_ * std::pow(energy / reference_, -index1_);
        double break_term = std::pow(1.0 + std::pow(energy / break_energy_, delta_), (index1_ - index2_) / delta_);

        return power_law * break_term;
    }

    /**
     * @brief Get model description
     * @return Model description string
     */
    std::string description() const override {
        return "Broken Power Law: dN/dE = " + std::to_string(amplitude_) +
               " * (E / " + std::to_string(reference_) + ")^-" + std::to_string(index1_) +
               " / (1 + (E / " + std::to_string(break_energy_) + ")^(" +
               std::to_string((index1_ - index2_) / delta_) + ")";
    }

    /**
     * @brief Get model type
     * @return Type identifier
     */
    std::string type() const override {
        return "BrokenPowerLaw";
    }

    /**
     * @brief Get break energy
     * @return Break energy in TeV
     */
    double break_energy() const { return break_energy_; }

    /**
     * @brief Get transition sharpness
     * @return Delta value
     */
    double delta() const { return delta_; }

private:
    double amplitude_;      ///< Amplitude at reference energy
    double index1_;        ///< Spectral index below break
    double index2_;        ///< Spectral index above break
    double reference_;      ///< Reference energy (TeV)
    double break_energy_;   ///< Break energy (TeV)
    double delta_;         ///< Transition sharpness
};

// ==================== Convenience Functions ====================

/**
 * @brief Create a standard power law model
 * @param amplitude Flux at 1 TeV (ph cm^-2 s^-1 TeV^-1)
 * @param index Spectral index
 * @return PowerLaw model
 */
inline PowerLaw create_power_law(double amplitude, double index) {
    return PowerLaw(amplitude, index, 1.0);  // Reference at 1 TeV
}

/**
 * @brief Create typical pulsar spectrum model
 * @param amplitude Flux at 1 TeV
 * @param index Spectral index (~1.5-2.5 for pulsars)
 * @param cutoff Cutoff energy in TeV (~1-10 TeV for young pulsars)
 * @return ExpCutoffPowerLaw model
 */
inline ExpCutoffPowerLaw create_pulsar_spectrum(double amplitude, double index, double cutoff) {
    return ExpCutoffPowerLaw(amplitude, index, 1.0, cutoff);
}

/**
 * @brief Create typical blazar spectrum model
 * @param amplitude Flux at 1 TeV
 * @param index1 Low-energy index (~1.5-2.0)
 * @param index2 High-energy index (~3.0-4.0)
 * @param break_energy Break energy (~0.1-1 TeV)
 * @return BrokenPowerLaw model
 */
inline BrokenPowerLaw create_blazar_spectrum(double amplitude, double index1, double index2,
                                           double break_energy = 0.3) {
    return BrokenPowerLaw(amplitude, index1, index2, 1.0, break_energy, 1.0);
}

} // namespace spectral
} // namespace modeling
} // namespace cxfunc

#endif // CXFUNC_MODELING_SPECTRAL_HPP