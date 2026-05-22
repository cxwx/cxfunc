# CMakeLists.txt for maps benchmarks

add_executable(benchmark_maps benchmark_maps.cpp)
target_link_libraries(benchmark_maps PRIVATE cxfunc_maps)