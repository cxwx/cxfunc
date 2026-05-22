/**
 * @file test_axes.cpp
 * @brief Unit tests for map axes
 */

#include <cxfunc/maps.hpp>
#include <iostream>
#include <cmath>
#include <iomanip>

using namespace cxfunc::maps;

// Test helper functions
bool approx_equal(double a, double b, double tol = 1e-10) {
    return std::abs(a - b) < tol;
}

void test_regular_axis() {
    std::cout << "Testing regular axis..." << std::endl;

    // Create regular axis
    MapAxis axis(10, 0.0, 10.0, "test", "units");

    // Test 1: Number of bins
    std::cout << "  Number of bins: ";
    if (axis.nbin() == 10) {
        std::cout << "PASS (10 bins)" << std::endl;
    } else {
        std::cout << "FAIL (expected 10, got " << axis.nbin() << ")" << std::endl;
    }

    // Test 2: Regular spacing
    std::cout << "  Regular spacing: ";
    if (axis.is_regular()) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL (should be regular)" << std::endl;
    }

    // Test 3: Bin width
    std::cout << "  Bin width: ";
    double width = axis.width(0);
    if (approx_equal(width, 1.0, 1e-10)) {
        std::cout << "PASS (width = " << width << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected 1.0, got " << width << ")" << std::endl;
    }

    // Test 4: All bins have same width
    std::cout << "  All bins equal width: ";
    bool all_equal = true;
    for (int i = 1; i < axis.nbin(); ++i) {
        if (!approx_equal(axis.width(i), width, 1e-10)) {
            all_equal = false;
            break;
        }
    }
    if (all_equal) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL (not all bins have equal width)" << std::endl;
    }

    // Test 5: Edge values
    std::cout << "  Edge values: ";
    if (approx_equal(axis.edges()(0), 0.0, 1e-10) &&
        approx_equal(axis.edges()(10), 10.0, 1e-10)) {
        std::cout << "PASS ([0, 10])" << std::endl;
    } else {
        std::cout << "FAIL (expected [0, 10])" << std::endl;
    }
}

void test_irregular_axis() {
    std::cout << "Testing irregular axis..." << std::endl;

    // Create irregular axis
    Eigen::ArrayXd edges(6);
    edges << 0.0, 1.0, 2.5, 5.0, 9.0, 15.0;

    MapAxis axis(edges, "test", "units");

    // Test 1: Number of bins
    std::cout << "  Number of bins: ";
    if (axis.nbin() == 5) {
        std::cout << "PASS (5 bins)" << std::endl;
    } else {
        std::cout << "FAIL (expected 5, got " << axis.nbin() << ")" << std::endl;
    }

    // Test 2: Not regular
    std::cout << "  Not regular: ";
    if (!axis.is_regular()) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL (should not be regular)" << std::endl;
    }

    // Test 3: Different bin widths
    std::cout << "  Variable bin widths: ";
    bool different = true;
    for (int i = 1; i < axis.nbin(); ++i) {
        if (approx_equal(axis.width(i), axis.width(0), 1e-10)) {
            different = false;
            break;
        }
    }
    if (different) {
        std::cout << "PASS" << std::endl;
    } else {
        std::cout << "FAIL (bins have different widths)" << std::endl;
    }

    // Test 4: Center calculation
    std::cout << "  Center calculation: ";
    double center_0 = axis.center(0);  // Should be (0 + 1) / 2 = 0.5
    if (approx_equal(center_0, 0.5, 1e-10)) {
        std::cout << "PASS (center[0] = 0.5)" << std::endl;
    } else {
        std::cout << "FAIL (expected 0.5, got " << center_0 << ")" << std::endl;
    }
}

void test_coordinate_conversion() {
    std::cout << "Testing coordinate conversion..." << std::endl;

    // Create regular axis for testing
    MapAxis axis(10, 0.0, 10.0, "test", "units");

    // Test 1: Integer pixel to coordinate (should give bin center)
    std::cout << "  Integer pixel → coordinate: ";
    double coord_5 = axis.pix_to_coord(5.0);
    double expected_5 = 5.5;  // Center of bin 5
    if (approx_equal(coord_5, expected_5, 1e-10)) {
        std::cout << "PASS (pix 5 → coord " << coord_5 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_5 << ", got " << coord_5 << ")" << std::endl;
    }

    // Test 2: Coordinate to pixel (reverse)
    std::cout << "  Coordinate → pixel: ";
    double pix_back = axis.coord_to_pix(coord_5);
    if (approx_equal(pix_back, 5.0, 1e-10)) {
        std::cout << "PASS (coord " << coord_5 << " → pix " << pix_back << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected 5.0, got " << pix_back << ")" << std::endl;
    }

    // Test 3: Fractional pixel
    std::cout << "  Fractional pixel → coordinate: ";
    double coord_5_5 = axis.pix_to_coord(5.5);
    double expected_5_5 = 6.0;  // Halfway through bin 5
    if (approx_equal(coord_5_5, expected_5_5, 1e-10)) {
        std::cout << "PASS (pix 5.5 → coord " << coord_5_5 << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_5_5 << ", got " << coord_5_5 << ")" << std::endl;
    }

    // Test 4: Boundary conditions
    std::cout << "  Boundary conditions: ";
    double coord_0 = axis.pix_to_coord(-1.0);  // Below range
    double coord_10 = axis.pix_to_coord(11.0);  // Above range
    if (approx_equal(coord_0, 0.0, 1e-10) && approx_equal(coord_10, 10.0, 1e-10)) {
        std::cout << "PASS (handles out-of-range)" << std::endl;
    } else {
        std::cout << "FAIL (boundary handling incorrect)" << std::endl;
    }
}

