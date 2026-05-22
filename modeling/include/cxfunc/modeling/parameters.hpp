#ifndef CXFUNC_MODELING_PARAMETERS_HPP
#define CXFUNC_MODELING_PARAMETERS_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <cmath>

#include "cxfunc/modeling/version.hpp"

namespace cxfunc {
namespace modeling {
namespace parameters {

/**
 * @brief Parameter constraint types
 */
enum class ParameterConstraint {
    None,           ///< No constraints
    Positive,       ///< Parameter must be > 0
    NonNegative,    ///< Parameter must be >= 0
    Bounded,        ///< Parameter within [min, max]
    Fixed           ///< Parameter is fixed (not fitted)
};

/**
 * @brief Parameter scaling types for optimization
 */
enum class ParameterScale {
    Linear,         ///< No transformation
    Log,            ///< log10 transformation
    Sqrt            ///< sqrt transformation
};

/**
 * @brief Single model parameter with constraints and metadata
 *
 * Encapsulates a parameter value along with its constraints,
 * uncertainty, and scaling information.
 *
 * Example:
 * @code
 * Parameter amplitude("amplitude", 1e-12);
 * amplitude.set_constraint(ParameterConstraint::Positive);
 * amplitude.set_error(1e-13);
 * amplitude.set_scale(ParameterScale::Log);
 *
 * double value = amplitude.value();
 * double scaled_value = amplitude.scaled_value();  // log10(value)
 * @endcode
 */
class Parameter {
public:
    /**
     * @brief Construct parameter
     * @param name Parameter name
     * @param value Initial value
     * @param unit Parameter unit (optional)
     */
    Parameter(const std::string& name, double value,
              const std::string& unit = "")
        : name_(name), value_(value), unit_(unit),
          min_(-std::numeric_limits<double>::infinity()),
          max_(std::numeric_limits<double>::infinity()),
          error_(0.0), constraint_(ParameterConstraint::None),
          scale_(ParameterScale::Linear), fixed_(false) {
    }

    /**
     * @brief Construct parameter with constraint
     * @param name Parameter name
     * @param value Initial value
     * @param constraint Parameter constraint
     * @param unit Parameter unit (optional)
     */
    Parameter(const std::string& name, double value,
              ParameterConstraint constraint,
              const std::string& unit = "")
        : Parameter(name, value, unit) {
        set_constraint(constraint);
    }

    /**
     * @brief Construct bounded parameter
     * @param name Parameter name
     * @param value Initial value
     * @param min Minimum value
     * @param max Maximum value
     * @param unit Parameter unit (optional)
     */
    Parameter(const std::string& name, double value,
              double min, double max,
              const std::string& unit = "")
        : Parameter(name, value, unit) {
        set_bounds(min, max);
        set_constraint(ParameterConstraint::Bounded);
    }

    // ==================== Getters ====================

    /**
     * @brief Get parameter name
     * @return Parameter name
     */
    std::string name() const { return name_; }

    /**
     * @brief Get parameter value
     * @return Current value
     */
    double value() const { return value_; }

    /**
     * @brief Get scaled parameter value
     * @return Value transformed according to scale
     */
    double scaled_value() const {
        switch (scale_) {
            case ParameterScale::Log:
                return std::log10(value_);
            case ParameterScale::Sqrt:
                return std::sqrt(value_);
            case ParameterScale::Linear:
            default:
                return value_;
        }
    }

    /**
     * @brief Get parameter unit
     * @return Unit string
     */
    std::string unit() const { return unit_; }

    /**
     * @brief Get parameter error
     * @return Uncertainty on value
     */
    double error() const { return error_; }

    /**
     * @brief Get minimum allowed value
     * @return Minimum value
     */
    double min() const { return min_; }

    /**
     * @brief Get maximum allowed value
     * @return Maximum value
     */
    double max() const { return max_; }

    /**
     * @brief Get parameter constraint type
     * @return Constraint type
     */
    ParameterConstraint constraint() const { return constraint_; }

    /**
     * @brief Get parameter scale type
     * @return Scale type
     */
    ParameterScale scale() const { return scale_; }

    /**
     * @brief Check if parameter is fixed
     * @return True if parameter is fixed
     */
    bool fixed() const { return fixed_; }

