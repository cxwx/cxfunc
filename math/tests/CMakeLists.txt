cmake_minimum_required(VERSION 3.15)

# Test for roots module
add_executable(test_roots test_roots.cpp)
target_link_libraries(test_roots PRIVATE cxfunc_math)

add_test(NAME test_roots COMMAND test_roots)

# Test for gauss module
add_executable(test_gauss test_gauss.cpp)
target_link_libraries(test_gauss PRIVATE cxfunc_math)

add_test(NAME test_gauss COMMAND test_gauss)

# Test for interpolation module
add_executable(test_interpolation test_interpolation.cpp)
target_link_libraries(test_interpolation PRIVATE cxfunc_math)

add_test(NAME test_interpolation COMMAND test_interpolation)