cmake_minimum_required(VERSION 3.15)
project(CxFuncMath VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ====== Build Options ======
option(BUILD_TESTS "Build test programs" ON)
option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_BENCHMARKS "Build benchmark programs" ON)
option(USE_BOOST_MATH "Use Boost.Math for special functions" OFF)
option(USE_GSL "Use GSL for root finding" ON)

# ====== Dependencies ======
find_package(Eigen3 5.0 REQUIRED NO_MODULE)

if(USE_BOOST_MATH)
    find_package(Boost 1.71 REQUIRED COMPONENTS math)
endif()

if(USE_GSL)
    find_package(GSL 2.0 REQUIRED)
endif()

# ====== Main Library ======
add_library(cxfunc_math INTERFACE)
target_include_directories(cxfunc_math INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(cxfunc_math INTERFACE
    Eigen3::Eigen
)

if(USE_BOOST_MATH)
    target_link_libraries(cxfunc_math INTERFACE Boost::math)
    target_compile_definitions(cxfunc_math INTERFACE CXFUNC_USE_BOOST_MATH)
endif()

if(USE_GSL)
    target_link_libraries(cxfunc_math INTERFACE GSL::gsl GSL::gslcblas)
    target_compile_definitions(cxfunc_math INTERFACE CXFUNC_USE_GSL)
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
install(DIRECTORY include/ DESTINATION include)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/CxFuncMathConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/CxFuncMathConfigVersion.cmake"
    DESTINATION cmake/CxFuncMath
)