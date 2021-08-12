#include "s_state.hpp"

#include <vector>

namespace xtc
{
    xState &xState::operator=(const xState &other)
    {
        _seed = other._seed;
        return *this;
    };
    xState &xState::operator=(xState &&other)
    {
        _seed = other._seed;
        return *this;
    };

    unit_id xState::agent() const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1));
    }
    uint8_t xState::steps() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3);
    }
    unit_id xState::operator[](uint8_t index) const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, index + 4, 2));
    }
    std::vector<uint8_t> xState::board() const
    {
        return bitf::solid::get_vector<uint64_t, uint8_t>(_seed, 4, 2, 2);
    }

    xState &xState::set_agent(unit_id agn)
    {
        _seed = bitf::solid::insert_scalar<uint64_t, uint8_t>(static_cast<uint8_t>(agn), 0, 1, _seed);
        return *this;
    }
    xState &xState::set_steps(uint8_t stp)
    {
        _seed = bitf::solid::insert_scalar<uint64_t, uint8_t>(stp, 1, 3, _seed);
        return *this;
    }
} // namespace xtc