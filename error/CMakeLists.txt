cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cxfunc_error VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Adding error module")

# === example ===
add_executable(cxErrorTest cxErrorTest.cpp)

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(FILES error_exit.hh
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc
    )

    # Create package config files for find_package
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_errorConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Simple package config
    configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/cxfunc_errorConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_errorConfig.cmake"
        @ONLY
    )

    install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_errorConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_errorConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_error
    )
endif()
