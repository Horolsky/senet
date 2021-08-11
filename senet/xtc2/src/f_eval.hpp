#pragma once
#include "senet.fwd.hpp"
#include "s_ply.hpp"

namespace senet
{
    inline float evaluate_state(const xState &state);
    float evaluate_state(const uint64_t seed);
} // namespace senet
