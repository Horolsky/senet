#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include "senet.fwd.hpp"

#include <vector>

namespace senet
{

    /**
 *  1 bit:  agent - 0 or 1
 *  3 bits: steps - 0 to 5
 * 60 bits: board - 2-bit offset cell array, 0: 1-agent, 1: 2-agent, 2: empty
 */
    struct xState
    {
        static const uint64_t def_state{0b010001000100010001000000};
        static uint64_t get_seed(unit_id agent, uint8_t steps, std::vector<uint8_t> board)
        {
            if (agent != unit_id::empty || steps > 5 || board.size() != 30)
            {
                throw std::logic_error("invalid xState parameters");
            }
            uint64_t seed{static_cast<uint64_t>(agent)};
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
        ~xState() = default;
        xState &operator=(const xState &other);
        xState &operator=(xState &&other);
        xState &set_agent(unit_id agn);
        xState &set_steps(uint8_t stp);

        unit_id agent() const;
        uint8_t steps() const;
        unit_id operator[](uint8_t index) const;
        std::vector<uint8_t> board() const;
    };
} // namespace xtc