void test_bin_operations() {
    std::cout << "Testing bin operations..." << std::endl;

    MapAxis axis(10, 0.0, 10.0, "test", "units");

    // Test 1: Bin finding
    std::cout << "  Bin finding: ";
    int bin_5_5 = axis.coord_to_bin(5.5);
    if (bin_5_5 == 5) {
        std::cout << "PASS (coord 5.5 → bin 5)" << std::endl;
    } else {
        std::cout << "FAIL (expected bin 5, got " << bin_5_5 << ")" << std::endl;
    }

    // Test 2: Bin edges
    std::cout << "  Bin edges: ";
    auto edges = axis.bin_edges(3);
    if (approx_equal(edges.first, 3.0, 1e-10) && approx_equal(edges.second, 4.0, 1e-10)) {
        std::cout << "PASS (bin 3: [3.0, 4.0])" << std::endl;
    } else {
        std::cout << "FAIL (expected [3.0, 4.0])" << std::endl;
    }

    // Test 3: Bin extent
    std::cout << "  Axis extent: ";
    auto extent = axis.extent();
    if (approx_equal(extent.first, 0.0, 1e-10) && approx_equal(extent.second, 10.0, 1e-10)) {
        std::cout << "PASS ([0.0, 10.0])" << std::endl;
    } else {
        std::cout << "FAIL (expected [0.0, 10.0])" << std::endl;
    }
}

void test_multidim_axes() {
    std::cout << "Testing multi-dimensional axes..." << std::endl;

    // Create 2D axes
    MapAxis axis1(10, 0.0, 10.0, "x", "units");
    MapAxis axis2(5, 0.0, 5.0, "y", "units");

    MapAxes axes({axis1, axis2});

    // Test 1: Number of dimensions
    std::cout << "  Number of dimensions: ";
    if (axes.n_dim() == 2) {
        std::cout << "PASS (2D)" << std::endl;
    } else {
        std::cout << "FAIL (expected 2, got " << axes.n_dim() << ")" << std::endl;
    }

    // Test 2: Total number of bins
    std::cout << "  Total bins: ";
    if (axes.n_bins() == 50) {  // 10 * 5 = 50
        std::cout << "PASS (50 bins)" << std::endl;
    } else {
        std::cout << "FAIL (expected 50, got " << axes.n_bins() << ")" << std::endl;
    }

    // Test 3: Shape
    std::cout << "  Shape: ";
    auto shape = axes.shape();
    if (shape.size() == 2 && shape[0] == 10 && shape[1] == 5) {
        std::cout << "PASS ([10, 5])" << std::endl;
    } else {
        std::cout << "FAIL (expected [10, 5])" << std::endl;
    }

    // Test 4: 3D pixel to flat index
    std::cout << "  3D pixel → flat index: ";
    std::vector<int> pix = {2, 3};  // x=2, y=3
    size_t idx = axes.pix_to_idx(pix);
    size_t expected_idx = 2 * 5 + 3;  // x * ny + y
    if (idx == expected_idx) {
        std::cout << "PASS (pix [2,3] → idx " << idx << ")" << std::endl;
    } else {
        std::cout << "FAIL (expected " << expected_idx << ", got " << idx << ")" << std::endl;
    }

    // Test 5: Flat index to 3D pixel
    std::cout << "  Flat index → 3D pixel: ";
    std::vector<int> pix_back = axes.idx_to_pix(idx);
    if (pix_back.size() == 2 && pix_back[0] == 2 && pix_back[1] == 3) {
        std::cout << "PASS (idx " << idx << " → pix [2,3])" << std::endl;
    } else {
        std::cout << "FAIL (round-trip failed)" << std::endl;
    }
}

