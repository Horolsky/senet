/**
 * game state node classes
 */
#pragma once
#include "xtc.fwd.hpp"
#include "xtc.strategies.hpp"
namespace xtc
{

/**
 * game state node
 * if 0 steps - chance node
 * else - strategy node
 */
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

public:
  friend ChanceNode;
  friend StrategyNode;

  using seed_type = uint64_t;
  // 0b1010101010101010101010101010101010101010010001000100010001000000
  static const uint64_t def_state_seed{ 12297829382466323520UL };
  static const int agent_offset{ 1 };
  static const int steps_offset{ 3 };
  static const int board_offset{ 2 };
  static const int board_size{ 30 };
  static uint64_t build_seed (Unit agent, int steps, int *board);

  State () { _data.seed = def_state_seed; }
  State (uint64_t seed) { _data.seed = seed; }

  State (const State &other) : _data (other._data) {}
  State (State &&other) : _data (other._data) {}

  State (const State &other, int steps) : _data (other._data)
  {
    _data.steps = steps;
  }
  State (State &&other, int steps) : _data (other._data)
  {
    _data.steps = steps;
  }

  State (const State &other, Unit agent) : _data (other._data)
  {
    _data.agent = static_cast<int> (agent);
  }
  State (State &&other, Unit agent) : _data (other._data)
  {
    _data.agent = static_cast<int> (agent);
  }

  State (const State &other, Unit agent, int steps) : _data (other._data)
  {
    _data.agent = static_cast<int> (agent);
    _data.steps = steps;
  }
  State (State &&other, Unit agent, int steps) : _data (other._data)
  {
    _data.agent = static_cast<int> (agent);
    _data.steps = steps;
  }

  State (Unit agent, int steps, int *board)
  {
    _data.seed = build_seed (agent, steps, board);
  }

  ~State () = default;

  State &operator= (const State &other);
  State &operator= (State &&other);

  /**
   * loads game state to an array buffer of size 30
   * return buffer pointer
   * lookup! raw pointers, size not checked
   */
  int *board (int *buffer) const;
  // return unit id on a given index
  Unit board (int) const;

  Unit agent () const;
  int steps () const;
  uint64_t seed () const;

  bool is_chance_node () const;
  bool is_strategy_node () const;
  bool is_terminal_node () const;

private:
  void update_board (int index, Unit unit);
}; // class State

/**
 * ChanceNode with chance (steps) = 0.
 * cannot be incremented
 * child(int chance) returns StrategyNode
 */ 
class ChanceNode : public State
{
public:
  ChanceNode () { _data.seed = State::def_state_seed; }
  ChanceNode (uint64_t seed)
  {
    _data.seed = seed;
    _data.steps = 0;
  }

  ChanceNode (const State &other) { _data = other._data, _data.steps = 0; }
  ChanceNode (State &&other) { _data = other._data, _data.steps = 0; }

  ChanceNode (const State &other, Unit agent)
  {
    _data = other._data;
    _data.agent = static_cast<int> (agent);
  }
  ChanceNode (State &&other, Unit agent)
  {
    _data = other._data;
    _data.agent = static_cast<int> (agent);
  }
  StrategyNode child(int chance) const;
};

/**
 * StrategyNode with non-zero steps parameter.
 * if no steps passed to a constructor, 
 * this parameter derived from given seed or state
 * child(int chance) returns incremented state as ChanceNode
 */ 
class StrategyNode : public State
{
public:
  explicit StrategyNode (uint64_t seed)
  {
    _data.seed = seed;
    if (_data.steps == 0)
      throw std::logic_error ("zero steps in StrategyNode");
  }
  
  StrategyNode (const State &other, int steps = 0)
  {
    _data = other._data;
    if (steps) _data.steps = steps;
    if (!_data.steps)
      throw std::logic_error ("zero steps in StrategyNode");
  }

  StrategyNode (State &&other, int steps = 0) 
  {
    _data = other._data;
    if (steps) _data.steps = steps;
    if (!_data.steps)
      throw std::logic_error ("zero steps in StrategyNode");
  }

  StrategyNode (const State &other, Unit agent, int steps = 0)
  {
    _data = other._data;
    _data.agent = static_cast<int> (agent);
    if (steps) _data.steps = steps;
    if (!_data.steps)
      throw std::logic_error ("zero steps in StrategyNode");
  }
  StrategyNode (State &&other, Unit agent, int steps = 0)
  {
    _data = other._data;
    _data.agent = static_cast<int> (agent);
    if (steps) _data.steps = steps;
    if (!_data.steps)
      throw std::logic_error ("zero steps in StrategyNode");
  }
  Strategies strategies() const;
  ChanceNode child(int i, Strategies strategies = Strategies(0UL)) const;
};

} // namespace xtc
