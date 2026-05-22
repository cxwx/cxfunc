# Basic example
add_executable(basic_example basic_example.cpp)

target_link_libraries(basic_example
    PRIVATE
        cxfunc_stats
)

# ON-OFF test example
add_executable(onoff_test onoff_test.cpp)

target_link_libraries(onoff_test
    PRIVATE
        cxfunc_stats
)

# Advanced example (add more examples as needed)
# add_executable(advanced_example advanced_example.cpp)
# target_link_libraries(advanced_example PRIVATE cxfunc_stats)