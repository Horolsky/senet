#pragma once
#include "xtc.fwd.hpp"
#include "xtc.state.hpp"
#include "xtc.strategies.hpp"
#include "xtc.eval.hpp"
#include <array>
#include <thread>


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
  
  static const int max_buffer{ 524288 }; // 0.5 Mb

  Emax () = default;
  Emax (const Emax &other) = default;
  Emax (Emax &&other) = default;
  ~Emax () = default;

  int operator() (const StrategyNode &state, int depth=4, int time=100) const;
private:
Eval _eval;
AlgoID _algo;
std::array<std::thread, Strategies::max_strategies> _threads;
};

} // namespace xtc
