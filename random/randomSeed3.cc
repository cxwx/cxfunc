#include "randomSeed3.hh"
#include <array>
#include <bit>
#include <ctime>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <random>

namespace cxfunc::rand {

auto strToSeed(const std::string& s) -> unsigned int {
  if (s == "TIME" || s == "time" || s == "Time") {
    std::cerr << "cxrandom: setting random seed using time, not a good idea\n";
    return static_cast<unsigned int>(time(nullptr));
  }
  if (s == "random_device") {
    std::random_device rd;
    if (rd.entropy() > 0) {
      return rd();
    }
    std::cerr << "cxrandom: random_device not available or non-deterministic\n";
    std::terminate();
  }
  if (s == "urandom") {
    std::ifstream urandom("/dev/urandom", std::ios::binary);
    if (!urandom) {
      std::cerr << "cxrandom: urandom error 1 device\n";
      std::terminate();
    }
    unsigned seed = 0;
    std::array<char, sizeof(seed)> buffer{};
    urandom.read(buffer.data(), buffer.size());
    auto bytesRead = urandom.gcount();
    if (bytesRead != static_cast<std::streamsize>(buffer.size())) {
      std::cerr << "cxrandom: urandom error 3 bytes\n";
      std::terminate();
    }
    if (!urandom) {
      std::cerr << "cxrandom: urandom error 2 read\n";
      std::terminate();
    }
    return std::bit_cast<unsigned int>(buffer);
  }
  return static_cast<unsigned int>(std::stoi(s));
}

}  // namespace cxfunc::rand
