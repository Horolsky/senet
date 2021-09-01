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
    RECURSIVE,
    ITERATIVE
  };
  static const int coefs_num {4};
  
  static const int max_buffer{ 524288 }; // 0.5 Mb

  Emax () = default;
  Emax (Eval eval, AlgoID algo, int depth, int time) 
  : _eval(eval), _algo(algo), _depth(depth), _time(time) {};
  Emax (const Emax &other) 
  {
    Emax tmp {other};
    std::swap(_eval, tmp._eval);
    std::swap(_algo, tmp._algo);
    std::swap(_depth, tmp._depth);
    std::swap(_time, tmp._time);
    std::swap(_threads, tmp._threads);
  }
  Emax (Emax &&other) 
  {
    std::swap(_eval, other._eval);
    std::swap(_algo, other._algo);
    std::swap(_depth, other._depth);
    std::swap(_time, other._time);
    std::swap(_threads, other._threads);
  }
  ~Emax () = default;

  int operator() (const StrategyNode &state) const;
  int test_call (const StrategyNode &node, int* nodes, int* leaves, int* threads, int* jobs) const;
private:
Eval _eval {Eval()};
std::array<std::thread, Strategies::max_strategies> _threads;
int _depth {4};
int _time {1000};
AlgoID _algo {AlgoID::RECURSIVE};
};

} // namespace xtc
