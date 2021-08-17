#pragma once
#include "xtc.common.hpp"

namespace xtc
{
namespace constants
{
const uint64_t def_state{
  0b1010101010101010101010101010101010101010010001000100010001000000
};

// const int agent_index{ 0 };
// const int steps_index{ 1 };
// const int board_index{ 4 };
// const int mobility_index{ 1 };
// const int indici_index{ 4 };

const size_t agent_offset{ 1UL };
const size_t steps_offset{ 3UL };
const size_t board_offset{ 2UL };
const size_t indici_offset{ 5UL };
const size_t actions_offset{ 5UL };

const size_t board_size{ 30UL };
const size_t max_moves{ 7UL };

const float P[5]{ 1, 1, 1, 1, 1 };
const int max_deque{ 524288 }; // 0.5 Mb
const int max_branching{ 7 };

enum house_id
{
  HOUSE_OF_REBIRTH = 14,
  HOUSE_OF_BEAUTY = 26,
  HOUSE_OF_WATERS = 26,
  HOUSE_OF_TRUTHS = 27,
  HOUSE_OF_ATOUM = 28,
  HOUSE_OF_SCARAB = 29
};

// board cell unit
enum unit_id
{
  UNIT_X,
  UNIT_Y,
  UNIT_NONE
};

// move event type
enum action_id
{
  ACTION_SKIP,
  ACTION_MOVE,
  ACTION_RETREAT,
  ACTION_ATTACK,
  ACTION_ESCAPE,
  ACTION_DROW
};

// game rules
enum rules_id
{
  RULES_MEUB,
  RULES_KENDALL
};
} // namespace constants
class Moves;
class Event;
class Emax;

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
      uint64_t _seed;
      struct
      {
        uint64_t _agent : 1;  // current ply active agent
        uint64_t _steps : 3;  // 1-5, 0 for unset
        uint64_t _board : 60; // 30x array,
      };
    };
  } bitfield;
  bitfield _data{ ._seed = 0UL };

public:
  using seed_type = uint64_t;
  template <class Iterator>
  uint64_t seed (constants::unit_id _agent, int _steps, Iterator _board)
  {
    bitfield data{ ._seed = 0UL };
    data._agent = _agent;
    data._steps = _steps;
    data._board = bitf::solid::set_bulk<Iterator, uint64_t> (
        _board, _board + constants::board_size, 0UL, constants::board_offset);
    return data._seed;
  }

  State () { _data._seed = constants::def_state; }
  State (uint64_t _seed) { _data._seed = _seed; }
  State (const State &other) : _data (other._data) {}
  State (State &&other) : _data (other._data) {}

  State (constants::unit_id _agent, int _steps, int *_board)
  {
    _data._seed = seed (_agent, _steps, _board);
  }
  State (constants::unit_id _agent, int _steps,
         std::vector<int>::iterator _board)
  {
    _data._seed = seed (_agent, _steps, _board);
  }

  ~State () = default;

  State &operator= (const State &other);
  State &operator= (State &&other);
  void set_agent (constants::unit_id _agent);
  void set_steps (int _steps);

  template <class Iterator>
  Iterator
  board (Iterator buffer)
  {
    bitf::solid::get_bulk<Iterator, uint64_t> (buffer, buffer + constants::board_size,
                                               _data._board, constants::board_offset);
    return buffer;
  }
  Moves moves (constants::rules_id) const;
  Event increment (constants::rules_id) const;

  float evaluate () const;

  constants::unit_id agent () const;
  int steps () const;

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
      uint64_t _seed;
      struct
      {
        uint64_t _agent : 1;
        uint64_t _mobility : 3;
        uint64_t _direction : 4;
        uint64_t _indici : 35;
        uint64_t _actions : 21;
      };
    };
  } bitfield;
  bitfield _data{ ._seed = 0UL };

