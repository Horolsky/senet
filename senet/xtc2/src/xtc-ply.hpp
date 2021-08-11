#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include "xtc.fwd.hpp"

#include <vector>

namespace senet
{

/**
 *  1 bit:  agent - 0 or 1
 *  3 bits: steps - 0 to 5
 * 60 bits: board - 2-bit offset cell array, 0: 1-agent, 1: 2-agent, 2: empty
 */
struct xState
{                                   //0b100110011001100110010000
    static const uint64_t def_state{0b010001000100010001000000};
    static uint64_t get_seed(unit_id agent, uint8_t steps, std::vector<uint8_t> board)
    {
        if ( agent != unit_id::empty || steps > 5 || board.size() != 30)
        {
            throw std::logic_error("invalid xState parameters");
        }
        int64_t seed{static_cast<uint64_t>(agent)};
        seed = bitf::solid::insert_scalar<uint64_t, uint8_t>(steps, 1, 3, seed);
        seed = bitf::solid::insert_vector<uint64_t, uint8_t>(board, 4, 60, seed);
        return seed;
    }

    uint64_t _seed;
    xState() : _seed(def_state){};
    xState(uint64_t seed) : _seed(seed){};
    xState(unit_id agent, uint8_t steps, std::vector<uint8_t> board) : _seed(get_seed(agent, steps, board)) {}
    xState(const xState &other) : _seed(other._seed){};
    xState(xState &&other) : _seed(other._seed){};
    xState &operator=(const xState &other) { _seed = other._seed; return *this; };
    xState &operator=(xState &&other) { _seed = other._seed; return *this; };
    ~xState() = default;

    unit_id agent() const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1));
    }
    uint8_t steps() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3);
    }
    unit_id operator[](uint8_t index) const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, index + 4, 2));
    }
    std::vector<uint8_t> board() const
    {
        return bitf::solid::get_vector<uint64_t, uint8_t>(_seed, 4, 2, 2);
    }

    xState& set_agent(unit_id agn)
    {
        _seed = bitf::solid::insert_scalar<uint64_t, uint8_t>(static_cast<uint8_t>(agn), 0, 1, _seed);
        return *this;
    }
    xState& set_steps(uint8_t stp)
    {
        _seed = bitf::solid::insert_scalar<uint64_t, uint8_t>(stp, 1, 3, _seed);
        return *this;
    }
};



/**
 *  2 bits: direction of obligatory movement, 0: skip, 1: move, 2: retreat
 *  3 bits: mobility - number of avaliable movements
 * 25 bits: moves - 5-bit offset movement indici
 */
struct xMoves
{
    static const uint32_t def_state{0b0};
    static uint32_t get_seed(event_id direction, uint8_t mobility, std::vector<uint8_t> moves)
    {
        if (direction > event_id::retreat || mobility > 5 || moves.size() > 5)
        {
            throw std::logic_error("invalid xMoves parameters");
        }
        uint32_t seed{static_cast<uint32_t>(direction)};
        seed = bitf::solid::insert_scalar<uint32_t, uint8_t>(mobility, 2, 3, seed);
        seed = bitf::solid::insert_vector<uint32_t, uint8_t>(moves, 5, 27, seed);
        return seed;
    }
    uint32_t _seed;
    xMoves() : _seed(def_state){};
    xMoves(uint32_t seed) : _seed(seed){};
    xMoves(event_id direction, uint8_t mobility, std::vector<uint8_t> moves)
        : _seed(get_seed(direction, mobility, moves)){};
    xMoves(const xMoves &other) : _seed(other._seed){};
    xMoves(xMoves &&other) : _seed(other._seed){};
    xMoves &operator=(const xMoves &other) { _seed = other._seed; return *this; };
    xMoves &operator=(xMoves &&other) { _seed = other._seed; return *this; };
    ~xMoves() = default;

    uint32_t seed() {return _seed; }
    event_id direction() const
    {
        return static_cast<event_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 2));
    }
    uint8_t mobility() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 2, 3);
    }
    uint8_t operator[](uint8_t index) const
    {
        if (index >= mobility())
        {
            throw std::out_of_range("index > mobility");
        }
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, index + 5, 5);
    }
    std::vector<uint8_t> moves() const
    {
        return bitf::solid::get_vector<uint32_t, uint8_t>(_seed, 5, 5, mobility());
    }
};
/**
 * 1 bit:  active agent
 * 3 bits: event code (see senet::event)
 * 5 bits: movement chain start position
 * 5 bits: movement chain destination
 */
struct xEvent
{
    static const uint16_t def_state{0b0};
    static uint16_t get_seed(unit_id agent, event_id event, uint8_t start, uint8_t dest)
    {
        if (agent == unit_id::empty)
        {
            throw std::logic_error("invalid xEvent parameters");
        }
        uint16_t seed = {static_cast<uint16_t>(agent)};
        seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(static_cast<uint8_t>(event), 1, 3, seed);
        seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(start, 4, 5, seed);
        seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(dest, 9, 5, seed);
        return seed;
    }
    uint16_t _seed;
    xEvent() : _seed(def_state){};
    xEvent(uint32_t seed) : _seed(seed){};
    xEvent(unit_id agent, event_id event, uint8_t start, uint8_t dest)
        : _seed(get_seed(agent, event, start, dest)){};
    xEvent(const xEvent &other) : _seed(other._seed){};
    xEvent(xEvent &&other) : _seed(other._seed){};
    xEvent &operator=(const xEvent &other) { _seed = other._seed; return *this; };
    xEvent &operator=(xEvent &&other) { _seed = other._seed; return *this; };
    ~xEvent() = default;
    uint16_t seed() {return _seed; }

    unit_id agent() const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1));
    }
    event_id code() const
    {
        return static_cast<event_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3));
    }
    uint8_t start() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 4, 5);
    }
    uint8_t destination() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 9, 5);
    }
};

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

    unit_id operator[](uint8_t index) const
    {
        return _state[index];
    }
    std::vector<uint8_t> board() const
    {
        return _state.board();
    }
    unit_id agent() const
    {
        return _state.agent();
    }
    uint8_t steps() const
    {
        return _state.steps();
    }

    std::vector<uint8_t> moves() const
    {
        return _moves.moves();
    }
    event_id direction() const
    {
        return _moves.direction();
    }
    uint8_t mobility() const
    {
        return _moves.mobility();
    }
    
    xEvent event() const
    {
        return _event;
    }

    void set_steps(uint8_t stp)
    {
        _state.set_steps(stp);
    }
    void set_agent(unit_id agn)
    {
        _state.set_agent(agn);
    }
};
} // namespace xtc