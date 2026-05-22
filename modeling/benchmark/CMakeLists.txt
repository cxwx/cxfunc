# CMakeLists.txt for modeling benchmarks

add_executable(benchmark_modeling benchmark_modeling.cpp)
target_link_libraries(benchmark_modeling PRIVATE cxfunc_modeling)