    /**
     * @brief Check if value satisfies constraints
     * @return True if value is valid
     */
    bool valid() const {
        if (fixed_) {
            return true;  // Fixed parameters are always valid
        }

        switch (constraint_) {
            case ParameterConstraint::None:
                return true;
            case ParameterConstraint::Positive:
                return value_ > 0;
            case ParameterConstraint::NonNegative:
                return value_ >= 0;
            case ParameterConstraint::Bounded:
                return value_ >= min_ && value_ <= max_;
            case ParameterConstraint::Fixed:
                return true;
        }
        return true;
    }

    // ==================== Setters ====================

    /**
     * @brief Set parameter value
     * @param value New value
     * @throw std::invalid_argument if value violates constraints
     */
    void set_value(double value) {
        double old_value = value_;
        value_ = value;

        if (!valid()) {
            value_ = old_value;
            throw std::invalid_argument("Parameter::set_value: Value " +
                                       std::to_string(value) +
                                       " violates constraints for parameter " + name_);
        }
    }

    /**
     * @brief Set parameter value from scaled value
     * @param scaled_value Scaled value
     * @throw std::invalid_argument if resulting value violates constraints
     */
    void set_scaled_value(double scaled_value) {
        double value;

        switch (scale_) {
            case ParameterScale::Log:
                value = std::pow(10.0, scaled_value);
                break;
            case ParameterScale::Sqrt:
                value = scaled_value * scaled_value;
                break;
            case ParameterScale::Linear:
            default:
                value = scaled_value;
                break;
        }

        set_value(value);
    }

    /**
     * @brief Set parameter error
     * @param error Parameter uncertainty
     */
    void set_error(double error) {
        error_ = std::abs(error);
    }

    /**
     * @brief Set parameter bounds
     * @param min Minimum value
     * @param max Maximum value
     */
    void set_bounds(double min, double max) {
        if (min >= max) {
            throw std::invalid_argument("Parameter::set_bounds: min must be less than max");
        }
        min_ = min;
        max_ = max;
    }

    /**
     * @brief Set parameter constraint
     * @param constraint Constraint type
     */
    void set_constraint(ParameterConstraint constraint) {
        constraint_ = constraint;

        // Set default bounds for standard constraints
        switch (constraint) {
            case ParameterConstraint::Positive:
                min_ = 0.0;
                max_ = std::numeric_limits<double>::infinity();
                break;
            case ParameterConstraint::NonNegative:
                min_ = 0.0;
                max_ = std::numeric_limits<double>::infinity();
                break;
            default:
                break;
        }
    }

    /**
     * @brief Set parameter scale
     * @param scale Scale type
     */
    void set_scale(ParameterScale scale) {
        if (scale == ParameterScale::Log && value_ <= 0) {
            throw std::invalid_argument("Parameter::set_scale: Cannot use log scale for non-positive value");
        }
        if (scale == ParameterScale::Sqrt && value_ < 0) {
            throw std::invalid_argument("Parameter::set_scale: Cannot use sqrt scale for negative value");
        }
        scale_ = scale;
    }

    /**
     * @brief Fix parameter (prevent fitting)
     * @param fixed True to fix, false to free
     */
    void set_fixed(bool fixed = true) {
        fixed_ = fixed;
        if (fixed) {
            constraint_ = ParameterConstraint::Fixed;
        }
    }

    /**
     * @brief Free parameter (allow fitting)
     */
    void free() {
        fixed_ = false;
        constraint_ = ParameterConstraint::None;
    }

    /**
     * @brief Get parameter description
     * @return Description string
     */
    std::string description() const {
        std::string desc = name_ + " = " + std::to_string(value_);

        if (!unit_.empty()) {
            desc += " " + unit_;
        }

        if (error_ > 0) {
            desc += " ± " + std::to_string(error_);
        }

        if (fixed_) {
            desc += " (fixed)";
        }

        return desc;
    }

private:
    std::string name_;                  ///< Parameter name
    double value_;                      ///< Parameter value
    std::string unit_;                  ///< Parameter unit
    double min_;                        ///< Minimum value
    double max_;                        ///< Maximum value
    double error_;                      ///< Parameter uncertainty
    ParameterConstraint constraint_;    ///< Constraint type
    ParameterScale scale_;              ///< Scale type
    bool fixed_;                        ///< Fixed flag
};

/**
 * @brief Collection of model parameters
 *
 * Manages a set of parameters for a model, providing convenient
 * access by name or index.
 *
 * Example:
 * @code
 * Parameters params;
 * params.add("amplitude", 1e-12, ParameterConstraint::Positive, "ph cm^-2 s^-1 TeV^-1");
 * params.add("index", 2.0);
 * params.add("reference", 1.0);
 *
 * // Access by name
 * double amp = params.get("amplitude").value();
 *
 * // Access by index
 * double idx = params[1].value();
 *
 * // Iterate over all parameters
 * for (size_t i = 0; i < params.size(); ++i) {
 *     std::cout << params[i].description() << std::endl;
 * }
 * @endcode
 */
class Parameters {
public:
    /**
     * @brief Default constructor
     */
    Parameters() = default;

