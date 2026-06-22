// Benchmark Usage Example
// This file demonstrates the simplified benchmark2.hh API

#include <iostream>
#include <thread>

// Using benchmark2.hh (pure C++, no ROOT dependency)
#include "benchmark2.hh"

// Example function
int expensiveCalculation(int n) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return n * n;
}

int main() {
    std::cout << "=== Simplified Benchmark2 Demo ===" << '\n';

    // Using benchmark2.hh (pure C++)
    auto benchmarkedCalc = cxfunc::timer::benchmark2(expensiveCalculation, "expensiveCalculation");
    int result = benchmarkedCalc(42);

    std::cout << "Result: " << result << "\n\n";

    std::cout << "Key Features:" << '\n';
    std::cout << "1. Pure C++ - No ROOT dependency" << '\n';
    std::cout << "2. Simple API - Just benchmark2(function, name)" << '\n';
    std::cout << "3. Works with both void and non-void functions" << '\n';
    std::cout << "4. High precision timing using std::chrono" << '\n';

    return 0;
}