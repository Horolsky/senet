#include "xtc.dice.hpp"
#include <random>
namespace xtc
{
const std::array<double, 6> Dice::P = { 0, .25, .375, .25, .0625, .0625 };
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
int*
Dice::sticks (int* buff) const
{
  for (int i =0; i < stick_number; i++)
  {
    buff[i] = _sticks[i];
  }
  return buff;
}
int
Dice::sticks (size_t i) const
{
  if (i >= stick_number)
    throw std::overflow_error ("only 4 sticks in dice");
  return _sticks[i];
}
int
Dice::operator[] (size_t i) const
{
  return sticks(i);
}

}