    /**
     * @brief Add parameter to collection
     * @param param Parameter to add
     */
    void add(const Parameter& param) {
        // Check for duplicate names
        for (const auto& p : parameters_) {
            if (p.name() == param.name()) {
                throw std::invalid_argument("Parameters::add: Parameter " +
                                           param.name() + " already exists");
            }
        }
        parameters_.push_back(param);
    }

    /**
     * @brief Add parameter to collection (convenience overload)
     * @param name Parameter name
     * @param value Initial value
     * @param constraint Parameter constraint
     * @param unit Parameter unit
     */
    void add(const std::string& name, double value,
             ParameterConstraint constraint = ParameterConstraint::None,
             const std::string& unit = "") {
        add(Parameter(name, value, constraint, unit));
    }

    /**
     * @brief Add parameter to collection (convenience overload with unit)
     * @param name Parameter name
     * @param value Initial value
     * @param unit Parameter unit
     * @param constraint Parameter constraint
     */
    void add(const std::string& name, double value,
             const std::string& unit,
             ParameterConstraint constraint = ParameterConstraint::None) {
        add(Parameter(name, value, constraint, unit));
    }

    /**
     * @brief Add bounded parameter to collection
     * @param name Parameter name
     * @param value Initial value
     * @param min Minimum value
     * @param max Maximum value
     * @param unit Parameter unit
     */
    void add(const std::string& name, double value,
             double min, double max,
             const std::string& unit = "") {
        add(Parameter(name, value, min, max, unit));
    }

    /**
     * @brief Get parameter by index
     * @param index Parameter index
     * @return Parameter reference
     * @throw std::out_of_range if index is invalid
     */
    Parameter& operator[](size_t index) {
        if (index >= parameters_.size()) {
            throw std::out_of_range("Parameters::operator[]: Index " +
                                   std::to_string(index) + " out of range");
        }
        return parameters_[index];
    }

    /**
     * @brief Get parameter by index (const version)
     * @param index Parameter index
     * @return Parameter const reference
     * @throw std::out_of_range if index is invalid
     */
    const Parameter& operator[](size_t index) const {
        if (index >= parameters_.size()) {
            throw std::out_of_range("Parameters::operator[]: Index " +
                                   std::to_string(index) + " out of range");
        }
        return parameters_[index];
    }

    /**
     * @brief Get parameter by name
     * @param name Parameter name
     * @return Parameter reference
     * @throw std::invalid_argument if parameter not found
     */
    Parameter& get(const std::string& name) {
        for (auto& param : parameters_) {
            if (param.name() == name) {
                return param;
            }
        }
        throw std::invalid_argument("Parameters::get: Parameter " +
                                   name + " not found");
    }

    /**
     * @brief Get parameter by name (const version)
     * @param name Parameter name
     * @return Parameter const reference
     * @throw std::invalid_argument if parameter not found
     */
    const Parameter& get(const std::string& name) const {
        for (const auto& param : parameters_) {
            if (param.name() == name) {
                return param;
            }
        }
        throw std::invalid_argument("Parameters::get: Parameter " +
                                   name + " not found");
    }

    /**
     * @brief Get number of parameters
     * @return Parameter count
     */
    size_t size() const {
        return parameters_.size();
    }

    /**
     * @brief Check if collection is empty
     * @return True if no parameters
     */
    bool empty() const {
        return parameters_.empty();
    }

