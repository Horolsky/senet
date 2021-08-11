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

    enum class game_rules;
    double evaluate_state(const xState &, game_rules);
    xState increment_state(const xState &, game_rules);
}
