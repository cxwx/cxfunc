#include "cxRandomSeed3.hh"
#include <iostream>
#include <random>

int main() {
    using namespace cxfunc::rand;

    // Test each seed source
    auto testSeed = [](const std::string& label, const std::string& s) {
        unsigned seed = strToSeed(s);
        std::cout << label << ": seed = " << seed << "\n";
    };

    testSeed("TIME", "TIME");
    testSeed("42", "42");
    testSeed("urandom", "urandom");

    return 0;
}
