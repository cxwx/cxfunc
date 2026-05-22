cmake_minimum_required(VERSION 3.15)

# Test for axes module
add_executable(test_axes test_axes.cpp)
target_link_libraries(test_axes PRIVATE cxfunc_maps)

add_test(NAME test_axes COMMAND test_axes)