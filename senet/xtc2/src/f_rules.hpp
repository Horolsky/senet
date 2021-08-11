#pragma once
#include "senet.fwd.hpp"
#include "s_ply.hpp"

namespace senet
{
    //chance probabilities
    static const float P[5] = {.25, .375, .25, .0625, .0625};

    xMoves available_moves_kendall(const xState &);
    xMoves available_moves_meub(const xState &);
    
    xState increment_state_kendall(const xState &);
    xState increment_state_meub(const xState &);

    xMoves available_moves(const xState &state, rules_id rules);
    xState increment_state(const xState &state, rules_id rules);

} // namespace senet
