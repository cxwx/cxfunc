cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cxfunc_crypt VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === sources ===
file(GLOB headers *.hh)

# === header-only library ===
add_library(cxfunc_crypt INTERFACE)
add_library(cxfunc_crypt::cxfunc_crypt ALIAS cxfunc_crypt)

target_include_directories(cxfunc_crypt INTERFACE
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
        EXPORT cxfunc_cryptTargets
        NAMESPACE cxfunc_crypt::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_crypt
    )

    # Create package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_cryptConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Create package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/cxfunc_cryptConfig.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_cryptConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_crypt
    )

    # Install package config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_cryptConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_cryptConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_crypt
    )
endif()
