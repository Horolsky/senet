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
  static const int coef_n{ 4 };
  static double expectation (const State &state);
  static double lin_eval (const State &state,
                          const std::array<double, coef_n> &coefs);
  static double poly_eval (const State &state,
                           const std::array<double, coef_n> &coefs);

  Eval () = default;
  Eval (Eval::Func func_id, std::array<int, coef_n> coefs = { 1, 0, 0, 0 });
  double operator() (const State &state) const;
  double coef (int i) const;

private:
  Eval::Func _func_id;
  std::array<double, coef_n> _coefs;
};

} // namespace xtc
