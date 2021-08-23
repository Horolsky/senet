#pragma once
#include "xtc.fwd.hpp"
#include "xtc.objects.hpp"
#include <array>

namespace xtc
{

class Eval
{
  public:
  enum class Func
  {
    BASIC,
    LINEAR,
    POLYNOMIAL
  };
  static const int coef_n {4};
  static double expectation(const State &state);
  static double lin_eval(const State &state, const std::array<double,coef_n> coefs);
  static double poly_eval(const State &state, const std::array<double,coef_n> coefs);
  

  Eval() = default;
  Eval(Eval::Func func_id, std::array<int,coef_n> coefs = {1,0,0,0}) 
  {
    std::array<double, coef_n> norm_coefs;
    int sum = 0;
    for (int c : coefs) { sum += c; }
    for (int i = 0; i < coef_n; i++)
    {
      norm_coefs[i] = ((double)coefs[i]) / sum;
    }
    _func_id = func_id;
    _coefs = norm_coefs;
  }
  
  double operator()(const State& state) const 
  {
    switch (_func_id)
    {
    case Func::BASIC:
      return expectation(state);
      break;
    case Func::LINEAR:
      return lin_eval(state, _coefs);
      break;
    case Func::POLYNOMIAL:
      return poly_eval(state, _coefs);
      break;
    default:
      break;
    }
  }
  private:
  Eval::Func _func_id;
  std::array<double,coef_n> _coefs;
};


} //namespace xtc

