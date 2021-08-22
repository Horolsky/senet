/**
 * game logic functors declarations
 * - FuncStrategies
 * - FuncIncrement
 */
#pragma once
#include "xtc.fwd.hpp"
#include "xtc.objects.hpp"

namespace xtc
{
/**
 * functor class
 * pass state and steps
 * if steps = 0 (default), steps parameter is derived from state,
 * if state is chance node and steps = 0,
 * Strategies(0UL) are returned
 */
class FuncStrategies
{
  public:
  static Strategies kendall (const State state, int steps = 0);
  static Strategies meub (const State state, int steps = 0);

  FuncStrategies() = default;
  FuncStrategies(Rules rules) : _rules(rules) {};
  ~FuncStrategies() = default;
  
  Strategies operator() (const State state, int steps = 0) const;
  Rules rules() const;
  private:
  Rules _rules {Rules::KENDALL};
};

class FuncIncrement
{
  public:
  static State kendall (const State state, const int choice, Strategies strategies = Strategies(0UL));
  static State meub (const State state, const int choice, Strategies strategies = Strategies(0UL));

  FuncIncrement() = default;
  FuncIncrement(Rules rules) : _rules(rules) {};
  ~FuncIncrement() = default;
  
  State operator() (const State state, const int choice, Strategies strategies = Strategies(0UL)) const;
  Rules rules() const;
  private:
  Rules _rules {Rules::KENDALL};
};

} // namespace xtc
