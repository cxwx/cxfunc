#include "swapbyte.hh"
#include <bitset>
#include <iostream>

using namespace std;

namespace cxfunc::swapbyte {
void swapsOld(void *obj, size_t s) {
  unsigned char *start = nullptr;
  unsigned char *end = nullptr;
  {
    for (start = static_cast<unsigned char *>(obj), end = start + s - 1; start < end; ++start, --end) {
      unsigned char swap = *start;
      *start = *end;
      *end = swap;
    }
  }
}
}  // namespace cxfunc::swapbyte

namespace cxfunc::type {
auto operator<<(ostream &out, const short2bit &v) -> ostream & {
  out
    << std::bitset<4>((v.data() >> 12) & 0b1111) << " "
    << std::bitset<4>((v.data() >> 8) & 0b1111) << " "
    << std::bitset<4>((v.data() >> 4) & 0b1111) << " "
    << std::bitset<4>(v.data() & 0b1111) << " "
      << " ";
  return out;
}

}  // namespace cxfunc::type