void test_energy_axis() {
    std::cout << "Testing energy axis creation..." << std::endl;

    // Create energy axis using convenience function
    MapAxis energy = create_energy_axis(8, 0.1, 10.0);

    // Test 1: Correct number of bins
    std::cout << "  Energy bins: ";
    if (energy.nbin() == 8) {
        std::cout << "PASS (8 bins)" << std::endl;
    } else {
        std::cout << "FAIL (expected 8, got " << energy.nbin() << ")" << std::endl;
    }

    // Test 2: Axis type
    std::cout << "  Axis type: ";
    if (energy.type() == AxisType::Energy) {
        std::cout << "PASS (Energy axis)" << std::endl;
    } else {
        std::cout << "FAIL (not Energy axis)" << std::endl;
    }

    // Test 3: Unit
    std::cout << "  Unit: ";
    if (energy.unit() == "TeV") {
        std::cout << "PASS (TeV)" << std::endl;
    } else {
        std::cout << "FAIL (expected 'TeV', got '" << energy.unit() << "')" << std::endl;
    }
}

void test_spatial_axis() {
    std::cout << "Testing spatial axis creation..." << std::endl;

    // Create spatial axis
    MapAxis spatial = create_spatial_axis(15, -3.0, 3.0);

    // Test 1: Correct number of bins
    std::cout << "  Spatial bins: ";
    if (spatial.nbin() == 15) {
        std::cout << "PASS (15 bins)" << std::endl;
    } else {
        std::cout << "FAIL (expected 15, got " << spatial.nbin() << ")" << std::endl;
    }

    // Test 2: Axis type
    std::cout << "  Axis type: ";
    if (spatial.type() == AxisType::Spatial) {
        std::cout << "PASS (Spatial axis)" << std::endl;
    } else {
        std::cout << "FAIL (not Spatial axis)" << std::endl;
    }

    // Test 3: Symmetric around 0
    std::cout << "  Symmetry: ";
    double lo = spatial.edges()(0);
    double hi = spatial.edges()(15);
    if (approx_equal(std::abs(lo), 3.0, 1e-10) && approx_equal(hi, 3.0, 1e-10)) {
        std::cout << "PASS (symmetric [-3, 3] deg)" << std::endl;
    } else {
        std::cout << "FAIL (not symmetric)" << std::endl;
    }
}

void test_axis_slicing() {
    std::cout << "Testing axis slicing..." << std::endl;

    MapAxis original(10, 0.0, 10.0, "test", "units");

    // Slice to middle region
    MapAxis sliced = original.slice(3.0, 7.0);

    // Test 1: Sliced size
    std::cout << "  Sliced size: ";
    if (sliced.nbin() == 4) {  // bins 3, 4, 5, 6
        std::cout << "PASS (4 bins)" << std::endl;
    } else {
        std::cout << "FAIL (expected 4, got " << sliced.nbin() << ")" << std::endl;
    }

    // Test 2: Sliced range
    std::cout << "  Sliced range: ";
    auto extent = sliced.extent();
    if (extent.first >= 3.0 && extent.second <= 7.0) {
        std::cout << "PASS (within [3, 7])" << std::endl;
    } else {
        std::cout << "FAIL (range incorrect)" << std::endl;
    }
}

void test_error_handling() {
    std::cout << "Testing error handling..." << std::endl;

    // Test 1: Invalid bin count
    std::cout << "  Invalid bin count: ";
    try {
        MapAxis axis(0, 0.0, 10.0);
        std::cout << "FAIL (should throw exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 2: Invalid range (lo >= hi)
    std::cout << "  Invalid range: ";
    try {
        MapAxis axis(5, 5.0, 0.0);
        std::cout << "FAIL (should throw exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 3: Non-monotonic edges
    std::cout << "  Non-monotonic edges: ";
    try {
        Eigen::ArrayXd bad_edges(4);
        bad_edges << 0.0, 2.0, 1.0, 3.0;  // Not monotonic
        MapAxis axis(bad_edges);
        std::cout << "FAIL (should throw exception)" << std::endl;
    } catch (const std::invalid_argument&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }

    // Test 4: Out of range bin access
    std::cout << "  Out of range bin access: ";
    MapAxis axis(10, 0.0, 10.0);
    try {
        double center = axis.center(15);  // Bin 15 doesn't exist
        std::cout << "FAIL (should throw exception)" << std::endl;
    } catch (const std::out_of_range&) {
        std::cout << "PASS (exception thrown)" << std::endl;
    }
}

int main() {
    std::cout << "=== CxFunc Maps Library - Axes Tests ===" << std::endl;
    std::cout << "Version: " << cxfunc::maps::version() << std::endl;
    std::cout << std::endl;

    test_regular_axis();
    std::cout << std::endl;

    test_irregular_axis();
    std::cout << std::endl;

    test_coordinate_conversion();
    std::cout << std::endl;

    test_bin_operations();
    std::cout << std::endl;

    test_multidim_axes();
    std::cout << std::endl;

    test_energy_axis();
    std::cout << std::endl;

    test_spatial_axis();
    std::cout << std::endl;

    test_axis_slicing();
    std::cout << std::endl;

    test_error_handling();
    std::cout << std::endl;

    std::cout << "=== All Tests Complete ===" << std::endl;

    return 0;
}