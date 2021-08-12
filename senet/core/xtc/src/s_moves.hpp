#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include "xtc.fwd.hpp"

#include <vector>

namespace xtc
{
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
        ~xMoves() = default;
        xMoves &operator=(const xMoves &other);
        xMoves &operator=(xMoves &&other);
        event_id direction() const;
        uint8_t mobility() const;
        uint8_t operator[](uint8_t index) const;
        std::vector<uint8_t> moves() const;
    };
} // namespace xtc