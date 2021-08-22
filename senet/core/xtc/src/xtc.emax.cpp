#include "xtc.emax.hpp"
namespace xtc
{
  int zerodepth_brute(const State& state, double* coefs, Rules rules, bool max)
  {
    const FuncStrategies get_strategies = FuncStrategies(rules);
    const FuncIncrement increment = FuncIncrement(rules);
    
    Strategies strategies = get_strategies(state);
    double util = max ? 0 : 1;
    int res = 0;
    for (int i = 0; i < strategies.mobility(); i++)
    {
      double sutil = Eval::expectation(increment(state, strategies.indici(i), strategies));
      if (max)
      {
        util = sutil > util ? sutil : util;
        res = i;
      }
      else
      {
        util = sutil < util ? sutil : util;
        res = i;
      }
      return res;
    }
  }


  int Emax::operator()(const State &state) const
  {
    bool max = static_cast<bool>(state.agent());
    return zerodepth_brute(state, nullptr, Rules::KENDALL, max);
  }
}