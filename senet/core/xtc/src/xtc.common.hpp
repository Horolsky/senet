#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include <cstdint>

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;

using intvec = std::vector<int>;

#define _XTC_BULK_EXTERN(T)                                                   \
  extern template void bitf::solid::get_bulk<T, uint64_t> (T, T, uint64_t,    \
                                                           size_t, int);      \
  extern template uint64_t bitf::solid::set_bulk<T, uint64_t> (               \
      T, T, uint64_t, size_t, int);

#define _XTC_BULK_EXPLICIT(T)                                                 \
  template void bitf::solid::get_bulk<T, uint64_t> (T, T, uint64_t, size_t,   \
                                                    int);                     \
  template uint64_t bitf::solid::set_bulk<T, uint64_t> (T, T, uint64_t,       \
                                                        size_t, int);

// using intvec = std::vector<int>;
