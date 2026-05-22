/**
 * @file axes_example.cpp
 * @brief Examples demonstrating map axes usage
 */

#include <cxfunc/maps.hpp>
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "=== CxFunc Maps Library - Axes Examples ===" << std::endl;
    std::cout << "Version: " << cxfunc::maps::version() << std::endl;
    std::cout << std::endl;

    using namespace cxfunc::maps;

    // ========================================
    // 1. Basic Axis Creation
    // ========================================
    std::cout << "1. Basic Axis Creation" << std::endl;
    std::cout << "-----------------------" << std::endl;

    // Create regular energy axis
    MapAxis energy_axis = create_energy_axis(10, 0.1, 10.0);  // 10 bins from 0.1 to 10 TeV

    std::cout << "Energy axis (0.1 - 10 TeV, 10 bins):" << std::endl;
    std::cout << "  Number of bins: " << energy_axis.nbin() << std::endl;
    std::cout << "  Is regular: " << (energy_axis.is_regular() ? "Yes" : "No") << std::endl;
    std::cout << "  Bin width: " << energy_axis.width(0) << " TeV" << std::endl;
    std::cout << "  First bin: [" << energy_axis.edges()(0) << ", "
              << energy_axis.edges()(1) << "] TeV" << std::endl;
    std::cout << "  Last bin: [" << energy_axis.edges()(9) << ", "
              << energy_axis.edges()(10) << "] TeV" << std::endl;

    std::cout << std::endl;

    // ========================================
    // 2. Irregular Axis Creation
    // ========================================
    std::cout << "2. Irregular Axis Creation" << std::endl;
    std::cout << "----------------------------" << std::endl;

    // Create log-spaced energy axis
    Eigen::ArrayXd log_edges(6);
    log_edges << 0.1, 0.2, 0.5, 1.0, 2.0, 5.0;  // TeV

    MapAxis log_energy_axis(log_edges, "energy", "TeV", AxisType::Energy);
    log_energy_axis.set_interpolation_method(InterpolationMethod::Log);

    std::cout << "Log-spaced energy axis:" << std::endl;
    std::cout << "  Number of bins: " << log_energy_axis.nbin() << std::endl;
    std::cout << "  Is regular: " << (log_energy_axis.is_regular() ? "Yes" : "No") << std::endl;
    std::cout << "  Bin widths: ";
    for (int i = 0; i < log_energy_axis.nbin(); ++i) {
        std::cout << log_energy_axis.width(i);
        if (i < log_energy_axis.nbin() - 1) std::cout << ", ";
    }
    std::cout << " TeV" << std::endl;

    std::cout << std::endl;

    // ========================================
    // 3. Coordinate Conversion
    // ========================================
    std::cout << "3. Coordinate Conversion" << std::endl;
    std::cout << "--------------------------" << std::endl;

    // Pixel to world coordinate conversion
    std::cout << "Pixel → World coordinate conversion:" << std::endl;
    for (double pix : {0.5, 2.5, 5.5, 9.5}) {
        double energy = energy_axis.pix_to_coord(pix);
        std::cout << "  pix = " << std::setw(4) << pix << " → energy = " << std::setw(6) << energy << " TeV" << std::endl;
    }

    std::cout << std::endl;

    // World to pixel coordinate conversion
    std::cout << "World → Pixel coordinate conversion:" << std::endl;
    for (double energy : {0.15, 0.6, 1.5, 3.0, 7.0}) {
        double pix = energy_axis.coord_to_pix(energy);
        std::cout << "  energy = " << std::setw(6) << energy << " TeV → pix = " << std::setw(6) << pix << std::endl;
    }

    std::cout << std::endl;

    // ========================================
    // 4. Multi-dimensional Axes
    // ========================================
    std::cout << "4. Multi-dimensional Axes" << std::endl;
    std::cout << "---------------------------" << std::endl;

    // Create spatial axes
    MapAxis lon_axis = create_spatial_axis(20, -5.0, 5.0);  // 20 bins, -5 to +5 degrees
    MapAxis lat_axis = create_spatial_axis(20, -5.0, 5.0);  // 20 bins, -5 to +5 degrees

    std::cout << "Spatial axes:" << std::endl;
    std::cout << "  Longitude: " << lon_axis.nbin() << " bins from "
              << lon_axis.edges()(0) << " to " << lon_axis.edges()(20) << " deg" << std::endl;
    std::cout << "  Latitude:  " << lat_axis.nbin() << " bins from "
              << lat_axis.edges()(0) << " to " << lat_axis.edges()(20) << " deg" << std::endl;

    // Create 3D map axes (energy, lon, lat)
    MapAxes map_3d({log_energy_axis, lon_axis, lat_axis});

    std::cout << std::endl;
    std::cout << "3D Map properties:" << std::endl;
    std::cout << "  Number of dimensions: " << map_3d.n_dim() << std::endl;
    std::cout << "  Total number of pixels: " << map_3d.n_bins() << std::endl;
    std::cout << "  Shape: [";
    auto shape = map_3d.shape();
    for (size_t i = 0; i < shape.size(); ++i) {
        std::cout << shape[i];
        if (i < shape.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // ========================================
    // 5. Multi-dimensional Coordinate Conversion
    // ========================================
    std::cout << std::endl;
    std::cout << "5. Multi-dimensional Coordinate Conversion" << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    // 3D pixel coordinates
    std::vector<double> pix_3d = {2.5, 10.5, 5.5};  // energy=2.5, lon=10.5, lat=5.5
    auto coord_3d = map_3d.pix_to_coord(pix_3d);

    std::cout << "Pixel → 3D World coordinates:" << std::endl;
    std::cout << "  pix = [" << pix_3d[0] << ", " << pix_3d[1] << ", " << pix_3d[2] << "]" << std::endl;
    std::cout << "  coord = [" << coord_3d[0] << " TeV, " << coord_3d[1] << " deg, "
              << coord_3d[2] << " deg]" << std::endl;

    // ========================================
    // 6. Pixel Index Operations
    // ========================================
    std::cout << std::endl;
    std::cout << "6. Pixel Index Operations" << std::endl;
    std::cout << "-------------------------" << std::endl;

    // 2D pixel to flat index
    std::vector<int> pix_2d = {3, 7};  // lon=3, lat=7
    size_t idx_2d = map_3d.pix_to_idx({0, pix_2d[0], pix_2d[1]});  // energy=0

    std::cout << "2D pixel → Flat index:" << std::endl;
    std::cout << "  pix = [lon=" << pix_2d[0] << ", lat=" << pix_2d[1] << "]" << std::endl;
    std::cout << "  idx = " << idx_2d << std::endl;

    // Flat index to 2D pixel
    std::vector<int> pix_back = map_3d.idx_to_pix(idx_2d);
    std::cout << "  pix_back = [lon=" << pix_back[1] << ", lat=" << pix_back[2] << "]" << std::endl;

    std::cout << std::endl;

    // ========================================
    // 7. Axis Slicing
    // ========================================
    std::cout << "7. Axis Slicing" << std::endl;
    std::cout << "---------------" << std::endl;

    // Slice energy axis to 1-3 TeV range
    MapAxis sliced_axis = log_energy_axis.slice(1.5, 4.5);  // approximate bin range

    std::cout << "Original axis: " << log_energy_axis.nbin() << " bins" << std::endl;
    std::cout << "Sliced axis: " << sliced_axis.nbin() << " bins" << std::endl;
    std::cout << "Sliced range: [" << sliced_axis.edges()(0) << ", "
              << sliced_axis.edges()(sliced_axis.nbin()) << "] TeV" << std::endl;

    std::cout << std::endl;

    // ========================================
    // 8. Practical Example: Spectral Analysis
    // ========================================
    std::cout << "8. Practical Example: Spectral Analysis Setup" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    // Create analysis setup
    Eigen::ArrayXd analysis_edges(11);
    analysis_edges << 0.1, 0.178, 0.316, 0.562, 1.0, 1.78, 3.16, 5.62, 10.0, 17.8, 31.6;  // ~0.3 dex spacing

    MapAxis analysis_energy(analysis_edges, "energy", "TeV", AxisType::Energy);
    analysis_energy.set_interpolation_method(InterpolationMethod::Log);

    std::cout << "Spectral analysis setup:" << std::endl;
    std::cout << "  Energy bins: " << analysis_energy.nbin() << " from "
              << analysis_energy.edges()(0) << " to "
              << analysis_energy.edges()(10) << " TeV" << std::endl;
    std::cout << "  Bin centers (first 5): ";
    for (int i = 0; i < 5; ++i) {
        std::cout << analysis_energy.center(i) << " ";
    }
    std::cout << "..." << std::endl;
    std::cout << "  Energy range: " << analysis_energy.edges()(10) - analysis_energy.edges()(0)
              << " TeV" << std::endl;

    // Coordinate conversion for spectral analysis
    double target_energy = 3.16;  // sqrt(10) TeV
    double pix_energy = analysis_energy.coord_to_pix(target_energy);
    int bin_energy = static_cast<int>(std::round(pix_energy));

    std::cout << std::endl;
    std::cout << "  Target energy: " << target_energy << " TeV" << std::endl;
    std::cout << "  Corresponding pixel: " << pix_energy << std::endl;
    std::cout << "  Corresponding bin: " << bin_energy << std::endl;
    std::cout << "  Bin range: [" << analysis_energy.edges()(bin_energy)
              << ", " << analysis_energy.edges()(bin_energy + 1) << "] TeV" << std::endl;

    std::cout << std::endl;

    return 0;
}