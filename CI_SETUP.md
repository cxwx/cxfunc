# CxFunc CI Configuration

## Overview

Overall build system for multiple C++ modules with automated testing via GitHub Actions.

## Building Instructions

### Overall Build and Install

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
make install
```

### Custom Installation Prefix

```bash
cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
```

### Disable Components

```bash
cmake -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF ..
```

## CI Dependencies

**macOS (latest) - Complete Support:**
- Homebrew: cmake, eigen, gsl, boost, yaml-cpp, nlohmann-json, tomlplusplus, rapidyaml, googletest
- Tests: ENABLED
- GSL: ENABLED
- Goal: Full functionality and testing coverage

**Ubuntu CI Removed:**
- Ubuntu 24.04 dependency issues proved too complex
- Focusing on macOS for reliable CI/CD
- Local development on Ubuntu still works fine

## Module Dependencies

- **maps**: Eigen3
- **compress**: Boost
- **config**: yaml-cpp, nlohmann-json, tomlplusplus, rapidyaml (optional)
- **math**: Eigen3, GSL (optional)
- **modeling**: Eigen3
- **random**: ROOT (optional)
- **stats**: Eigen3, GSL (optional)

## CI Badge

[![CI](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/ci.yml/badge.svg)](https://github.com/YOUR_USERNAME/YOUR_REPO/actions/workflows/ci.yml)

## Usage

Link all modules in your project:

```cmake
find_package(CxFunc REQUIRED)
target_link_libraries(your_target PRIVATE cxfunc_all)
```

Or link individual modules:

```cmake
target_link_libraries(your_target PRIVATE
    cxfunc_math
    cxfunc_stats
    cxconfig::cxconfig
)
```

## Origin and Acknowledgments

* **gammapy** [stats, modeling, math], BSD-3-Clause
* **claude** - AI assistant for code development
