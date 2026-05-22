# CMakeLists.txt for math benchmarks

add_executable(benchmark_math benchmark_math.cpp)
target_link_libraries(benchmark_math PRIVATE cxfunc_math)