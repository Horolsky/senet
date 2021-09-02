#include "xtc.emax.hpp"
#include "xtc.dice.hpp"
#include <atomic>
#include <chrono>
namespace xtc
{

namespace bruteforce
{

const int FRAME = 2;
Eval g_eval;
int g_stopdepth;
int g_timetowork;

bool g_stopflag;
int g_jobsdone;
int g_threads_created;

std::atomic<int> a_nodes_visited;
std::atomic<int> a_leaves_visited;

int get_best_strategy (const StrategyNode &state, int stopdepth, int time);
void threadwork (const ChanceNode &state, double *result, int depth);
double expectation_recursive (const ChanceNode &chancenode, int depth);
int get_minimax (double *expectations, int n, bool max);

int
get_best_strategy (const StrategyNode &state, int stopdepth, int time)
{
  g_eval = Eval ();
  g_stopflag = false;
  g_stopdepth = stopdepth;
  g_timetowork = time;

  g_threads_created = 0;
  
  a_nodes_visited = 0;
  a_leaves_visited = 0;
  

  bool max = state.agent () != Unit::X;
  auto strats = state.strategies ();
  if (strats.seed () == 0)
    throw std::logic_error ("corrupted node, seed: " + std::to_string(state.seed()));
  if (strats.mobility () < 2)
    return 0;

  double *expectations = new double[strats.mobility ()];
  std::thread *threads = new std::thread[strats.mobility ()];

  for (int i = 0; i < strats.mobility (); i++)
    {
      auto chancenode = state.choice (strats.indici (i), strats);
      threads[i]
          = std::thread (threadwork, chancenode, expectations + i, 0);
      g_threads_created++;
    }

  int frames = g_timetowork / FRAME + 1;
  while (--frames > 0)
    {
      if (g_jobsdone >= strats.mobility ())
        break;
      std::this_thread::sleep_for (std::chrono::milliseconds (FRAME));
    }
  g_stopflag = true;
  for (int i = 0; i < strats.mobility (); i++)
    {
      threads[i].join ();
    }

  int choice = get_minimax (expectations, strats.mobility (), max);
  delete[] threads;
  delete[] expectations;
  return choice;
}

double
expectation_recursive (const ChanceNode &chancenode, int depth)
{
  double expect = g_eval (chancenode);
  if (g_stopflag || depth >= g_stopdepth || (expect - (long)expect) == 0)
    {
      a_leaves_visited++;
      return expect;
    }
  a_nodes_visited++;
  expect = 0;

  for (int steps = 1; steps < Dice::P.size (); steps++)
    {
      // taking minimax for this choice node
      auto choicenode = chancenode.chance (steps);
      auto strats = choicenode.strategies ();
      double *expectations = new double[strats.mobility ()];
      if (expectations == nullptr)
        {
          throw std::logic_error ("unable to allocate expectations");
        }
      if (strats.seed () == 0)
        {
          throw std::logic_error ("corrupted node, seed: " + std::to_string(choicenode.seed()));
        }
      for (int i = 0; i < strats.mobility (); i++)
        {
          auto chance_subnode = choicenode.choice (strats.indici (i), strats);
          expectations[i] = expectation_recursive (chance_subnode, depth + 1);
        }

      int minimax_index = get_minimax (expectations, strats.mobility (),
                                       choicenode.agent () != Unit::X);
      double choiceexpect = expectations[minimax_index];
      expect += Dice::P[steps] * choiceexpect;
      delete[] expectations;
    }
  return expect;
}

void
threadwork (const ChanceNode &chancenode, double *result, int depth)
{
  *result = expectation_recursive (chancenode, depth);
  if (depth == 0)
    g_jobsdone++;
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

} // namespace bruteforce

int
Emax::operator() (const StrategyNode &node) const
{
  int choice = 0;
  if (_depth < 8)
    choice = bruteforce::get_best_strategy (node, _depth, _time);
  else
    {
      throw std::runtime_error ("max depth exceeded");
    }
  return choice;
}

int
Emax::test_call (const StrategyNode &node, int *nodes, int *leaves,
                  int *threads, int *jobs) const
{
  int choice = bruteforce::get_best_strategy (node, _depth, _time);
  *nodes = bruteforce::a_nodes_visited;
  *leaves = bruteforce::a_leaves_visited;
  *threads = bruteforce::g_threads_created;
  *jobs = bruteforce::g_jobsdone;
  return choice;
}

} // namespace xtc