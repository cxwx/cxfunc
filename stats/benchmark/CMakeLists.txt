# CMakeLists.txt for stats benchmarks

add_executable(benchmark_stats benchmark_stats.cpp)
target_link_libraries(benchmark_stats PRIVATE cxfunc_stats)