#include "xtc.eval.hpp"

namespace xtc
{

double
Eval::expectation (const State &state)
{
  uint64_t seed = state.seed ();
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

double
Eval::lin_eval (const State &state, const std::array<double, coef_n> &coefs)
{
  return .5;
}
double
Eval::poly_eval (const State &state, const std::array<double, coef_n> &coefs)
{
  return .5;
}

Eval::Eval (Eval::Func func_id, std::array<int, coef_n> coefs)
{
  std::array<double, coef_n> norm_coefs;
  int sum = 0;
  for (int c : coefs)
    {
      sum += c;
    }
  for (int i = 0; i < coef_n; i++)
    {
      norm_coefs[i] = ((double)coefs[i]) / sum;
    }
  _func_id = func_id;
  _coefs = norm_coefs;
}

double
Eval::operator() (const State &state) const
{
  switch (_func_id)
    {
    case Func::BASIC:
      return expectation (state);
      break;
    case Func::LINEAR:
      return lin_eval (state, _coefs);
      break;
    case Func::POLYNOMIAL:
      return poly_eval (state, _coefs);
      break;
    default:
      break;
    }
}
double
Eval::coef (int i) const
{
  return _coefs[i];
}
} // namespace xtc