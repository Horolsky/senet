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
   * { 0, .25, .375, .25, .0625, .0625 }
   */
  static const std::array<double,6> P;
  static const int chance_number{ 5 };
  static const int stick_number{ 4 };
  int roll ();
  int operator[] (size_t i) const;
  int* sticks (int* it) const;
  int sticks (size_t i) const;

private:
  std::array<int,4> _sticks { 0, 0, 0, 0 };
};
} // namespace xtc
