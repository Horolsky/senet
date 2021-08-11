#include "s_moves.hpp"

namespace senet
{
    xMoves &xMoves::operator=(const xMoves &other)
    {
        _seed = other._seed;
        return *this;
    };
    xMoves &xMoves::operator=(xMoves &&other)
    {
        _seed = other._seed;
        return *this;
    };

    event_id xMoves::direction() const
    {
        return static_cast<event_id>(bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 0, 2));
    }
    uint8_t xMoves::mobility() const
    {
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, 2, 3);
    }
    uint8_t xMoves::operator[](uint8_t index) const
    {
        if (index >= mobility())
        {
            throw std::out_of_range("index > mobility");
        }
        return bitf::solid::get_scalar<uint64_t, uint8_t>(_seed, index + 5, 5);
    }
    std::vector<uint8_t> xMoves::moves() const
    {
        return bitf::solid::get_vector<uint32_t, uint8_t>(_seed, 5, 5, mobility());
    }
} // namespace xtc