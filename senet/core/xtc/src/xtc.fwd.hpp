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

const size_t board_size { 30UL };
const size_t max_moves  { 7UL };


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
}

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

class State
{
  /**
   * 1 bit: agent
   * 3 bits: steps
   * 30x2bits: board
   */
  typedef struct {
    union {
        uint64_t _seed; 
        struct {
            uint64_t _agent:1; // current ply active agent 
            uint64_t _steps:3; // 1-5, 0 for unset
            uint64_t _board:60;// 30x array, 
        };
    };
  } bitfield;
  bitfield _data {._seed=0UL};

public:
  using seed_type = uint64_t;
  template <class Iterator>
  static uint64_t seed (constants::unit_id agent, int steps, Iterator board);

  State() {_data._seed = constants::def_state; }
  State(uint64_t seed) { _data._seed = seed; }
  State(const State &other) : _data(other._data){}
  State(State &&other) : _data(other._data){}

  State(constants::unit_id agent, int steps, int* board) 
  { _data._seed = State::seed(agent, steps, board); }
  State(constants::unit_id agent, int steps, std::vector<int>::iterator board) 
  { _data._seed = State::seed(agent, steps, board); }
  
  ~State() = default;

  State &operator=(const State &other);
  State &operator=(State &&other);
  void set_agent(constants::unit_id agent);
  void set_steps(int steps);

  template <class Iterator> Iterator board (Iterator) const;
  Moves moves (constants::rules_id) const;
  Event increment (constants::rules_id) const;
  
  float evaluate () const;

  constants::unit_id agent () const;
  int steps () const;

} // namespace state

class Moves
{
  /**
   * 1 bit: agent
   * 3 bits: mobility
   * 7x5bits: indici
   * 7x3bits: actions
   */
  typedef struct {
    union {
        uint64_t _seed; 
        struct {
            uint64_t _agent:1;
            uint64_t _mobility:3;
            uint64_t _direction:4;
            uint64_t _indici:35;
            uint64_t _actions:21;
            
        };
    };
  } bitfield;
  bitfield _data {._seed=0UL};
  
public:
  friend class State;
  using seed_type = uint64_t;
  template <class Iterator>
  static uint64_t seed (constants::unit_id agent, constants::action_id direction, int mobility, Iterator indici, Iterator actions);
  template <class Iterator>
  static uint64_t seed (constants::unit_id agent, constants::action_id direction, int mobility, Iterator indici); //for internal use

  Moves() = default;
  Moves(uint64_t seed) { _data._seed = seed; }
  Moves(const Moves &other) : _data(other._data){}
  Moves(Moves &&other) : _data(other._data){}
  
  Moves(constants::unit_id agent, constants::action_id direction, int mobility, int* indici, int* actions)
   { _data._seed = Moves::seed(agent, direction, mobility, indici, actions); }
  Moves(constants::unit_id agent, constants::action_id direction, int mobility, std::vector<int>::iterator indici, std::vector<int>::iterator actions)
   { _data._seed = Moves::seed(agent, direction, mobility, indici, actions); }

  Moves(constants::unit_id agent, constants::action_id direction, int mobility, int* indici)
   { _data._seed = Moves::seed(agent, direction, mobility, indici); }
  Moves(constants::unit_id agent, constants::action_id direction, int mobility, std::vector<int>::iterator indici)
   { _data._seed = Moves::seed(agent, direction, mobility, indici); }

  Moves(const State &state, constants::rules_id rules) { this = state.moves(rules); }
  
  ~Moves() = default;

  State &operator=(const Moves &other) ;
  State &operator=(Moves &&other);

  template <class Iterator> Iterator indici (Iterator) const;
  template <class Iterator> Iterator actions (Iterator) const;

  constants::unit_id agent () const;
  int mobility () const;
  constants::action_id direction () const;
} // namespace moves

class Event
{
/**
   * 1 bit: agent
   * 3 bits: mobility
   * 7x5bits: indici
   * 7x3bits: actions
   */
  typedef struct {
    union {
        uint16_t _seed; 
        struct {
            uint16_t _agent:1;
            uint16_t _action:5;
            uint16_t _start:5;
            uint16_t _destination:5;
        };
    };
  } bitfield;
  bitfield _data {._seed=0UL};
public:
friend class State;
using seed_type = uint16_t;
static uint64_t seed (constants::unit_id agent, constants::action_id action, int start, int _destination)
{
  bitfield data {._seed=0};
  data._agent = agent;
  data._action = action;
  data._start = start;
  data._destination = destination;
  return data._seed;
}

  Event() = default;
  Event(uint16_t seed) { _data._seed = seed; }
  Event(const Event &other) : _data(other._data){}
  Event(Event &&other) : _data(other._data){}
  
  Event(constants::unit_id agent, constants::action_id action, int start, int _destination)
   { _data._seed = Moves::seed(agent, action, start, destination); }


  ~Event() = default;

  Event &operator=(const Event &other);
  Event &operator=(Event &&other);
 
  constants::agent_id agent () const;
  constants::action_id action () const;
  int start () const;
  int destination () const;
} // namespace event

//functor class
class Emax
{
  public:
  Emax() = default;
  Emax(const Emax &other) = default;
  Emax(Emax &&other) = default;
  ~Emax() = default;

  int operator()(State state);

} 

} // namespace xtc
