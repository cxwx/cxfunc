# Poisson Error Bar

A lightweight C++ library for calculating confidence intervals (error bars) for Poisson-distributed data.

## Overview

This library provides a simple and efficient implementation for computing asymmetric error bars for Poisson-distributed counts. When dealing with count data following Poisson statistics, symmetric error bars (√n) are often insufficient, especially for low counts. This implementation provides proper asymmetric confidence intervals.

## Features

- **Asymmetric Error Bars**: Calculates separate upper and lower error bars for Poisson-distributed data
- **Efficient**: Header-only implementation with zero runtime overhead
- **Simple API**: Easy-to-use interface with minimal dependencies
- **Standard Compliant**: Modern C++ with type safety

## Installation

### Build from Source

```bash
git clone <repository-url>
cd poisson
mkdir build && cd build
cmake ..
make install
```

### Usage

Include the header in your project:

```cpp
#include "cxPoisson.hh"
```

## API Reference

### `errorBar(double n)`

Calculates the upper and lower error bars for a Poisson-distributed count.

**Parameters:**
- `n`: The observed count (must be non-negative)

**Returns:**
- `std::tuple<double, double>`: A tuple containing (upper_error, lower_error)

**Example:**

```cpp
#include <iostream>
#include "cxPoisson.hh"

int main() {
    double count = 10.0;
    auto [up, down] = cxfunc::poisson::errorBar(count);
    
    std::cout << "Count: " << count << std::endl;
    std::cout << "Upper error bar: " << up << std::endl;
    std::cout << "Lower error bar: " << down << std::endl;
    std::cout << "Range: [" << (count - down) << ", " << (count + up) << "]" << std::endl;
    
    return 0;
}
```

## Mathematical Background

The error bars are calculated using the following formulas:

- **Upper error bar**: `up = 0.5 + sqrt(n + 0.25)`
- **Lower error bar**: `down = -0.5 + sqrt(n + 0.25)`

These formulas provide approximate confidence intervals that are more accurate than the simple √n approximation, especially for low-count events where the Poisson distribution is significantly asymmetric.

## Dependencies

- C++11 or later
- CMake 3.10 or later (for building)
- Standard library only - no external dependencies

## References

The implementation is based on statistical methods for Poisson confidence intervals. For detailed information about the statistical theory, see:

**[CDF Poisson Error Bars - Fermilab CDF Statistics Notes](https://www-cdf.fnal.gov/physics/statistics/notes/pois_eb.txt)**

## Create by claude
