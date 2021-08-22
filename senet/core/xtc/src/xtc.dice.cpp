#include "xtc.objects.hpp"
#include <random>
namespace xtc
{

const double
Dice::P (size_t index)
{
  if (index > chance_number) throw std::overflow_error("up to 5 chances only");
  static const double prob[] = { 0, .25, .375, .25, .0625, .0625 };
  return prob[index];
}
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