public:
  friend class State;
  using seed_type = uint64_t;
  template <class Iterator>
  uint64_t
  seed (constants::unit_id _agent, constants::action_id _direction,
               int _mobility, Iterator _indici, Iterator _actions)
  {
    bitfield data{ ._seed = 0UL };
    data._agent = _agent;
    data._mobility = _mobility;
    data._direction = _direction;
    data._indici = bitf::solid::set_bulk<Iterator, uint64_t> (
        _indici, _indici + constants::max_moves, 0UL, constants::indici_offset);
    data._actions = bitf::solid::set_bulk<Iterator, uint64_t> (
        _actions, _actions + constants::max_moves, 0UL, constants::actions_offset);
    return data._seed;
  }
  template <class Iterator>
  uint64_t
  seed (constants::unit_id _agent, constants::action_id _direction,
               int _mobility, Iterator _indici)
  {
    bitfield data{ ._seed = 0UL };
    data._agent = _agent;
    data._mobility = _mobility;
    data._direction = _direction;
    data._indici = bitf::solid::set_bulk<Iterator, uint64_t> (
        _indici, _indici + constants::max_moves, 0UL, constants::indici_offset);
    return data._seed;
  }

  Moves () = default;
  Moves (uint64_t _seed) { _data._seed = _seed; }
  Moves (const Moves &other) : _data (other._data) {}
  Moves (Moves &&other) : _data (other._data) {}

  Moves (constants::unit_id _agent, constants::action_id _direction,
         int _mobility, int *_indici, int *_actions)
  {
    _data._seed
        = seed (_agent, _direction, _mobility, _indici, _actions);
  }
  Moves (constants::unit_id _agent, constants::action_id _direction,
         int _mobility, std::vector<int>::iterator _indici,
         std::vector<int>::iterator _actions)
  {
    _data._seed
        = seed (_agent, _direction, _mobility, _indici, _actions);
  }

  Moves (constants::unit_id _agent, constants::action_id _direction,
         int _mobility, int *_indici)
  {
    _data._seed = seed (_agent, _direction, _mobility, _indici);
  }
  Moves (constants::unit_id _agent, constants::action_id _direction,
         int _mobility, std::vector<int>::iterator _indici)
  {
    _data._seed = seed (_agent, _direction, _mobility, _indici);
  }

  Moves (const State &state, constants::rules_id rules)
  {
    _data = state.moves (rules)._data;
  }

  ~Moves () = default;

  Moves &operator= (const Moves &other);
  Moves &operator= (Moves &&other);

  template <class Iterator>
  Iterator
  indici (Iterator buffer)
  {
    bitf::solid::get_bulk<Iterator, uint64_t> (buffer, buffer + constants::max_moves,
                                               _data._indici, constants::indici_offset);
    return buffer;
  }

  template <class Iterator>
  Iterator
  actions (Iterator buffer)
  {
    bitf::solid::get_bulk<Iterator, uint64_t> (buffer, buffer + constants::max_moves,
                                               _data._actions, constants::actions_offset);
    return buffer;
  }
  constants::unit_id agent () const;
  int mobility () const;
  constants::action_id direction () const;
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
      uint16_t _seed;
      struct
      {
        uint16_t _agent : 1;
        uint16_t _action : 5;
        uint16_t _start : 5;
        uint16_t _destination : 5;
      };
    };
  } bitfield;
  bitfield _data{ ._seed = 0UL };

public:
  friend class State;
  using seed_type = uint16_t;
  static uint64_t
  seed (constants::unit_id _agent, constants::action_id _action, int _start,
        int _destination)
  {
    bitfield data{ ._seed = 0 };
    data._agent = _agent;
    data._action = _action;
    data._start = _start;
    data._destination = _destination;
    return data._seed;
  }

  Event () = default;
  Event (uint16_t seed) { _data._seed = seed; }
  Event (const Event &other) : _data (other._data) {}
  Event (Event &&other) : _data (other._data) {}

  Event (constants::unit_id _agent, constants::action_id _action, int _start,
         int _destination)
  {
    _data._seed = seed (_agent, _action, _start, _destination);
  }

  ~Event () = default;

  Event &operator= (const Event &other);
  Event &operator= (Event &&other);

  constants::unit_id agent () const;
  constants::action_id action () const;
  int start () const;
  int destination () const;
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
