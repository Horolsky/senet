#include "xtc.emax.hpp"
#include "xtc.dice.hpp"
#include <chrono>
namespace xtc
{

namespace bruteforce
{
typedef struct
{
  Eval eval;
  int stopdepth;
  int jobsdone;
  int timetowork;
  int threads_created;
  int nodes_visited;
  int leaves_visited;
  bool stopflag;

} shared_param;

int get_minimax (double *expectations, int n, bool max);
void threadwork (const ChanceNode &state, double *result, int depth);

double expectation_rec (const ChanceNode &chancenode, int depth);
void expectations_par (const StrategyNode &choicenode,
                       const Strategies &strats, double *expectations,
                       int depth);

static shared_param GLOBAL_STATE;
static const int FRAME = 10;
static const int THREADING_DEPTH = 2;

int
get_best (const StrategyNode &state, int stopdepth, int time)
{
  GLOBAL_STATE.stopflag = false;
  GLOBAL_STATE.stopdepth = stopdepth;
  GLOBAL_STATE.timetowork = time;
  GLOBAL_STATE.eval = Eval ();

  GLOBAL_STATE.nodes_visited = 0;
  GLOBAL_STATE.leaves_visited = 0;
  GLOBAL_STATE.threads_created = 0;

  bool max = state.agent () != Unit::X;
  auto strats = state.strategies ();
  double *expectations = new double[strats.mobility ()];
  expectations_par (state, strats, expectations, -1);
  int choice = get_minimax (expectations, strats.mobility (), max);
  delete[] expectations;
  return choice;
}

int
get_minimax (double *expectations, int n, bool max)
{
  int choice = 0;
  if (n < 2)
    return choice;
  for (int i = 1; i < n; i++)
    {
      if (max)
        {
          choice = expectations[i] > expectations[choice] ? i : choice;
        }
      else
        {
          choice = expectations[i] < expectations[choice] ? i : choice;
        }
    }
  return choice;
}

void
expectations_par (const StrategyNode &choicenode, const Strategies &strats,
                  double *expectations, int depth)
{
  if (strats.seed () == 0)
    throw std::logic_error ("corrupted node");
  if (expectations == nullptr)
    throw std::logic_error ("nullptr expectations");
  if (strats.mobility () < 2)
    return;
  std::thread *threads = new std::thread[strats.mobility ()];

  for (int i = 0; i < strats.mobility (); i++)
    {
      auto chancenode = choicenode.child (strats.indici (i), strats);
      threads[i]
          = std::thread (threadwork, chancenode, expectations + i, depth + 1);
      GLOBAL_STATE.threads_created++;
    }

  int frames = GLOBAL_STATE.timetowork / FRAME + 1;
  while (--frames > 0)
    {
      if (GLOBAL_STATE.jobsdone >= strats.mobility ())
        break;
      std::this_thread::sleep_for (std::chrono::milliseconds (FRAME));
    }
  GLOBAL_STATE.stopflag = true;
  for (int i = 0; i < strats.mobility (); i++)
    {
      threads[i].join ();
    }
  delete[] threads;
}

void
threadwork (const ChanceNode &chancenode, double *result, int depth)
{
  *result = expectation_rec (chancenode, depth);
  if (depth == 0)
    GLOBAL_STATE.jobsdone++;
}

double
expectation_rec (const ChanceNode &chancenode, int depth)
{
  double expect = GLOBAL_STATE.eval (chancenode);
  if (GLOBAL_STATE.stopflag || depth >= GLOBAL_STATE.stopdepth
      || (expect - (long)expect) == 0)
    {
      GLOBAL_STATE.leaves_visited++;
      return expect;
    }
  GLOBAL_STATE.nodes_visited++;
  expect = 0;

  for (int steps = 1; steps < Dice::P.size (); steps++)
    {
      // taking minimax for this choice node
      auto choicenode = chancenode.child (steps);
      auto strats = choicenode.strategies ();
      double *expectations = new double[strats.mobility ()];
      if (expectations == nullptr)
        throw std::logic_error ("unable to allocate expectations");

      if (false) // depth < THREADING_DEPTH) //TODO: test this
        {
          expectations_par (choicenode, strats, expectations, depth + 1);
        }
      else
        {
          if (strats.seed () == 0)
            throw std::logic_error ("corrupted node");
          for (int i = 0; i < strats.mobility (); i++)
            {
              auto chance_subnode
                  = choicenode.child (strats.indici (i), strats);
              expectations[i] = expectation_rec (chance_subnode, depth + 1);
            }
        }
      int minimax_index = get_minimax (expectations, strats.mobility (),
                                       choicenode.agent () != Unit::X);
      double choiceexpect = expectations[minimax_index];
      expect += Dice::P[steps] * choiceexpect;
      delete[] expectations;
    }
  return expect;
}

} // namespace bruteforce

int
Emax::operator() (const StrategyNode &state, int depth, int time) const
{
  if (depth < 6) bruteforce::get_best (state, depth, time);
  else {
    throw std::runtime_error("deep algos not implemented yet");
  }
}

} // namespace xtc