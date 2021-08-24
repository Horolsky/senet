#include "xtc.emax.hpp"
#include "xtc.dice.hpp"
#include <chrono>
namespace xtc
{
int
zerodepth_brute (const State &state, double *coefs, Rules rules, bool max)
{
  const FuncStrategies get_strategies = FuncStrategies (rules);
  const FuncIncrement increment = FuncIncrement (rules);

  Strategies strategies = get_strategies (state);
  double util = max ? 0 : 1;
  int res = 0;
  for (int i = 0; i < strategies.mobility (); i++)
    {
      double sutil = Eval::expectation (
          increment (state, strategies.indici (i), strategies));
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

typedef struct
{
  FuncStrategies getstrats;
  FuncIncrement increment;
  Eval eval;
  int stopdepth;
  int jobsdone;
  int timetowork;
  int threads_created;
  int nodes_visited;
  int leaves_visited;
  bool stopflag;
  
} shared_param;

static shared_param GLOBAL_STATE;
static const int FRAME = 10;
static const int THREADING_DEPTH = 2;
void threadwork (const State &state, double *result, int depth);
double rec_emax (const State &chancenode, int depth);
int get_minimax(double *expectations, int n, bool max);
void load_expectations_par (const State &choicenode, const Strategies &strats, double* expectations, int depth);

int
recursive_brute (const State &state, Rules rules, int stopdepth, int time)
{
  GLOBAL_STATE.stopflag = false;
  GLOBAL_STATE.stopdepth = stopdepth;
  GLOBAL_STATE.timetowork = time;
  GLOBAL_STATE.getstrats = FuncStrategies (rules);
  GLOBAL_STATE.increment = FuncIncrement (rules);
  GLOBAL_STATE.eval = Eval ();

  GLOBAL_STATE.nodes_visited = 0;
  GLOBAL_STATE.leaves_visited = 0;
  GLOBAL_STATE.threads_created = 0;
  

  bool max = state.agent () != Unit::X;
  auto strats = GLOBAL_STATE.getstrats (state);
  double* expectations = new double[strats.mobility()];
  load_expectations_par (state, strats, expectations, -1);
  int choice = get_minimax(expectations, strats.mobility (), max);
  delete[] expectations;
  return choice;
}

int get_minimax(double *expectations, int n, bool max)
{
  int choice = 0;
  if (n < 2) return choice;
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
load_expectations_par (const State &choicenode, const Strategies &strats, double* expectations, int depth)
{
  if (strats.seed() == 0) throw std::logic_error("corrupted node");
  if (expectations == nullptr) throw std::logic_error("nullptr expectations");
  if (strats.mobility () < 2)
    return;
  std::thread *threads = new std::thread[strats.mobility ()];

  for (int i = 0; i < strats.mobility (); i++)
    {
      auto chancenode = GLOBAL_STATE.increment (choicenode, strats.indici(i), strats);
      threads[i] = std::thread(threadwork, chancenode, expectations + i, depth+1);
      GLOBAL_STATE.threads_created++;
    }

  int frames = GLOBAL_STATE.timetowork / FRAME + 1;
  while (--frames > 0)
    {
      if (GLOBAL_STATE.jobsdone >= strats.mobility ())
        break;
      std::this_thread::sleep_for (std::chrono::milliseconds(FRAME));
    }
  GLOBAL_STATE.stopflag = true;
  for (int i = 0; i < strats.mobility (); i++)
    {
      threads[i].join ();
    }
  delete[] threads;
}

void
threadwork (const State &chancenode, double *result, int depth)
{
  *result = rec_emax (chancenode, depth);
  if (depth == 0)
    GLOBAL_STATE.jobsdone++;
}

double
rec_emax (const State &chancenode, int depth)
{
  double expect = GLOBAL_STATE.eval (chancenode);
  if (GLOBAL_STATE.stopflag || depth >= GLOBAL_STATE.stopdepth
      || (expect-(long)expect) == 0)
    {
      GLOBAL_STATE.leaves_visited++;
      return expect;
    }
  GLOBAL_STATE.nodes_visited++;
  auto strats = GLOBAL_STATE.getstrats (chancenode);
  expect = 0;

  for (int steps = 1; steps < Dice::P.size (); steps++)
    {
      //taking minimax for this choice node      
      auto choicenode = State (chancenode, steps);
      auto strats = GLOBAL_STATE.getstrats (choicenode);
      double *expectations = new double[strats.mobility()];
      if (expectations == nullptr) throw std::logic_error("unable to allocate expectations");


      if (false)//depth < THREADING_DEPTH) //TODO: test this
        {
          load_expectations_par (choicenode, strats, expectations, depth + 1);
        }
      else
        { 
          if (strats.seed() == 0) throw std::logic_error("corrupted node");
          for (int i = 0; i < strats.mobility (); i++)
            {
              auto chance_subnode = GLOBAL_STATE.increment (choicenode, strats.indici(i), strats);
              expectations[i] = rec_emax(chance_subnode, depth+1);
            }
        }
      int minimax_index = get_minimax(expectations, strats.mobility(), choicenode.agent() != Unit::X);
      double choiceexpect = expectations[minimax_index];
      expect += Dice::P[steps] * choiceexpect;
      delete[] expectations;
    }
    return expect;
}

int
Emax::operator() (const State &state, int depth, int time) const
{
  
  return recursive_brute(state, Rules::KENDALL, depth, time);
}
}