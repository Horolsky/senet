#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include "xtc.fwd.hpp"

#include <inttypes.h>
#include <vector>

/**
 *  1 bit:  agent - 0 or 1
 *  3 bits: steps - 0 to 5
 * 60 bits: board - 2-bit offset cell array, 0: empty, 1: 1-agent, 2: 0-agent
 */
class xState
{
    const uint64_t _seed;

public:
    static const uint64_t def_state{0b100110011001100110010000};
    static uint64_t get_seed(uint8_t agent, uint8_t steps, std::vector<uint8_t> board)
    {
        if (agent > 1 || steps > 5 || board.size() != 30)
        {
            throw std::logic_error("invalid xState parameters");
        }
        int64_t seed{agent};
        seed = bitf::solid::insert_scalar<uint64_t, uint8_t>(steps, 1, 3, seed);
        seed = bitf::solid::insert_vector<uint64_t, uint8_t>(board, 4, 60, seed);
        return seed;
    }
    xState() : _seed(def_state){};
    xState(uint64_t seed) : _seed(seed){};
    xState(uint8_t agent, uint8_t steps, std::vector<uint8_t> board) : _seed(get_seed(agent, steps, board)) {}
    xState(const xState &other) : _seed(other._seed){};
    xState(xState &&other) : _seed(other._seed){};
    xState &operator=(const xState &other) = delete;
    xState &operator=(xState &&other) = delete;
    ~xState() = default;

    uint8_t agent() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1);
    }
    uint8_t steps() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3);
    }
    uint8_t operator[](uint8_t index) const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, index + 4, 2);
    }
    std::vector<uint8_t> board() const
    {
        return bitf::solid::get_vector<uint64_t, uint8_t>(_seed, 4, 2, 2);
    }
    double utility() const
    {

    }
};

/**
 *  2 bits: direction - 0: no movement, 1: normal, 2: backward
 *  3 bits: mobility - number of avaliable movements
 * 25 bits: moves - 5-bit offset movement indici
 */
class xMoves
{
    const uint32_t _seed;

public:
    static const uint32_t def_state{0b0};
    static uint32_t get_seed(uint8_t direction, uint8_t mobility, std::vector<uint8_t> moves)
    {
        if (direction > 2 || mobility > 5 || moves.size() > 5)
        {
            throw std::logic_error("invalid xMoves parameters");
        }
        uint32_t seed{direction};
        seed = bitf::solid::insert_scalar<uint32_t, uint8_t>(mobility, 2, 3, seed);
        seed = bitf::solid::insert_vector<uint32_t, uint8_t>(moves, 5, 27, seed);
        return seed;
    }
    xMoves() : _seed(def_state){};
    xMoves(uint32_t seed) : _seed(seed){};
    xMoves(uint8_t direction, uint8_t mobility, std::vector<uint8_t> moves)
        : _seed(get_seed(direction, mobility, moves)){};
    xMoves(const xMoves &other) : _seed(other._seed){};
    xMoves(xMoves &&other) : _seed(other._seed){};
    xMoves &operator=(const xMoves &other) = delete;
    xMoves &operator=(xMoves &&other) = delete;
    ~xMoves() = default;

    uint8_t direction() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 2);
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
 * 1 bit:  agent
 * 3 bits: event code
 * 5 bits: movement chain start
 * 5 bits: movement chain destination
 * codes:
 * 0 - drow
 * 1 - reverse
 * 2 - skip
 * 3 - move
 * 4 - attack
 * 5 - escape
 */
class xEvent
{
    uint16_t _seed;

public:
    static const uint16_t def_state{0b0};
    static uint16_t get_seed(uint8_t agent, uint8_t eventcode, uint8_t start, uint8_t dest)
    {
        if (agent > 1 || eventcode > 5)
        {
            throw std::logic_error("invalid xEvent parameters");
        }
        uint16_t seed = {agent};
        seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(eventcode, 1, 3, seed);
        seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(start, 4, 5, seed);
        seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(dest, 9, 5, seed);
        return seed;
    }
    xEvent() : _seed(def_state){};
    xEvent(uint32_t seed) : _seed(seed){};
    xEvent(uint8_t agent, uint8_t eventcode, uint8_t start, uint8_t dest)
        : _seed(get_seed(agent, eventcode, start, dest)){};
    xEvent(const xEvent &other) : _seed(other._seed){};
    xEvent(xEvent &&other) : _seed(other._seed){};
    xEvent &operator=(const xEvent &other) = delete;
    xEvent &operator=(xEvent &&other) = delete;
    ~xEvent() = default;

    uint8_t agent() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1);
    }
    uint8_t code() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3);
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

    uint8_t operator[](uint8_t index) const
    {
        return _state[index];
    }
    std::vector<uint8_t> board() const
    {
        return _state.board();
    }
    uint8_t agent() const
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
    uint8_t direction() const
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
};
