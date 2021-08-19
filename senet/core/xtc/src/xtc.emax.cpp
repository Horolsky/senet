#pragma once
// #include "util.deque.hpp"
#include "xtc.common.hpp"
#include "xtc.internal.hpp"
namespace xtc
{

  int Emax::operator()(State state)
  {
    return 0;
  }
}
/*

iterativePreorder(node)
if (node == null)
return
stack ← empty stack
stack.push(node)
while (not stack.isEmpty())
node ← stack.pop()
visit(node)
//right child is pushed first so that left is processed first
if node.right ≠ null
  stack.push(node.right)
if node.left ≠ null
  stack.push(node.left)
  */


// float eval (uint64_t);
// uint64_t increment (uint64_t);
// int get_strategies(uint64_t state, unsigned int* buff, int steps);
// static cloc_t stoptime;
// static int stopdepth;


// double
// __expectimax (xtc::state::type origin, int depth)
// {
    // if (depth < 1) throw std::logic_error("non-positive  expectimax depth");
//   
//   origin = set_depth(origin, 0);
//   deque<uint64_t, MAX_DEQUE> deque;
//   deque.push_front (origin);
//   
//   while (!deque.empty () &&)
    // {
    //   auto node = deque.pop_back ();
// 
    //   for (int i = 0; i < MAX_STEPS; i++)
        // {
        //   float p = P[i];
        //   unsigned int strategies[MAX_BRANCHING];
        //   int mobility = get_strategies(node, strategies, i+1); // change content of strategies
// 
        //   for (int i = 0; i < mobility; i++)
            // {
            //   auto subnode = increment (node, i);
            // }
        // }
    // }
// }

// single branch
/*
float
_expectimax (state::type state, uint8_t depth, const uint8_t stopdepth, const clock_t stoptime)
{

  float util = eval (state);

  if (util == 1 || util == 0 || depth >= stopdepth || clock () > stoptime)
    {
      return util;
    }

  depth++;
  util = 0;
  
  for (uint16_t chance = 0; chance < 5; chance++)
    {
        float cur_util = 0; // chance util
        state::type cur_state = 
        moves::type cur_moves;
      cur_util = 0;
      cur_state._seed = seed;
      cur_state._steps = chance + 1;
      cur_moves._seed = _get_moves (cur_state._seed);

      for (ui8 move = 0; move < cur_moves._len; move++)
        {
          ui64 childstate = _increment (cur_state, cur_moves,
                                        MOVES_GET (cur_moves, move));
          cur_util += _expectimax (childstate, depth);
        }
      util += cur_moves._len != 0 ? cur_util * P[chance] / cur_moves._len
                                     : cur_util * P[chance];
    }
  return util;
}
} // namespace algo
} // namespace xtc
*/