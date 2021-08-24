/**
 * Senet Dice class
 */
#pragma once
#include "xtc.fwd.hpp"
#include <array>
namespace xtc
{

/**
 * Senet stick dice
 */
class Dice
{
public:
  /**
   * probabilities of roll output [1-5]
   * with P[0] = 0 for impossible 0 output
   */
  static constexpr std::array<double,6> P { 0, .25, .375, .25, .0625, .0625 };
  // static const double P(size_t index);
  static const int chance_number{ 5 };
  static const int stick_number{ 4 };
  int roll ();
  int
  operator[] (size_t i) const
  {
    if (i >= stick_number)
      throw std::overflow_error ("only 4 sticks in dice");
    return _sticks[i];
  }

private:
  std::array<int,4> _sticks { 0, 0, 0, 0 };
};

} // namespace xtc
