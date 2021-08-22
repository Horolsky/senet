#pragma once
#include "xtc.fwd.hpp"
#include "xtc.objects.hpp"
#include "xtc.logic.hpp"
#include "xtc.eval.hpp"

namespace xtc
{

// functor class
class Emax
{
public:
  enum class AlgoID
  {
    recursive_brute,
    recursive_star,
    iterative_opt
  };
  static const int coefs_num {4};
  enum class CoefsID
  {
    stepsum,
    mobility,
    priority,
    attack
  };
  static const int max_buffer{ 524288 }; // 0.5 Mb

  Emax () = default;
  Emax (const Emax &other) = default;
  Emax (Emax &&other) = default;
  ~Emax () = default;

  int operator() (const State &state) const;
};

} // namespace xtc
