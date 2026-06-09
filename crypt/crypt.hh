#ifndef CRYPT_CX_HH
#define CRYPT_CX_HH
#include <type_traits>
namespace cxfunc::crypt::p1 {
template <typename T>
// TODO(CX):
  auto encrypt(const T& t, int p) -> void requires(std::is_integral_v<T>) {
  t ^= p;
}
template <typename T>
void decrypt(const T& t, int p) {
  t ^= p;
}
// TODO(CX): tem->using
// using decrypt = encrypt<T>;
}

#endif
