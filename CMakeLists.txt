cmake_minimum_required(VERSION 3.15)
project(CxFunc VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ====== Installation Directory ======
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/software/install" CACHE PATH "Install path" FORCE)
endif()

message(STATUS "Installation prefix: ${CMAKE_INSTALL_PREFIX}")

# ====== Global Build Options ======
option(BUILD_TESTS "Build test programs for all modules" OFF)
option(BUILD_EXAMPLES "Build example programs for all modules" OFF)
option(BUILD_BENCHMARKS "Build benchmark programs for all modules" OFF)
option(CXFUNC_STANDALONE "Standalone installation (includes cmake configs)" OFF)

# Set top-level build flag
set(CXFUNC_TOP_LEVEL_BUILD TRUE CACHE INTERNAL "Top-level CMake build")

# ====== Modules ======
set(CXFUNC_MODULES
    maps
    compress
    config
    math
    modeling
    poisson
    random
    stats
    timer
)

# Pass global options to subdirectories
foreach(module ${CXFUNC_MODULES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${module}/CMakeLists.txt")
        message(STATUS "Adding module: ${module}")
        add_subdirectory(${module})
    else()
        message(WARNING "Module CMakeLists.txt not found: ${module}")
    endif()
endforeach()

# ====== Installation ======
find_package(ROOT QUIET)

if(NOT CXFUNC_STANDALONE)
    # Unified installation: only lib, include, and ROOT dictionaries
    include(GNUInstallDirs)

    # Collect all library targets
    set(CXFUNC_INSTALL_TARGETS)
    if(TARGET cxfunc_maps)
        list(APPEND CXFUNC_INSTALL_TARGETS cxfunc_maps)
    endif()
    if(TARGET cxfileio)
        list(APPEND CXFUNC_INSTALL_TARGETS cxfileio)
    endif()
    if(TARGET cxconfig)
        list(APPEND CXFUNC_INSTALL_TARGETS cxconfig)
    endif()
    if(TARGET cxfunc_math)
        list(APPEND CXFUNC_INSTALL_TARGETS cxfunc_math)
    endif()
    if(TARGET cxfunc_modeling)
        list(APPEND CXFUNC_INSTALL_TARGETS cxfunc_modeling)
    endif()
    if(TARGET cxrandom)
        list(APPEND CXFUNC_INSTALL_TARGETS cxrandom)
    endif()
    if(TARGET cxfunc_stats)
        list(APPEND CXFUNC_INSTALL_TARGETS cxfunc_stats)
    endif()

    # Install all libraries
    if(CXFUNC_INSTALL_TARGETS)
        install(
            TARGETS ${CXFUNC_INSTALL_TARGETS}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        )
    endif()

    # Install all headers from modules
    foreach(module ${CXFUNC_MODULES})
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${module}/include")
            install(
                DIRECTORY ${module}/include/
                DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
                FILES_MATCHING PATTERN "*.h" PATTERN "*.hh" PATTERN "*.hpp"
            )
        endif()

        # Install module headers directly in module directory
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${module}")
            file(GLOB module_headers
                ${CMAKE_CURRENT_SOURCE_DIR}/${module}/*.h
                ${CMAKE_CURRENT_SOURCE_DIR}/${module}/*.hh
                ${CMAKE_CURRENT_SOURCE_DIR}/${module}/*.hpp
            )
            if(module_headers)
                install(
                    FILES ${module_headers}
                    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc
                )
            endif()
        endif()
    endforeach()

    # Install ROOT dictionaries
    if(ROOT_FOUND)
        install(
            FILES
                ${CMAKE_BINARY_DIR}/random/libcxrandom_rdict.pcm
                ${CMAKE_BINARY_DIR}/random/libcxrandom.rootmap
                ${CMAKE_BINARY_DIR}/config/libcxconfig_rdict.pcm
                ${CMAKE_BINARY_DIR}/config/libcxconfig.rootmap
                ${CMAKE_BINARY_DIR}/compress/libcxfileio_rdict.pcm
                ${CMAKE_BINARY_DIR}/compress/libcxfileio.rootmap
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            OPTIONAL
        )
    endif()
else()
    # Standalone installation: include cmake configs
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/CxFuncConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/CxFuncConfigVersion.cmake"
        DESTINATION cmake/CxFunc
    )
endif()

# Print summary
message(STATUS "")
message(STATUS "====== CxFunc Configuration Summary ======")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Build tests: ${BUILD_TESTS}")
message(STATUS "Build examples: ${BUILD_EXAMPLES}")
message(STATUS "Build benchmarks: ${BUILD_BENCHMARKS}")
message(STATUS "===========================================")
message(STATUS "")
