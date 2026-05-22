# Google Test setup
find_package(GTest REQUIRED)

# Create test executable
add_executable(test_stats test_stats.cpp)

target_link_libraries(test_stats
    PRIVATE
        cxfunc_stats
        GTest::gtest
        GTest::gtest_main
)

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(test_stats)

# Add custom target to run tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    DEPENDS test_stats
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)