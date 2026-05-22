# CMakeLists.txt for modeling examples

# Spectral models example
add_executable(spectral_example spectral_example.cpp)
target_link_libraries(spectral_example PRIVATE cxfunc_modeling)