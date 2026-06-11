cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(cxfunc_timer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Adding timer module")

# === dependencies ===
find_package(ROOT QUIET)
find_package(Geant4 QUIET)
find_package(spdlog QUIET)

# === header-only library ===
add_library(cxfunc_timer INTERFACE)
add_library(cxfunc_timer::cxfunc_timer ALIAS cxfunc_timer)

target_include_directories(cxfunc_timer INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# === examples (only when ROOT is found) ===
if(ROOT_FOUND AND Geant4_FOUND AND spdlog_FOUND)
    set(CMAKE_CXX_STANDARD 20)
    message(STATUS "Building timer examples with ROOT")

    # Random number performance comparison example
    add_executable(cxRandTest cxRandCompare.cpp)
    target_link_libraries(cxRandTest PRIVATE
        ${ROOT_LIBRARIES}
        ${Geant4_LIBRARIES}
        spdlog::spdlog
    )
    target_include_directories(cxRandTest PRIVATE
        ${ROOT_INCLUDE_DIRS}
        ${Geant4_INCLUDE_DIRS}
    )

    # ROOT header test example
    add_executable(cxHeaderTest cxHeaderTest.cpp)
    target_link_libraries(cxHeaderTest PRIVATE
        ${ROOT_LIBRARIES}
        spdlog::spdlog
    )
    target_include_directories(cxHeaderTest PRIVATE
        ${ROOT_INCLUDE_DIRS}
    )
else()
    message(WARNING "ROOT, Geant4, or spdlog not found, timer module examples will not be built")
endif()

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install headers
    install(FILES benchmark.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc/timer)

    # Install examples (if built)
    if(TARGET cxRandTest AND TARGET cxHeaderTest)
        install(TARGETS cxRandTest cxHeaderTest
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()

    # Export targets
    install(
        EXPORT cxfunc_timerTargets
        NAMESPACE cxfunc_timer::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_timer
    )

    # Create package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_timerConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Create package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/cxfunc_timerConfig.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_timerConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_timer
    )

    # Install package config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_timerConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_timerConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_timer
    )
endif()
