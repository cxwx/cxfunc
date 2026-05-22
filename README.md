# CxFunc V2 - Self Used Function Library

v1 on git.lug.ustc
v2 rewrite by claude (WIP)

[![CI](https://github.com/cxwx/cxfunc/actions/workflows/ci.yml/badge.svg)](https://github.com/cxwx/cxfunc/actions/workflows/ci.yml)

## Overview

This is a collection of standalone C++ modules unified under a single build system.

## Modules

- **maps**: Mapping and coordinate transformations
- **compress**: File I/O and compression utilities
- **config**: Configuration management (JSON, YAML, TOML)
- **math**: Mathematical functions and algorithms
- **modeling**: Statistical modeling and fitting
- **random**: Random number generation utilities
- **stats**: Statistical analysis tools

## Dependencies

- **CMake** >= 3.15
- **C++17** or later
- **Eigen3** (any version, Ubuntu 24.04 has 3.4.0)
- **ROOT** (optional, for dictionary generation)
- **GSL** >= 2.0 (optional)
- **Boost** >= 1.71 (optional)
- **nlohmann/json** >= 3.11.0
- **toml++** (for config module)
- **rapidyaml** (for config module)
- **GTest** (for testing)

**Local Installation (Homebrew):**
```bash
brew install cmake ninja eigen gsl boost yaml-cpp nlohmann-json tomlplusplus rapidyaml googletest
```

## Build Instructions

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

### Individual Module Build

Each module can be built independently:

```bash
cd math
mkdir build && cd build
cmake ..
make install
```

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

## CI Status

The project uses GitHub Actions for continuous integration:
- **macOS (latest)**: Full dependencies and testing
- **Removed Ubuntu CI**: Focusing on macOS for stability

## Origin and Acknowledgments

* **gammapy** [stats, modeling, math], BSD-3-Clause
* **claude** - AI assistant for code development
* **glm-5.1** - AI language model
* **mimo-v2.5-pro** - AI language model  
* **qwen-3.6:35b** - AI language model
