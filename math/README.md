# CxFunc Math Library

High-performance mathematical library for gamma-ray astronomy and computational physics.

## Overview

This library provides optimized C++ implementations of mathematical algorithms commonly used in gamma-ray astronomy, focusing on:

- **Root finding algorithms**: Bisection, Brent's method, Secant method
- **Gaussian PDFs**: 2D and multi-component Gaussian distributions for PSF modeling
- **Interpolation**: Scaled regular grid interpolation with log/sqrt support

## Features

### Root Finding
- **Bisection method**: Guaranteed convergence for bracketed roots
- **Brent's method**: Fast and robust combination of bisection, secant, and inverse quadratic interpolation
- **Secant method**: Fast convergence when good initial guesses available
- Automatic method selection with fallback strategies

### Gaussian Distributions
- **Gauss2DPDF**: 2D symmetric Gaussian for point spread function (PSF) modeling
- **MultiGauss2D**: Multi-component Gaussian for complex PSF representations
- Containment fraction and radius calculations
- Gaussian convolution operations

### Interpolation
- **ScaledRegularGridInterpolator**: Multi-dimensional interpolation on regular grids
- Support for linear, logarithmic, and square root scaling
- Optimized for astronomical data (flux points, spectra, etc.)
- 1D, 2D, and N-dimensional interpolation

## Performance

Compared to Python implementations (using numpy/scipy):

| Module | Operation | Speedup |
|--------|-----------|---------|
| Root finding | Brent's method | 5-20x |
| Gaussian PDF | Multi-component evaluation | 10-50x |
| Interpolation | Log-log interpolation | 5-30x |

## Dependencies

### Required
- **Eigen3** (≥ 3.0): Linear algebra and vector operations
- **C++17** compiler

### Optional
- **GSL** (≥ 2.0): Advanced root finding algorithms
- **Boost.Math**: Special mathematical functions (Lambert W, etc.)

## Building

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install cmake libeigen3-dev libgsl-dev libboost-math-dev

# macOS (with Homebrew)
brew install cmake eigen gsl boost
```

### Build Instructions

```bash
# Clone repository (if applicable)
cd ${HOME}/software/cxfunc/v2/math

# Create build directory
mkdir -p build && cd build

# Configure
cmake ..

# Build
make

# Run tests
make test

# Build examples
make examples
```

### Build Options

```bash
# Disable tests
cmake -DBUILD_TESTS=OFF ..

# Disable examples
cmake -DBUILD_EXAMPLES=OFF ..

# Use Boost.Math
cmake -DUSE_BOOST_MATH=ON ..

# Disable GSL
cmake -DUSE_GSL=OFF ..
```

## Usage

### Root Finding

```cpp
#include <cxfunc/math.hpp>
using namespace cxfunc::math;

// Define function
auto f = [](double x) { return x * x - 4.0; };

// Find root using Brent's method
auto result = roots::brent(f, 0.0, 5.0);
std::cout << "Root: " << result.root << std::endl;  // Output: 2.0
```

### Gaussian PDFs

```cpp
using namespace cxfunc::math::gauss;

// Create 2D Gaussian
Gauss2DPDF gauss(0.5);  // sigma = 0.5 deg

// Calculate containment radius for 68% confidence
double radius = gauss.containment_radius(0.68);

// Evaluate PDF at position (1.0, 0.5)
double value = gauss(1.0, 0.5);

// Convolve with another Gaussian
auto convolved = gauss.convolve(0.3);

// Multi-component Gaussian
std::vector<double> sigmas = {0.1, 0.3, 1.0};
std::vector<double> norms = {0.7, 0.2, 0.1};
MultiGauss2D multi(sigmas, norms);
double eff_sigma = multi.eff_sigma();
```

### Interpolation

```cpp
using namespace cxfunc::math::interpolation;

// Create energy axis and flux values
Eigen::ArrayXd energy(10);
energy << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0, 10.0, 20.0, 50.0, 100.0;  // TeV

Eigen::ArrayXd flux(10);
flux = 1.0e-12 / energy.pow(2.0);  // E^-2 spectrum

// Create interpolator with log-log scaling
std::vector<Eigen::ArrayXd> points = {energy};
ScaledRegularGridInterpolator interp(
    points, flux, Scale::Log, Scale::Log);

// Interpolate at 3.16 TeV
double result = interp.interpolate(3.16);
```

## Examples

See the `examples/` directory for complete working examples:

- `basic_example.cpp`: Demonstrates all major features
- `gauss_example.cpp`: Gaussian PDF usage for PSF modeling
- `interpolation_example.cpp`: Spectral interpolation examples

Build and run examples:

```bash
cd build
./examples/basic_example
```

## Testing

The library includes comprehensive unit tests:

```bash
cd build
ctest --verbose
```

Or run individual tests:

```bash
./tests/test_roots
./tests/test_gauss
./tests/test_interpolation
```

## Project Status

**Current Version**: 1.0.0

**Implemented Modules**:
- ✅ Root finding algorithms
- ✅ Gaussian PDFs (1D, 2D, multi-component)
- ✅ Interpolation (1D, 2D, N-dimensional)

**Planned Modules**:
- 📅 Maps and coordinate systems
- 📅 Spectral and spatial models
- 📅 Optimization and fitting

## Integration with Python

This library can be integrated with Python through:

1. **Command-line interface**: Callable via subprocess
2. **Network service**: REST/gRPC API
3. **C API**: ctypes/cffi wrapping

See the parent project documentation for integration examples.

## Related Projects

This is part of the larger `cxfunc` ecosystem:

- **stats**: Statistical analysis tools (completed)
- **math**: Mathematical algorithms (this project)
- **maps**: Astronomical maps and coordinates (planned)
- **modeling**: Spectral and spatial modeling (planned)

## License

[Specify your license here]

## Contributing

[Specify contribution guidelines here]

## Contact

[Specify contact information here]
