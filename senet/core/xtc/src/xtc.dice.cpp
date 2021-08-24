#include "xtc.dice.hpp"
#include <random>
namespace xtc
{
const std::array<double,6> Dice::P = { 0, .25, .375, .25, .0625, .0625 };
int
Dice::roll ()
{
  std::random_device rd;
  int sum = 0;
  for (int i = 0; i < stick_number; i++)
    {
      _sticks[i] = rd () & 1;
      sum += _sticks[i];
    }
  return sum ? sum : 5;
}
}