#include <iostream>
#include <thread>
#include "benchmark2.hh"

// Test function that returns a value
int calculateSum(int a, int b) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return a + b;
}

// Test void function
void printMessage(const std::string& msg) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    std::cout << "Message: " << msg << '\n';
}

int main() {
    std::cout << "Testing benchmark2.hh (simplified pure C++ std implementation)" << '\n';
    std::cout << "==============================================================" << '\n';

    // Test with return value
    auto benchmarkedSum = cxfunc::timer::benchmark2(calculateSum, "calculateSum");
    int result = benchmarkedSum(10, 20);
    std::cout << "Result: " << result << '\n';

    std::cout << '\n';

    // Test with void function
    auto benchmarkedPrint = cxfunc::timer::benchmark2(printMessage, "printMessage");
    benchmarkedPrint("Hello from benchmark2!");

    std::cout << '\n';
    std::cout << "Test completed successfully!" << '\n';

    return 0;
}