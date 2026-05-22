# CMakeLists.txt for modeling tests

# Spectral model tests
add_executable(test_spectral test_spectral.cpp)
target_link_libraries(test_spectral PRIVATE cxfunc_modeling)

# Register tests
add_test(NAME test_spectral COMMAND test_spectral)