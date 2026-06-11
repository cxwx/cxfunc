cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cxfunc_poisson VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === sources ===
file(GLOB headers *.hh)

# === header-only library ===
add_library(cxfunc_poisson INTERFACE)
add_library(cxfunc_poisson::cxfunc_poisson ALIAS cxfunc_poisson)

target_include_directories(cxfunc_poisson INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc)

    # Export targets
    install(
        EXPORT cxfunc_poissonTargets
        NAMESPACE cxfunc_poisson::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_poisson
    )

    # Create package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_poissonConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Create package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/cxfunc_poissonConfig.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_poissonConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_poisson
    )

    # Install package config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_poissonConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_poissonConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_poisson
    )
endif()
