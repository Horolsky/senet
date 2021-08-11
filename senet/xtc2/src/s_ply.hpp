#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include "senet.fwd.hpp"
#include "s_state.hpp"
#include "s_moves.hpp"
#include "s_event.hpp"

namespace senet
{
    class xPly
    {
        xState _state;
        xMoves _moves;
        xEvent _event;

    public:
        xPly() : _state(), _moves(), _event(){};
        xPly(uint64_t state) : _state(state), _moves(), _event(){};
        xPly(uint64_t state, uint32_t moves) : _state(state), _moves(moves), _event(){};
        xPly(xState state) : _state(state), _moves(), _event(){};
        xPly(xState state, xMoves moves) : _state(state), _moves(moves), _event(){};
        xPly(const xPly &other) : _state(other._state), _moves(other._moves), _event(other._event){};
        xPly(xPly &&other) : _state(other._state), _moves(other._moves), _event(other._event){};
        xPly &operator=(const xPly &other) = delete;
        xPly &operator=(xPly &&other) = delete;
        ~xPly() = default;

        unit_id operator[](uint8_t index) const;
        std::vector<uint8_t> board() const;
        unit_id agent() const;
        uint8_t steps() const;

        std::vector<uint8_t> moves() const;
        event_id direction() const;
        uint8_t mobility() const;

        xEvent event() const;

        void set_steps(uint8_t stp);
        void set_agent(unit_id agn);
    };
} // namespace xtc