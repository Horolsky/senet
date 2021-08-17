/**
 * xtc.libs.cpp
 * precompiled header functions
 * NB: add explicit external instantiation when needed
 * */

#include "xtc.common.hpp"
#include "xtc.fwd.hpp"

#define _xtc_ivec std::vector<int>

template void bitf::solid::get_bulk<_xtc_ivec::iterator, uint64_t> (
    _xtc_ivec::iterator, _xtc_ivec::iterator, uint64_t, size_t, int);
template void bitf::solid::get_bulk<int *, uint64_t> (int *, int *, uint64_t,
                                                      size_t, int);

template uint64_t bitf::solid::set_bulk<_xtc_ivec::iterator, uint64_t> (
    _xtc_ivec::iterator, _xtc_ivec::iterator, uint64_t, size_t, int);
template uint64_t
bitf::solid::set_bulk<int *, uint64_t> (int *, int *, uint64_t, size_t, int);

