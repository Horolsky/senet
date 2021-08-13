#include "s_event.hpp"

namespace xtc
{
    xEvent &xEvent::operator=(const xEvent &other)
    {
        _seed = other._seed;
        return *this;
    };
    xEvent &xEvent::operator=(xEvent &&other)
    {
        _seed = other._seed;
        return *this;
    };

    uint16_t xEvent::seed() const
    {
        return _seed;
    }
    unit_id xEvent::agent() const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1));
    }
    action_id xEvent::action() const
    {
        return static_cast<action_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3));
    }
    uint8_t xEvent::start() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 4, 5);
    }
    uint8_t xEvent::destination() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 9, 5);
    }
} // namespace xtc