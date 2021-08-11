#include "s_event.hpp"

namespace senet
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

    unit_id xEvent::agent() const
    {
        return static_cast<unit_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 1));
    }
    event_id xEvent::code() const
    {
        return static_cast<event_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 1, 3));
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