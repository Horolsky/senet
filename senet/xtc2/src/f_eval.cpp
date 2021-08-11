#include "f_eval.hpp"
namespace senet
{
    inline float evaluate_state(const xState &state)
    {
        return evaluate_state(state._seed);
    }

    float evaluate_state(const uint64_t seed)
    {
        uint8_t maxSum = 0;
        uint8_t minSum = 0;
        for (uint8_t i = 4; i < 64; i += 2)
        {
            uint8_t cell = (seed >> i) & 0b11;
            if (cell == 0)
                maxSum += (30 - i);
            else if (cell == 1)
                minSum += (30 - i);
        }
        float res;
        if (minSum == 0)
            res = 0;
        else if (maxSum == 0)
            res = 0;
        else
            res = (float)(minSum) / (maxSum + minSum);
        return res;
    }
} // namespace senet
