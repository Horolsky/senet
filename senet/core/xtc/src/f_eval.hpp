#pragma once
#include "xtc.fwd.hpp"
#include "s_ply.hpp"

namespace xtc
{
    float evaluate_state(const xState &state);
    float evaluate_state(const uint64_t seed);
} // namespace xtc
