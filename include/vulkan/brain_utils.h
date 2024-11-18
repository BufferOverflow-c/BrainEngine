#ifndef BRAIN_UTILS_H
#define BRAIN_UTILS_H

#include <functional>

namespace brn {

template <typename T, typename... Rest>
void hashCombine(std::size_t &seed, const T &v, const Rest &...rest) {
  seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  (hashCombine(seed, rest), ...);
};

} // namespace brn

#endif // !BRAIN_UTILS_H
