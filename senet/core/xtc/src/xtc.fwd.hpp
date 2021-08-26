#pragma once
#include "xtc.common.hpp"

_XTC_BULK_EXTERN (int *)
_XTC_SCALAR_EXTERN (int)

namespace xtc
{
/* GAME OBJECTS */

class State;
  class ChanceNode;
  class StrategyNode;
class Strategies;
class Dice;

/* ALGO FUNCTORS*/

class Eval;
class Expectimax;

/* CONSTANTS & ENUMS */

// special board cells indici
namespace House
{
const int REBIRTH = 14;
const int BEAUTY = 25;
const int WATERS = 26;
const int TRUTHS = 27;
const int ATOUM = 28;
const int SCARAB = 29;
const int NETHER = 30;
const int ERROR = 30; //sic, for use in Strategies::indici
const int SKIPTURN = 31;
}

// board cell unit
enum class Unit
{
  X,   // minimixer player
  Y,   // maximizer player
  NONE // empty cell mark
};

// game event type
enum class Action
{
  SKIP,
  MOVE,
  RETREAT,
  SWAPBACK,
  DROW,
  ATTACK,
  ATTACK_HOUSE,
  ESCAPE,
  NONE, // for game start
  ERROR // reserved
};
}
