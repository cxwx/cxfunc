cmake_minimum_required(VERSION 3.15)

# Axes example
add_executable(axes_example axes_example.cpp)
target_link_libraries(axes_example PRIVATE cxfunc_maps)