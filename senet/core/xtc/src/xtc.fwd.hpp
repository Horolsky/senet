#pragma once
#include "../lib/bitf/src/bitf_solid.hpp"
#include <cstdint>

using uint8_t = std::uint8_t;
using uint16_t = std::uint16_t;
using uint32_t = std::uint32_t;
using uint64_t = std::uint64_t;

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
class Emax;

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
  PANIC,
  NONE, // for game start
  ERROR // reserved
};
}
