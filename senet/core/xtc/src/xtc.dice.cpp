#include "xtc.dice.hpp"
#include <random>
namespace xtc
{
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