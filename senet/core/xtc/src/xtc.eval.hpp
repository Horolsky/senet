#pragma once
#include "xtc.fwd.hpp"
#include "xtc.objects.hpp"
#include <array>

namespace xtc
{

class Eval
{
public:
  enum class FuncID
  {
    BASIC,
    LINEAR,
    POLYNOMIAL
  };
  enum class CoefID
  {
    stepsum,
    mobility,
    priority,
    attack
  };
  static const int coef_n{ 4 };
  static double expectation (const State &state);
  static double lin_eval (const State &state,
                          const std::array<double, coef_n> &coefs);
  static double poly_eval (const State &state,
                           const std::array<double, coef_n> &coefs);

  Eval () = default;
  Eval (Eval::FuncID func_id, std::array<int, coef_n> coefs = { 1, 0, 0, 0 });
  Eval (const Eval &other) : _func_id(other._func_id), _coefs(other._coefs) {};
  Eval (Eval &&other) : _func_id(other._func_id), _coefs(std::move(other._coefs)) {};
  
  double operator() (const State &state) const;
  double coef (CoefID) const;

private:
  Eval::FuncID _func_id;
  std::array<double, coef_n> _coefs;
};

} // namespace xtc
