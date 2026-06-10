#ifndef SWAPBYTES_CX___
#define SWAPBYTES_CX___

#include <algorithm>
#include <iostream>
namespace cxfunc::swapbyte {

// INFO: std::reverse is slower
// using ROOT Rbswap_16, __builtin_bswap16,
template <typename T>
  requires std::is_trivially_copyable_v<T>
void swap(T& obj) {
  char* c = reinterpret_cast<char*>(&obj);
  std::reverse(c, c + sizeof(T));
}
template <typename T>
void swaps(T& obj, size_t s) {
  char* c = reinterpret_cast<char*>(&obj);
  std::reverse(c, c + s);
}
void swapsOld(void* obj, size_t s);
}

namespace cxfunc::type{
class short2bit {
 private:
  uint16_t fData;

 public:
  [[nodiscard]] auto data() const -> uint16_t { return fData; }
  short2bit(const short2bit&) = default;
  short2bit(short2bit&&) = delete;
  auto operator=(const short2bit&) -> short2bit& = default;
  auto operator=(short2bit&&) -> short2bit& = delete;
  explicit short2bit(const uint16_t& a) : fData(a) {}
  ~short2bit() = default;
};
auto operator<<(std::ostream& out, const short2bit& v) -> std::ostream&;
}

#endif
