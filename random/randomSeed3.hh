#ifndef CXRANDOMSEED3_HH__
#define CXRANDOMSEED3_HH__

#include <string>

namespace cxfunc::rand {

/**
 * @brief Convert a string to a random seed value.
 *
 * Supported string values:
 *   - "TIME" / "time" / "Time" → time(nullptr)
 *   - "random_device"          → std::random_device (with entropy check)
 *   - "urandom"                → /dev/urandom (binary read, C++ bit_cast)
 *   - any other numeric string → stoi()
 */
auto strToSeed(const std::string& s) -> unsigned int;

}  // namespace cxfunc::rand

#endif
