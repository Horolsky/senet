#pragma once
#include "xtc.common.hpp"

_XTC_BULK_EXTERN (int *)
_XTC_SCALAR_EXTERN (int)

namespace xtc
{

// namespace House;
// enum class Unit;
// enum class Action;
// enum class Rules;

// GAME OBJECTS

class State;
class Moves;
class Event;
class Emax;

namespace cnst
{
const uint64_t def_state_seed{
  0b1010101010101010101010101010101010101010010001000100010001000000
};

const size_t agent_offset{ 1UL };
const size_t steps_offset{ 3UL };
const size_t board_offset{ 2UL };
const size_t indici_offset{ 5UL };
const size_t actions_offset{ 5UL };

const size_t board_size{ 30UL };
const size_t max_moves{ 7UL };

// chance steps probabilities
const float chance_prob[6]{ 0, .25, .375, .25, .0625, .0625 };
// 0.5 Mb
const int max_deque{ 524288 };
const int max_branching{ 7 };

} // namespace cnst

namespace House
{
const int REBIRTH = 14;
const int BEAUTY = 25;
const int WATERS = 26;
const int TRUTHS = 27;
const int ATOUM = 28;
const int SCARAB = 29;
const int NETHER = 30;
const int SKIPTURN = 0b11111;
}

// board cell unit
enum class Unit
{
  X,
  Y,
  NONE
};

// move event type
enum class Action
{
  SKIP,
  MOVE,
  RETREAT,
  SWAPBACK,
  DROW,
  ATTACK,
  ATTACK_HOUSE,
  ESCAPE
};

// game rules
enum class Rules
{
  MEUB,
  KENDALL
};

class State
{
  /**
   * 1 bit: agent
   * 3 bits: steps
   * 30x2bits: board
   */
  typedef struct
  {
    union
    {
      uint64_t seed;
      struct
      {
        uint64_t agent : 1;  // current ply active agent
        uint64_t steps : 3;  // 1-5, 0 for unset
        uint64_t board : 60; // 30x array,
      };
    };
  } bitfield;
  bitfield _data{ .seed = 0UL };

  Moves moves_kendall () const;
  Moves moves_meub () const;
  State increment_kendall (int, Moves) const;
  State increment_meub (int, Moves) const;
  void update_board (int index, Unit unit);

public:
  friend class Emax;
  using seed_type = uint64_t;
  using increment_f = State (State::*) (int, Moves) const;
  using moves_f = Moves (State::*) () const;
  static uint64_t build_seed (Unit agent, int steps, int *board);
  static moves_f get_moves_f (Rules);
  static increment_f get_increment_f (Rules);

  State () { _data.seed = cnst::def_state_seed; }
  State (uint64_t seed) { _data.seed = seed; }
  State (const State &other) : _data (other._data) {}
  State (State &&other) : _data (other._data) {}

  State (Unit agent, int steps, int *board)
  {
    _data.seed = build_seed (agent, steps, board);
  }

  ~State () = default;

  State &operator= (const State &other);
  State &operator= (State &&other);
  void set_agent (Unit agent);
  void set_steps (int steps);

  int *board (int *buffer) const;
  Unit board (int) const;

  Unit agent () const;
  int steps () const;
  float expectation () const;
  uint64_t seed () const;
}; // class state

class Moves
{
  /**
   * 1 bit: agent
   * 3 bits: mobility
   * 7x5bits: indici
   * 7x3bits: actions
   */
  typedef struct
  {
    union
    {
      uint64_t seed;
      struct
      {
        uint64_t agent : 1;
        uint64_t mobility : 3;
        uint64_t direction : 4;
        uint64_t indici : 35;
        uint64_t actions : 21;
      };
    };
  } bitfield;
  bitfield _data{ .seed = 0UL };
  int add_move (int index, Action action);

public:
  friend class State;
  friend class Emax;
  using seed_type = uint64_t;
  static uint64_t build_seed (Unit agent, Action direction, int mobility,
                              int *indici, int *actions);
  static uint64_t build_seed (Unit agent, Action direction, int mobility,
                              int *indici);

  Moves () = default;
  Moves (uint64_t seed) { _data.seed = seed; }
  Moves (const Moves &other) : _data (other._data) {}
  Moves (Moves &&other) : _data (other._data) {}

  Moves (Unit agent, Action direction, int mobility, int *indici, int *actions)
  {
    _data.seed = build_seed (agent, direction, mobility, indici, actions);
  }

  Moves (Unit agent, Action direction, int mobility, int *indici)
  {
    _data.seed = build_seed (agent, direction, mobility, indici);
  }

  Moves (const State &state, Rules rules)
  {
    State::moves_f f = State::get_moves_f (rules);
    _data = (state.*f) ()._data;
  }

  ~Moves () = default;

  Moves &operator= (const Moves &other);
  Moves &operator= (Moves &&other);

  int *indici (int *buffer) const;
  int indici (int index) const;
  int *actions (int *buffer) const;
  Action actions (int index) const;

  Unit agent () const;
  int mobility () const;
  Action direction () const;
  bool contains (int) const;
  uint64_t seed () const;
}; // class moves

class Event
{
  /**
   * 1 bit: agent
   * 3 bits: mobility
   * 7x5bits: indici
   * 7x3bits: actions
   */
  typedef struct
  {
    union
    {
      uint16_t seed;
      struct
      {
        uint16_t agent : 1;
        uint16_t action : 5;
        uint16_t start : 5;
        uint16_t destination : 5;
      };
    };
  } bitfield;
  bitfield _data{ .seed = 0UL };

public:
  friend class State;
  friend class Emax;
  using seed_type = uint16_t;
  static uint64_t build_seed (Unit agent, Action action, int start,
                              int destination);

  Event () = default;
  Event (uint16_t seed) { _data.seed = seed; }
  Event (const Event &other) : _data (other._data) {}
  Event (Event &&other) : _data (other._data) {}

  Event (Unit agent, Action action, int start, int destination)
  {
    _data.seed = build_seed (agent, action, start, destination);
  }

  ~Event () = default;

  Event &operator= (const Event &other);
  Event &operator= (Event &&other);

  Unit agent () const;
  Action action () const;
  int start () const;
  int destination () const;
  uint16_t seed () const;
}; // class event

// functor class
class Emax
{
public:
  Emax () = default;
  Emax (const Emax &other) = default;
  Emax (Emax &&other) = default;
  ~Emax () = default;

  int operator() (State state);
};

} // namespace xtc
