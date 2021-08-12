#include "s_ply.hpp"

namespace xtc
{
    unit_id xPly::operator[](uint8_t index) const
    {
        return _state[index];
    }
    std::vector<uint8_t> xPly::board() const
    {
        return _state.board();
    }
    unit_id xPly::agent() const
    {
        return _state.agent();
    }
    uint8_t xPly::steps() const
    {
        return _state.steps();
    }

    std::vector<uint8_t> xPly::moves() const
    {
        return _moves.moves();
    }
    event_id xPly::direction() const
    {
        return _moves.direction();
    }
    uint8_t xPly::mobility() const
    {
        return _moves.mobility();
    }

    xEvent xPly::event() const
    {
        return _event;
    }

    void xPly::set_steps(uint8_t stp)
    {
        _state.set_steps(stp);
    }
    void xPly::set_agent(unit_id agn)
    {
        _state.set_agent(agn);
    }
} // namespace xtc