#pragma once
#include "xtc.fwd.hpp"
#include "xtc.objects.hpp"

namespace xtc
{

class Eval
{
  public:
  static double expectation(const State &state);
};

} //namespace xtc