    /**
     * @brief Check if parameter exists
     * @param name Parameter name
     * @return True if parameter exists
     */
    bool has(const std::string& name) const {
        for (const auto& param : parameters_) {
            if (param.name() == name) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Get parameter index by name
     * @param name Parameter name
     * @return Parameter index
     * @throw std::invalid_argument if parameter not found
     */
    size_t index(const std::string& name) const {
        for (size_t i = 0; i < parameters_.size(); ++i) {
            if (parameters_[i].name() == name) {
                return i;
            }
        }
        throw std::invalid_argument("Parameters::index: Parameter " +
                                   name + " not found");
    }

    /**
     * @brief Get all parameter names
     * @return Vector of parameter names
     */
    std::vector<std::string> names() const {
        std::vector<std::string> result;
        result.reserve(parameters_.size());
        for (const auto& param : parameters_) {
            result.push_back(param.name());
        }
        return result;
    }

    /**
     * @brief Get all parameter values
     * @return Vector of parameter values
     */
    std::vector<double> values() const {
        std::vector<double> result;
        result.reserve(parameters_.size());
        for (const auto& param : parameters_) {
            result.push_back(param.value());
        }
        return result;
    }

    /**
     * @brief Get all scaled parameter values
     * @return Vector of scaled parameter values
     */
    std::vector<double> scaled_values() const {
        std::vector<double> result;
        result.reserve(parameters_.size());
        for (const auto& param : parameters_) {
            result.push_back(param.scaled_value());
        }
        return result;
    }

    /**
     * @brief Get all free (non-fixed) parameter indices
     * @return Vector of indices for free parameters
     */
    std::vector<size_t> free_indices() const {
        std::vector<size_t> result;
        for (size_t i = 0; i < parameters_.size(); ++i) {
            if (!parameters_[i].fixed()) {
                result.push_back(i);
            }
        }
        return result;
    }

    /**
     * @brief Get number of free parameters
     * @return Count of non-fixed parameters
     */
    size_t n_free() const {
        size_t count = 0;
        for (const auto& param : parameters_) {
            if (!param.fixed()) {
                ++count;
            }
        }
        return count;
    }

    /**
     * @brief Check all parameters are valid
     * @return True if all parameters satisfy constraints
     */
    bool valid() const {
        for (const auto& param : parameters_) {
            if (!param.valid()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Clear all parameters
     */
    void clear() {
        parameters_.clear();
    }

    // Iterator support
    std::vector<Parameter>::iterator begin() {
        return parameters_.begin();
    }

    std::vector<Parameter>::iterator end() {
        return parameters_.end();
    }

    std::vector<Parameter>::const_iterator begin() const {
        return parameters_.begin();
    }

    std::vector<Parameter>::const_iterator end() const {
        return parameters_.end();
    }

    /**
     * @brief Get description of all parameters
     * @return Multi-line description string
     */
    std::string description() const {
        if (parameters_.empty()) {
            return "No parameters";
        }

        std::string desc = "Parameters (" + std::to_string(parameters_.size()) + "):\n";
        for (const auto& param : parameters_) {
            desc += "  " + param.description() + "\n";
        }
        return desc;
    }

private:
    std::vector<Parameter> parameters_;  ///< Parameter storage
};

// ==================== Convenience Functions ====================

/**
 * @brief Create parameter collection for power law model
 * @param amplitude Amplitude value
 * @param index Spectral index
 * @param reference Reference energy
 * @return Parameters object with power law parameters
 */
inline Parameters create_power_law_params(double amplitude, double index, double reference = 1.0) {
    Parameters params;
    params.add("amplitude", amplitude, ParameterConstraint::Positive, "ph cm^-2 s^-1 TeV^-1");
    params.add("index", index);
    params.add("reference", reference, ParameterConstraint::Positive, "TeV");
    params.get("reference").set_fixed(true);  // Usually fixed
    return params;
}

/**
 * @brief Create parameter collection for Gaussian spatial model
 * @param lon Center longitude
 * @param lat Center latitude
 * @param sigma Gaussian width
 * @return Parameters object with Gaussian parameters
 */
inline Parameters create_gaussian_params(double lon, double lat, double sigma) {
    Parameters params;
    params.add("lon_0", lon, "deg");
    params.add("lat_0", lat, "deg");
    params.add("sigma", sigma, ParameterConstraint::Positive, "deg");
    return params;
}

/**
 * @brief Create parameter collection for point source spatial model
 * @param lon Source longitude
 * @param lat Source latitude
 * @return Parameters object with point source parameters
 */
inline Parameters create_point_source_params(double lon, double lat) {
    Parameters params;
    params.add("lon_0", lon, "deg");
    params.add("lat_0", lat, "deg");
    return params;
}

} // namespace parameters
} // namespace modeling
} // namespace cxfunc

#endif // CXFUNC_MODELING_PARAMETERS_HPP