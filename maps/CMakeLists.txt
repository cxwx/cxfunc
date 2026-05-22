cmake_minimum_required(VERSION 3.15)
project(CxFuncMaps VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ====== Build Options ======
option(BUILD_TESTS "Build test programs" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_BENCHMARKS "Build benchmark programs" ON)

# ====== Dependencies ======
find_package(Eigen3 5.0 REQUIRED NO_MODULE)

# Optional: Math library (if available as separate project)
find_package(cxfunc_math QUIET)
if(cxfunc_math_FOUND)
    message(STATUS "Found cxfunc_math, enabling integration")
else()
    message(STATUS "cxfunc_math not found, building standalone")
endif()

# ====== Main Library ======
add_library(cxfunc_maps INTERFACE)
target_include_directories(cxfunc_maps INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cxfunc_maps INTERFACE
    Eigen3::Eigen
)

if(cxfunc_math_FOUND)
    target_link_libraries(cxfunc_maps INTERFACE cxfunc_math)
endif()

# ====== Tests ======
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ====== Examples ======
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ====== Benchmarks ======
if(BUILD_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

# ====== Installation ======
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    install(DIRECTORY include/ DESTINATION include)
    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/CxFuncMapsConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/CxFuncMapsConfigVersion.cmake"
        DESTINATION cmake/CxFuncMaps
    )
endif()