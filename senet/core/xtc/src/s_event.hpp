#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include "xtc.fwd.hpp"

namespace xtc
{
    /**
 * 1 bit:  active agent
 * 3 bits: action code (see xtc::action_id)
 * 5 bits: movement chain start position
 * 5 bits: movement chain destination
 */
    struct xEvent
    {
        static const uint16_t def_state{0b0};
        static uint16_t get_seed(unit_id agent, action_id action, uint8_t start, uint8_t dest)
        {
            if (agent == unit_id::empty)
            {
                throw std::logic_error("invalid xEvent parameters");
            }
            uint16_t seed = {static_cast<uint16_t>(agent)};
            seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(static_cast<uint8_t>(action), 1, 3, seed);
            seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(start, 4, 5, seed);
            seed = bitf::solid::insert_scalar<uint16_t, uint8_t>(dest, 9, 5, seed);
            return seed;
        }
        uint16_t _seed;
        xEvent() : _seed(def_state){};
        xEvent(uint32_t seed) : _seed(seed){};
        xEvent(unit_id agent, action_id action, uint8_t start, uint8_t dest)
            : _seed(get_seed(agent, action, start, dest)){};
        xEvent(const xEvent &other) : _seed(other._seed){};
        xEvent(xEvent &&other) : _seed(other._seed){};
        ~xEvent() = default;

        xEvent &operator=(const xEvent &other);
        xEvent &operator=(xEvent &&other);
        
        uint16_t seed() const;
        unit_id agent() const;
        action_id action() const;
        uint8_t start() const;
        uint8_t destination() const;
    };
} // namespace xtc