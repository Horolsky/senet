#pragma once
#include <cstdint>
#include "../lib/bitf/src/bitf_solid.hpp"

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;

#define _xtc_ivec std::vector<int>


#define _XTC_EXTERN_BULK                                                      \
  extern template void bitf::solid::get_bulk<_xtc_ivec::iterator, uint64_t> ( \
      _xtc_ivec::iterator, _xtc_ivec::iterator, uint64_t, size_t, int);       \
  extern template void bitf::solid::get_bulk<int *, uint64_t> (               \
      int *, int *, uint64_t, size_t, int);                                   \
  extern template uint64_t                                                    \
  bitf::solid::set_bulk<_xtc_ivec::iterator, uint64_t> (                      \
      _xtc_ivec::iterator, _xtc_ivec::iterator, uint64_t, size_t, int);       \
  extern template uint64_t bitf::solid::set_bulk<int *, uint64_t> (           \
      int *, int *, uint64_t, size_t, int);
