#include "f_rules.hpp"

namespace xtc
{
    xMoves available_moves(const xState &state, rules_id rules)   
    {
        switch (rules)
        {
        case rules_id::kendall:
            return xMoves(available_moves_kendall(state._seed));
            break;
        case rules_id::meub:
            return xMoves(available_moves_meub(state._seed));
            break;
        default:
            throw std::runtime_error("unexpected rules_id");
        }
    }
    xState increment_state(const xState &state, rules_id rules)
    {
        switch (rules)
        {
        case rules_id::kendall:
            return xState(increment_state_kendall(state._seed));
            break;
        case rules_id::meub:
            return xState(increment_state_meub(state._seed));
            break;
        default:
            throw std::runtime_error("unexpected rules_id");
        }
    }

    xMoves available_moves_kendall(const xState &)
    {
        return xMoves();
    }
    xMoves available_moves_meub(const xState &)
    {
        return xMoves();
    }
    xState increment_state_kendall(const xState &)
    {
        return xState();
    }
    xState increment_state_meub(const xState &)
    {
        return xState();
    }
} // namespace xtc