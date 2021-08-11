#pragma once
#include <cstdint>
namespace senet
{
    using uint8_t = std::uint8_t;
    using uint16_t = std::uint16_t;
    using uint32_t = std::uint32_t;
    using uint64_t = std::uint64_t;

    //board cell unit
    enum class unit_id : uint8_t
    {
        X,
        Y,
        empty
    };
    //move event type
    enum class event_id : uint8_t
    {
        skip,
        move,
        retreat,
        attack,
        escape,
        drow
    };

    class xState;
    class xMoves;
    class xEvent;
    class xPly;

    float evaluate_state(const xState &);
    float evaluate_state(const uint64_t);

    //game rules
    enum class rules_id : std::uint8_t
    {
        meub,
        kendall
    };
    xMoves available_moves(const xState &, rules_id);
    xState increment_state(const xState &, rules_id);

    uint32_t available_moves_kendall(const uint64_t);
    uint32_t available_moves_meub(const uint64_t);
    uint64_t increment_state_kendall(const uint64_t);
    uint64_t increment_state_meub(const uint64_t);
}
